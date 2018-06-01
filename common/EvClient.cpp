
/*
 * =========================================================================
 *
 *       FileName:  EvClient.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-26 09:45:30
 *  Last Modified:  2018-04-26 10:47:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "EvClient.h"

EvClient::EvClient ( std::string selfname, std::string server, uint16_t port )
  : m_base ( NULL ),
    m_bev ( NULL ),
    m_evtimer ( NULL ),
    m_ip_server ( server ),
    m_port_server ( port ),
    m_selfname ( selfname )
{
  evthread_use_pthreads();
  m_base = event_base_new();

  Connect();

  m_evtimer = event_new ( m_base, -1, EV_TIMEOUT | EV_PERSIST, TimerCbkHeartBeat, ( void* ) this );
  timeval tv = { 5, 0};
  event_add ( m_evtimer, &tv );

  m_thread_event = std::thread ( event_base_dispatch, m_base );
}

EvClient::~EvClient()
{
  timeval delay = { 0, 300 * 1000 };
  event_base_loopexit ( m_base, &delay );

  if ( m_thread_event.joinable() )
    m_thread_event.join();

  if ( m_evtimer )
    evtimer_del ( m_evtimer );

  if ( m_bev )
    bufferevent_free ( m_bev );

  if ( m_base )
    event_base_free ( m_base );
}

bool EvClient::Connect()
{
  if ( m_bev )
    return true;

  m_bev = bufferevent_socket_new ( m_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE );

  if ( !m_bev )
    return false;

  memset ( &m_addr_server, 0, sizeof ( m_addr_server ) );
  m_addr_server.sin_family = AF_INET;
  m_addr_server.sin_port = htons ( m_port_server );
  m_addr_server.sin_addr.s_addr = inet_addr ( m_ip_server.c_str() );

  bufferevent_socket_connect ( m_bev, reinterpret_cast<sockaddr*> ( &m_addr_server ), sizeof ( m_addr_server ) );

  bufferevent_setcb ( m_bev, ConnReadCb, NULL, ConnEventCb, reinterpret_cast<void*> ( this ) );
  bufferevent_enable ( m_bev, EV_READ | EV_PERSIST );

  std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
  return true;
}

void EvClient::ConnWriteCb ( bufferevent* bev, void* user_data )
{
  EvClient* pObj = reinterpret_cast<EvClient*> ( user_data );
  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  evbuffer* ouput  = bufferevent_get_output ( bev );

  if ( evbuffer_get_length ( ouput ) == 0 )
  {
    //bufferevent_free ( bev );
  }
}

void EvClient::ConnReadCb ( bufferevent* bev, void* user_data )
{
  EvClient* pObj = reinterpret_cast<EvClient*> ( user_data );

  size_t len = 0;
  int cnt = 0;
  std::vector<uint8_t> vecBuffer;
  std::lock_guard<std::mutex> lck ( pObj->m_mutex );
  cnt = evbuffer_get_length ( bufferevent_get_input ( bev ) );

  if ( cnt <= 0 )
    return;

  vecBuffer.resize ( cnt, 0 );
  len = bufferevent_read ( bev, &vecBuffer[0], vecBuffer.size() );
  auto& vec = pObj->m_vecBuffer;

  if ( len > 0 )
  {
    vecBuffer.resize ( len );

    if ( vec.size() > 32 * 1024 )
      vec.erase ( vec.begin(), vec.begin() + len );

    vec.insert ( vec.end(), vecBuffer.begin(), vecBuffer.end() );
  }

  //fprintf ( stderr, "read %d\n", ( int ) vec.size() );
}

void EvClient::ConnEventCb ( bufferevent* bev, short events, void* user_data )
{
  EvClient* pObj = reinterpret_cast<EvClient*> ( user_data );
  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  if ( events & BEV_EVENT_READING)
  {
    fprintf ( stderr, "bev %p BEV_EVENT_READING\n", bev );
  }
  else if ( events & BEV_EVENT_WRITING)
  {
    fprintf ( stderr, "bev %p BEV_EVENT_WRITING  %s\n", bev, strerror ( errno ) );
  }
  else if ( events & BEV_EVENT_EOF)
  {
    fprintf ( stderr, "bev %p BEV_EVENT_EOF %s\n", bev, strerror ( errno ) );
  }
  else if ( events & BEV_EVENT_ERROR )
  {
    fprintf ( stderr, "bev %p BEV_EVENT_ERROR %s\n", bev, strerror ( errno ) );
  }
  else if ( events & BEV_EVENT_TIMEOUT)
  {
    fprintf ( stderr, "bev %p BEV_EVENT_TIMEOUT %s\n", bev, strerror ( errno ) );
  }
  else if ( events & BEV_EVENT_CONNECTED )
  {
    //fprintf ( stderr, "bev %p BEV_EVENT_CONNECTED %s\n", bev, strerror ( errno ) );
    return;
  }

  bufferevent_free ( bev );
  pObj->m_bev = NULL;
  pObj->m_vecBuffer.clear();
  /* reconnect */
  pObj->Connect();
}

