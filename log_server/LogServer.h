
/*
 * =========================================================================
 *
 *       FileName:  LogServer.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-02-08 15:35:00
 *  Last Modified:  2018-03-12 14:58:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef LOGSERVER_INC
#define LOGSERVER_INC

#include "../common/EvClient.h"
#include "../common/Sqlite3InterfaceLog.h"


class LogServer: public EvClient
{
  public:
    LogServer ( std::string server = "127.0.0.1", uint16_t port = 18888 );
    virtual ~LogServer();

  public:

  private:
    void LogTask ( void );

  private:
    bool m_runFlag;
    std::thread m_thread;
    std::string m_logmsg;
    Sqlite3InterfaceLog* m_sqlite3log;

};

#endif /*  LOGSERVER_INC  */

