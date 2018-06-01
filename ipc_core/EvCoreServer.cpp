
/*
 * =========================================================================
 *
 *       FileName:  EvCoreServer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-24 10:31:13
 *  Last Modified:  2018-03-05 09:11:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "EvCoreServer.h"


EvCoreServer::EvCoreServer()
  : EvTcpHandler ( 18888 ),
    m_runFlag ( false )
{
  Start();
  m_runFlag = true;
  m_thread_core = std::thread ( &EvCoreServer::CoreTask, this );
}

EvCoreServer::~EvCoreServer()
{
  m_runFlag = false;

  if ( m_thread_core.joinable() )
    m_thread_core.join();
}

void EvCoreServer::CoreTask()
{
  while ( m_runFlag )
  {
    //CheckClientStatus();
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
  }

  //fprintf ( stderr, "exit %s\n", __FUNCTION__ );
}

void EvCoreServer::CheckClientStatus()
{
  std::lock_guard<std::mutex> lck ( m_mutex );

  time_t tm;
  time ( &tm );

  for ( auto iter = m_map_bfevnt_heartbeatinfo.begin(); iter != m_map_bfevnt_heartbeatinfo.end(); )
  {
    if ( tm - iter->second.tm >= 5 )
    {
      fprintf ( stderr, "%s tm_delta=%d\n", iter->second.name.c_str(), ( int ) ( tm - iter->second.tm ) );
      m_map_bfevnt_heartbeatinfo.erase ( iter++ );
    }
    else
      ++iter;
  }
}

bool EvCoreServer::GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal )
{
  return GetOneProtocol ( vecBuffer, vecProtocal );
}

void EvCoreServer::HandleOneProtocolData ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  std::lock_guard<std::mutex> lck ( m_mutex );

  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vecProtocal[0] );
  //fprintf ( stderr, "type=%d\n", p->type );

  if ( PROTOCOL_HEARTBEAT == p->type )
    ProcHeartBeat ( bev, vecProtocal );
  else if ( PROTOCOL_PUB == p->type )
    ProcPub ( bev, vecProtocal );
  else if ( PROTOCOL_PUB_QUERY == p->type )
    ProcPubQuery ( bev, vecProtocal );
  else if ( PROTOCOL_SUB == p->type )
    ProcSub ( bev, vecProtocal );
  else if ( PROTOCOL_SUB_CANCEL == p->type )
    ProcSubCancel ( bev, vecProtocal );
  else
    SendDataToClient ( bev, vecProtocal );
}

void EvCoreServer::ProcHeartBeat ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vecProtocal[0] );
  char* pstr = reinterpret_cast<char*> ( p->data );
  std::string str = pstr;

  auto iter = m_map_bfevnt_heartbeatinfo.find ( bev );

  if ( iter == m_map_bfevnt_heartbeatinfo.end() )
  {
    m_map_bfevnt_heartbeatinfo.insert ( {bev, HeartBeatInfo ( str ) } );
  }
  else
  {
    iter->second.name = str;
    time ( & ( iter->second.tm ) );
  }

  //fprintf ( stderr, "HeartBeatInfo %p %s %08x\n", bev, str.c_str(), ( uint32_t ) iter->second.tm );
}

void EvCoreServer::ProcPub ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  ( void ) bev;
  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vecProtocal[0] );

  std::string str = reinterpret_cast<char*> ( p->data );

  auto iter = m_map_pubname_listsubbev.find ( str );

  if ( iter != m_map_pubname_listsubbev.end() )
  {
    auto& listsubbev = iter->second;

    for ( auto it = listsubbev.begin(); it != listsubbev.end(); )
    {
      if ( SendDataToClient ( *it, vecProtocal ) != 0 )
      {
        fprintf ( stderr, "listsubbev.erase %p\n", *it );
        listsubbev.erase ( it++ );
      }
      else
        ++it;
    }
  }
  else
  {
    m_map_pubname_listsubbev.insert ( { str, std::list<bufferevent*>() } );
  }
}

void EvCoreServer::ProcPubQuery ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  ( void ) bev;
  ( void ) vecProtocal;

  for ( auto& e : m_map_pubname_listsubbev )
  {
    fprintf ( stderr, "%s\n", e.first.c_str() );
  }
}

void EvCoreServer::ProcSub ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vecProtocal[0] );
  char* pstr = reinterpret_cast<char*> ( p->data );
  std::string str = pstr;
  auto iter = m_map_pubname_listsubbev.find ( str );

  if ( iter != m_map_pubname_listsubbev.end() )
    iter->second.push_front ( bev );
  else
  {
    auto v = std::make_pair ( str, std::list<bufferevent*>() );
    v.second.push_front ( bev );
    m_map_pubname_listsubbev.insert ( v );
  }

  fprintf ( stderr, "%p sub %s\n", bev, str.c_str() );
}

void EvCoreServer::ProcSubCancel ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vecProtocal[0] );
  char* pstr = reinterpret_cast<char*> ( p->data );
  std::string str = pstr;

  auto iter = m_map_pubname_listsubbev.find ( str );

  if ( iter != m_map_pubname_listsubbev.end() )
    iter->second.remove ( bev );
}

#ifdef TEST

bool runflag = true;
void SigInt ( int signal )
{
  runflag = false;
}

int main ( int argc, char* argv[] )
{
  ( void ) argc;
  ( void ) argv;

  signal ( SIGINT,  SigInt );

  EvCoreServer a;

  while ( runflag )
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );

  return 0;
}
#endif


