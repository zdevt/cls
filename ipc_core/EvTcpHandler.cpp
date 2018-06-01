
/*
 * =========================================================================
 *
 *       FileName:  EvTcpHandler.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-23 08:21:57
 *  Last Modified:  2018-01-27 15:32:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "EvTcpHandler.h"


EvTcpHandler:: EvTcpHandler ( uint16_t port )
  : m_runFlag ( false ),
    m_base ( NULL ),
    m_listenner ( NULL ),
    m_port ( port )
{
  Init();
}

EvTcpHandler::~EvTcpHandler()
{
  Deinit();
}

void EvTcpHandler::Init()
{
  evthread_use_pthreads();
  m_base = event_base_new();

  if ( !m_base )
  {
    fprintf ( stderr, "m_base==NULL\n" );
    return;
  }

  sockaddr_in addr;
  memset ( &addr, 0, sizeof ( addr ) );
  addr.sin_family = AF_INET;
  addr.sin_port = htons ( m_port );

  m_listenner = evconnlistener_new_bind ( m_base,
                                          ListenerCb, this,
                                          LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                          ( sockaddr* ) &addr, sizeof ( addr ) );

  if ( m_listenner )
  {
    m_thread_event = std::thread ( &EvTcpHandler::TaskEvent, this );
  }
  else
    fprintf ( stderr, "listener==NULL\n" );
}

void EvTcpHandler::Deinit()
{
  m_runFlag = false;

  if ( m_thread_getpro_and_hand.joinable() )
    m_thread_getpro_and_hand.join();

  timeval delay = { 0, 300 * 1000 };
  event_base_loopexit ( m_base, &delay );

  if ( m_thread_event.joinable() )
    m_thread_event.join();

  for ( auto& e : m_map_bfevnt_vecuint8 )
  {
    fprintf ( stderr, "free %p\n", e.first );
    bufferevent_free ( e.first );
  }

  evconnlistener_free ( m_listenner );
  event_base_free ( m_base );
}

void EvTcpHandler::TaskEvent()
{
  event_base_dispatch ( m_base );
  //fprintf ( stderr, "exit %s\n", __FUNCTION__ );
}

void EvTcpHandler::Start()
{
  if ( !m_runFlag && m_listenner )
  {
    m_runFlag = true;
    m_thread_getpro_and_hand = std::thread ( &EvTcpHandler::TaskGetProtocolDataAndHandle, this );
  }
}

void EvTcpHandler::ListenerCb ( evconnlistener* listenner, evutil_socket_t fd, sockaddr* sa, int socklen, void* user_data )
{
  ( void ) listenner;
  ( void ) socklen;
  ( void ) sa;

  EvTcpHandler* pObj = reinterpret_cast<EvTcpHandler*> ( user_data );
  evutil_make_socket_nonblocking ( fd );

  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  bufferevent* bev = bufferevent_socket_new ( pObj->m_base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE );

  if ( bev )
  {
    bufferevent_setcb ( bev, ConnReadCb, NULL, ConnEventCb, user_data );
    bufferevent_enable ( bev, EV_WRITE | EV_READ );
    pObj->m_map_bfevnt_vecuint8.insert ( { bev, std::vector<uint8_t>() } );

    #if 1
    fprintf ( stderr, "bev %p from ip=%s port=%d connected fd=%d\n",
              bev, inet_ntoa ( ( ( sockaddr_in* ) sa )->sin_addr ),
              ntohs ( ( ( sockaddr_in* ) sa )->sin_port ),
              fd );
    #endif
  }
}

void EvTcpHandler::ConnWriteCb ( bufferevent* bev, void* user_data )
{
  EvTcpHandler* pObj = reinterpret_cast<EvTcpHandler*> ( user_data );
  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  evbuffer* ouput  = bufferevent_get_output ( bev );

  if ( evbuffer_get_length ( ouput ) == 0 )
  {
    //bufferevent_free ( bev );
  }
}

void EvTcpHandler::ConnReadCb ( bufferevent* bev, void* user_data )
{
  EvTcpHandler* pObj = reinterpret_cast<EvTcpHandler*> ( user_data );

  size_t len = 0;
  int cnt = 0;
  std::vector<uint8_t> vecBuffer;

  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  cnt = evbuffer_get_length ( bufferevent_get_input ( bev ) );

  if ( cnt <= 0 )
    return;

  vecBuffer.resize ( cnt, 0 );
  len = bufferevent_read ( bev, &vecBuffer[0], vecBuffer.size() );

  if ( len > 0 )
  {
    auto iter = pObj->m_map_bfevnt_vecuint8.find ( bev );

    if ( iter == pObj->m_map_bfevnt_vecuint8.end() )
      return;

    auto& vec = iter->second;
    vecBuffer.resize ( len );

    if ( vec.size() > 32 * 1024 )
      vec.erase ( vec.begin(), vec.begin() + len );

    vec.insert ( vec.end(), vecBuffer.begin(), vecBuffer.end() );
    //fprintf ( stderr, "%p read %d\n", bev, ( int ) vec.size() );
  }
}

void EvTcpHandler::ConnEventCb ( bufferevent* bev, short events, void* user_data )
{
  ( void ) events;

  EvTcpHandler* pObj = reinterpret_cast<EvTcpHandler*> ( user_data );

  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  auto& map_bfevnt_vecuint8 = pObj->m_map_bfevnt_vecuint8;
  auto iter = map_bfevnt_vecuint8.find ( bev );

  #if 1

  if ( events & BEV_EVENT_EOF )
    fprintf ( stderr, "bev %p connect closed\n", bev );
  else if ( events & BEV_EVENT_ERROR )
    fprintf ( stderr, "bev %p got an error on the connection %s\n", bev, strerror ( errno ) );

  #endif

  if ( iter != map_bfevnt_vecuint8.end() )
  {
    bufferevent_free ( bev );
    map_bfevnt_vecuint8.erase ( bev );
    bev = NULL;
  }
}

int EvTcpHandler::SendDataToClient ( bufferevent* bev, std::vector<uint8_t>& data )
{
  std::lock_guard<std::mutex> lck ( m_mutex );

  auto iter = m_map_bfevnt_vecuint8.find ( bev );

  if ( iter != m_map_bfevnt_vecuint8.end() )
    return bufferevent_write ( bev, reinterpret_cast<void*> ( &data[0] ), data.size() );

  fprintf ( stderr, "bev %p deleted can't send data!\n", bev );

  return -1;
}

bool EvTcpHandler::DisconnectClient ( bufferevent* bev )
{
  std::lock_guard<std::mutex> lck ( m_mutex );

  auto iter = m_map_bfevnt_vecuint8.find ( bev );

  if ( iter != m_map_bfevnt_vecuint8.end() )
  {
    m_map_bfevnt_vecuint8.erase ( bev );
    bufferevent_free ( bev );
    bev = NULL;
    fprintf ( stderr, "disconected by server\n" );
    return true;
  }

  return false;
}

void EvTcpHandler::GetProtocalData ( std::map<bufferevent*, std::list<std::vector<uint8_t> > >& map_bfevnt_listvecProtocol )
{
  std::vector<uint8_t> vecProtocal;
  auto iter = map_bfevnt_listvecProtocol.begin();

  for ( auto& e : m_map_bfevnt_vecuint8 )
  {
    while ( GetOneProtocolData ( e.second, vecProtocal ) )
    {
      iter = map_bfevnt_listvecProtocol.find ( e.first );

      if ( iter == map_bfevnt_listvecProtocol.end() )
      {
        auto v = std::make_pair ( e.first, std::list<std::vector<uint8_t> >() );
        v.second.push_front ( vecProtocal );
        map_bfevnt_listvecProtocol.insert ( v );
      }
      else
        iter->second.push_front ( vecProtocal );
    }
  }
}

void EvTcpHandler::TaskGetProtocolDataAndHandle()
{
  while ( m_runFlag )
  {
    {
      std::lock_guard<std::mutex> lck ( m_mutex );
      GetProtocalData ( m_map_bfevnt_listvecProtocol );
    }

    if ( m_map_bfevnt_listvecProtocol.size() > 0 )
    {
      for ( auto& epair : m_map_bfevnt_listvecProtocol )
      {
        for ( auto& evec : epair.second )
          HandleOneProtocolData ( epair.first, evec );
      }

      m_map_bfevnt_listvecProtocol.clear();
    }
    else
      std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
  }

  //fprintf ( stderr, "exit %s\n", __FUNCTION__ );
}

#if 0
bool EvTcpHandler::GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal )
{
  if ( vecBuffer.size() > 0 )
  {
    vecProtocal = vecBuffer;
    vecBuffer.clear();
    return true;
  }

  return false;
}

void EvTcpHandler::HandleOneProtocolData ( bufferevent* bev, std::vector<uint8_t>& vecProtocal )
{
  SendDataToClient ( bev, vecProtocal );
}
#endif
