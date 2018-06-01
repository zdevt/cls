/*
 * =====================================================================================
 *
 *       Filename:  DebugModule.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2017年09月22日 08时53分59秒
 *  Last Modified:  2017年09月22日 08时53分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <iostream>

#include "DebugModule.h"
#include "LogPrint.h"
#include "NetworkServer.h"
#include "NetworkServerAdmin.h"
#include "Sqlite3InterfaceLog.h"

std::thread DebugModule::m_thread;

map_funname_pfun_t DebugModule::m_map_funname_pfun =
{
  {"PrintSock",     DebugModule::PrintSock },
  {"Log",           DebugModule::Log},
  {"ListAllClient", DebugModule::ListAllClient},
  {"Help",          DebugModule::Help},
  {"GetLog",        DebugModule::GetLog},
};

DebugModule::DebugModule()
{

}

DebugModule::~DebugModule()
{

}

void DebugModule::SendMsg ( int argc, char* argv[] )
{
  int i = 0;
  int msg_id = -1;
  std::string arg;

  debugMsg_t debugMsg;

  memset ( &debugMsg, 0, sizeof ( debugMsg ) );
  debugMsg.msgtype = MY_IPCMSGTYPE_ID;

  std::string pts;
  GetPts ( pts );

  arg = "/dev/";
  arg += pts;
  arg += " ";

  for ( i = 0; i < argc; i++ )
  {
    arg += argv[i];
    arg += " ";
  }

  strncpy ( debugMsg.buff, arg.c_str(), MAX_BUFF_SIZE );

  msg_id = msgget ( ( key_t ) MY_IPCMSGTYPE_ID, IPC_CREAT | 0666 );

  if ( msg_id == -1 )
    return;

  if ( -1 == msgsnd ( msg_id, &debugMsg, sizeof ( debugMsg_t ), 0 ) )
    std::cout << "Send Msg Error\n";

  return;
}


bool DebugModule::GetPts ( std::string& pts )
{
  char buffer[32];

  memset ( buffer, 0, 32 );
  FILE* fp = popen ( "who -m | awk '{print $2}'", "r" );

  if ( !fp )
    return false;

  fgets ( buffer, 32, fp );
  pclose ( fp );

  pts = buffer;

  pts.back() = ' ';

  return true;
}

bool DebugModule::RecvMsg ( std::string& strmsg )
{
  int msg_id = -1;
  debugMsg_t debugMsg;

  strmsg.clear();

  memset ( &debugMsg, 0, sizeof ( debugMsg ) );

  debugMsg.msgtype = MY_IPCMSGTYPE_ID;
  msg_id = msgget ( ( key_t ) MY_IPCMSGTYPE_ID, IPC_CREAT | 0666 );

  if ( msg_id < 0 )
    return false;

  if ( -1 != msgrcv ( msg_id, &debugMsg, sizeof ( debugMsg ), 0, IPC_NOWAIT ) )
  {
    strmsg = debugMsg.buff;
    return true;
  }

  return false;
}

void DebugModule::GetVecStr ( std::string strmsg, vecstr_t& vecstr )
{
  std::stringstream ssIn ( strmsg );
  std::string strTemp;

  while ( std::getline ( ssIn, strTemp, ' ' ) )
  {
    if ( strTemp.size() != 0 )
      vecstr.insert ( vecstr.end(), strTemp );
  }
}

void DebugModule::Task ( void )
{
  std::string strmsg;
  vecstr_t vecstr;

  while ( true )
  {
    if ( RecvMsg ( strmsg ) )
      GetVecStr ( strmsg, vecstr );

    if ( vecstr.size() > 2 )
    {
      auto iter = m_map_funname_pfun.find ( vecstr.at ( DEBUG_FUN_NAME_POS ) );

      if ( iter != m_map_funname_pfun.end() )
        ( iter->second ) ( vecstr );

      vecstr.clear();
    }

    usleep ( 10 * 1000 );
  }
}

void DebugModule::StartTask()
{
  m_thread = std::thread ( DebugModule::Task );
  m_thread.detach();
}

void DebugModule::PrintSock ( vecstr_t& vecstr )
{
  if ( vecstr.size() < 4 )
    return;

  if ( 0 == vecstr.at ( DEBUG_FUN_ARG_STARG_POS ).compare ( "on" ) )
  {
    LogPrint::SetPts2Name ( vecstr.at ( DEBUG_FUN_PTS_POS ) );
    NetworkServer::SetShowDataFlag ( true );
  }
  else
  {
    NetworkServer::SetShowDataFlag ( false );
  }
}


void DebugModule::Log ( vecstr_t& vecstr )
{
  if ( vecstr.size() < 4 )
    return;

  if ( 0 == vecstr.at ( DEBUG_FUN_ARG_STARG_POS ).compare ( "on" ) )
    LogPrint::SetPtsName ( vecstr.at ( DEBUG_FUN_PTS_POS ) );
  else
    LogPrint::SetPtsName ( " " );
}

void DebugModule::ListAllClient ( vecstr_t& vecstr )
{
  LogPrint::SetPts2Name ( vecstr.at ( DEBUG_FUN_PTS_POS ) );
  GET_NETWORKSERVERADMIN->PrintAllSockInfo();
}

void DebugModule::Help ( vecstr_t& vecstr )
{
  LogPrint::SetPts2Name ( vecstr.at ( DEBUG_FUN_PTS_POS ) );

  PRINT_LOG2 ( "\n" );
  for_each ( m_map_funname_pfun.begin(), m_map_funname_pfun.end(),
             [] ( map_funname_pfun_t::value_type m )
  {
    PRINT_LOG2 ( "%s\n", m.first.c_str() );
  } );
}

void DebugModule::GetLog ( vecstr_t& vecstr )
{
  if ( vecstr.size() < 4 )
    return;

  LogPrint::SetPts2Name ( vecstr.at ( DEBUG_FUN_PTS_POS ) );

  std::string sql;

  for ( auto iter = vecstr.begin() + 3; iter != vecstr.end(); ++iter )
  {
    sql += *iter;
    sql += " ";
  }

  vecstring_t vecstring;
  GET_LOGPDB->GetLog ( sql, vecstring );

  for ( auto& s : vecstring )
    PRINT_LOG2 ( "%s\n", s.c_str() );
}

#ifdef TEST
int main ( int argc, char* argv[] )
{
  std::string str;
  vecstr_t vecstr;

  DebugModule::SendMsg ( argc, argv );
  DebugModule::StartTask();

  while ( 1 )
  {
    sleep ( 1 );
  }

  return 0;
}

#endif

