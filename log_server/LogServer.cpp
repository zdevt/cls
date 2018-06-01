
/*
 * =========================================================================
 *
 *       FileName:  LogServer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-02-08 15:34:55
 *  Last Modified:  2018-03-06 15:17:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "LogServer.h"

LogServer::LogServer ( std::string server, uint16_t port )
  : EvClient ( "LogServer", server, port ),
    m_runFlag ( true )
{
  m_sqlite3log = Sqlite3InterfaceLog::GetInstance( );
  m_thread = std::thread ( &LogServer::LogTask, this );
}

LogServer::~LogServer()
{
  m_runFlag = false;

  if ( m_thread.joinable() )
    m_thread.join();
}

void LogServer::LogTask ( void )
{
  while ( ! SubMsg ( "log" ) )
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );

  fprintf ( stderr, "sub sucessed!\n" );

  while ( m_runFlag )
  {
    if ( GetLog ( m_logmsg ) )
    {
      //TODO
      m_sqlite3log->AddLog ( m_logmsg );
      //fprintf ( stderr, "GET LOG: %s", m_logmsg.c_str() );
    }
    else
      std::this_thread::sleep_for ( std::chrono::microseconds ( 500 ) );
  }
}

/* test */
bool runflag = true;
void SigInt ( int signal )
{
  ( void ) signal;
  runflag = false;
}

int main ( int argc, char* argv[] )
{
  ( void ) argc;
  ( void ) argv;

  signal ( SIGINT,  SigInt );

  /* demo usage*/
  std::shared_ptr<LogServer> b = NULL;

  if ( argc == 2 )
    b = std::make_shared< LogServer> ( argv[1] );
  else if ( argc == 3 )
    b = std::make_shared< LogServer> ( argv[1], atoi ( argv[2] ) );
  else
    b = std::make_shared< LogServer> ( );

  /* demo usage*/

  while ( runflag )
  {
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
  }

  return 0;
}