void EvClient::TimerCbkHeartBeat ( int fd, short event, void* user_data )
{
  ( void ) fd;
  ( void ) event;
  EvClient* pObj = reinterpret_cast<EvClient*> ( user_data );
  std::lock_guard<std::mutex> lck ( pObj->m_mutex );

  std::string selfname = pObj->m_selfname;
  std::vector<uint8_t> data ( selfname.size() + 1, 0 );

  memcpy ( &data[0], selfname.c_str(), selfname.size() );

  std::vector<uint8_t> vec_heartbeat = MakeProtocol ( PROTOCOL_HEARTBEAT, data.size(), &data[0] );

  if ( pObj->m_bev )
    bufferevent_write ( pObj->m_bev, reinterpret_cast<void*> ( &vec_heartbeat[0] ), vec_heartbeat.size() );
}

bool EvClient::GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocol )
{
  return GetOneProtocol ( vecBuffer, vecProtocol );
}

int EvClient::SendToServer ( std::vector<uint8_t>& vecProtocol )
{
  return bufferevent_write ( m_bev, reinterpret_cast<void*> ( &vecProtocol[0] ), vecProtocol.size() );
}

bool EvClient::GetMsg ( std::string& msgname, std::vector<uint8_t>& vecmsg )
{
  std::lock_guard<std::mutex> lck ( m_mutex );

  if ( !GetOneProtocol ( m_vecBuffer, vecmsg ) )
    return false;

  vecmsg.erase ( vecmsg.begin(), vecmsg.begin() + PROTOCOL_HEADER_SIZE );

  msgname = reinterpret_cast<char*> ( &vecmsg[0] );
  vecmsg.erase ( vecmsg.begin(), vecmsg.begin() + msgname.size() + 1 );
  //vecmsg.insert ( vecmsg.end(), 0 );

  return true;
}

void EvClient::PubMsg ( std::string msgname, std::vector<uint8_t> vecmsg )
{
  std::lock_guard<std::mutex> lck ( m_mutex );
  vecmsg.insert ( vecmsg.begin(), 0x0 );
  vecmsg.insert ( vecmsg.begin(), msgname.data(), msgname.data() + msgname.size() );

  std::vector<uint8_t> protocol =  MakeProtocol ( PROTOCOL_PUB, vecmsg.size(), &vecmsg[0] );
  SendToServer ( protocol );
}

bool EvClient::SubMsg ( std::string msgname )
{
  std::lock_guard<std::mutex> lck ( m_mutex );
  std::vector<uint8_t> data ( msgname.size() + 1, 0 );
  memcpy ( &data[0], msgname.c_str(), msgname.size() );

  std::vector<uint8_t> protocol =  MakeProtocol ( PROTOCOL_SUB, data.size(), &data[0] );
  return ( 0 == SendToServer ( protocol ) );
}

void EvClient::SubMsgCancel ( std::string msgname )
{
  std::lock_guard<std::mutex> lck ( m_mutex );
  std::vector<uint8_t> data ( msgname.size() + 1, 0 );
  memcpy ( &data[0], msgname.c_str(), msgname.size() );

  std::vector<uint8_t> protocol =  MakeProtocol ( PROTOCOL_SUB_CANCEL, data.size(), &data[0] );
  SendToServer ( protocol );
}

int EvClient::PubLog ( std::string level, std::string filename,
                       int fileline, std::string funcname, const char* fmt, ... )
{
  std::stringstream ss;
  ss << level << " " << filename << " " << fileline << " " << funcname << ": ";

  std::vector<char> strBuffer ( 512, 0 );

  va_list arg;

  va_start ( arg, fmt );
  vsnprintf ( &strBuffer[0], strBuffer.size(), fmt, arg );
  va_end ( arg );

  ss << &strBuffer[0];
  ss.str().back() = 0x0;

  std::string msgstr = ss.str();

  std::vector<uint8_t> vecmsg;
  vecmsg.insert ( vecmsg.begin(), msgstr.data(), msgstr.data() + msgstr.size() );

  PubMsg ( "log", vecmsg );

  return 0;
}

bool EvClient::GetLog ( std::string& logstr )
{
  std::string msgname;
  std::vector<uint8_t> msg;

  if ( GetMsg ( msgname, msg ) && msgname == "log" )
  {
    logstr.clear();

    for ( auto& c : msg )
      logstr.push_back ( c );

    return true;
  }

  return false;
}

#ifdef TEST_EVCLIENT

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
  std::vector<uint8_t> msg ( 5, 0x31 );

  EvClient b ( "test" );
  //b.PubLog ( "info", __FILE__, __LINE__, __FUNCTION__, "0x%02x %p %s\n", 122, NULL, "test" );
  //b.PRINT_LOG ( "info", "0x%02x %p %s\n", 122, NULL, "test" );

  while ( runflag )
  {
    b.PRINT_LOG ( LOG_LEVEL_DEBUG, "0x%02x %p %s\n", 122, NULL, "test" );
    std::this_thread::sleep_for ( std::chrono::seconds ( 3 ) );
  }

  return 0;
}

#endif


