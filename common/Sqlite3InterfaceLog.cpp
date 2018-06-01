/*
 * =====================================================================================
 *
 *       Filename:  Sqlite3InterfaceLog.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/14/2017 01:54:42 PM
 *  Last Modified:  2018-03-08 10:14:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */

#include "Sqlite3InterfaceLog.h"

Sqlite3InterfaceLog::Sqlite3InterfaceLog ( std::string dbFilename )
  : Sqlite3Interface ( dbFilename )
{

}

Sqlite3InterfaceLog::~Sqlite3InterfaceLog()
{

}

Sqlite3InterfaceLog* Sqlite3InterfaceLog::GetInstance( )
{
  static Sqlite3InterfaceLog instance ( SQLITE3_SEISLOG_NAME );
  return &instance;
}

bool Sqlite3InterfaceLog::AddLog ( std::string logstr )
{
  std::stringstream ss;

  ss << "create table if not exists LOG";
  ss << "(ID integer primary key,";
  ss << "DATE TimeStamp NOT NULL DEFAULT(datetime('now','localtime')),";
  ss << "STR varchar);";
  exec ( ss.str().c_str() );

  logstr.back() = ' ';
  ss.str ( "" );
  ss << "insert into LOG(STR) values('" << logstr << "');";
  exec ( ss.str().c_str() );

  return true;
}

bool Sqlite3InterfaceLog::QueryLog ( std::string sql, std::vector<std::string>& vecstring )
{
  vec_mapstrstr_t vec_mapstrstr;
  vecstring.resize ( 0 );

  //std::cout << sql << std::endl;

  if ( GetTable ( sql, vec_mapstrstr ) )
  {
    for_each ( vec_mapstrstr.begin(), vec_mapstrstr.end(),
               [&] ( vec_mapstrstr_t::value_type m )
    {
      for ( auto& mv : m )
        vecstring.insert ( vecstring.end(), mv.second );
    } );

    return true;
  }

  return false;
}

#ifdef TESTS
int main ( int argc, char* argv[] )
{
  Sqlite3InterfaceLog* a = Sqlite3InterfaceLog::GetInstance( );

  std::vector<std::string> vecstring;

  a->AddLog ( "error", "log test" );
  a->AddLog ( "info", "log test" );
  a->QueryLog ( "select DATE || ' ' || STR from log;", vecstring );
  //a->QueryLog ( "select DATE || ' ' || STR from log where DATE>='2018-02-09 11:00:00' and DATE<'2018-02-09 13:00:00';", vecstring );
  //a->QueryLog ( "select DATE || ' ' || STR from log where LEVEL='info';", vecstring );
  //a->QueryLog ( "select DATE || ' ' || STR from log where LEVEL like '%err%';", vecstring );

  for ( auto& s : vecstring )
  {
    std::cout << s << std::endl;
  }

  return 0;
}
#endif

