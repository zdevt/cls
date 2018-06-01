/*
 * =====================================================================================
 *
 *       Filename:  Sqlite3InterfaceLog.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/14/2017 01:54:50 PM
 *  Last Modified:  2018-03-08 10:14:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef  SQLITE3INTERFACELOG_INC
#define  SQLITE3INTERFACELOG_INC

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <mutex>

#include "Sqlite3Interface.h"

#define SQLITE3_SEISLOG_NAME       "./log.db"

#define GET_LOGPDB    Sqlite3InterfaceLog::GetInstance()

class Sqlite3InterfaceLog : public Sqlite3Interface
{
  private:
    explicit Sqlite3InterfaceLog ( std::string dbFilename );
    virtual ~Sqlite3InterfaceLog();

  public:
    static Sqlite3InterfaceLog* GetInstance( );

    /* ID DATE LOG */
    bool AddLog ( std::string logstr );
    bool QueryLog ( std::string sql, std::vector<std::string>& vecstring );
};

#endif   /* ----- #ifndef Sqlite3InterfaceLog.h ----- */

