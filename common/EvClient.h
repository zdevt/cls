
/*
 * =========================================================================
 *
 *       FileName:  EvClient.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-26 09:45:34
 *  Last Modified:  2018-04-26 10:46:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef EVCLIENT_INC
#define EVCLIENT_INC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

//#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/socket.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <string>
#include <functional>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/listener.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#include "ProtocolInc.h"

#define LOG_LEVEL_DEBUG  "debug"
#define LOG_LEVEL_INFO   "info"
#define LOG_LEVEL_ALARM  "alarm"
#define LOG_LEVEL_ERROR  "error"
#define LOG_LEVEL_FATAL  "fatal"

#define PRINT_LOG( level, fmt, arg...)  PubLog( level, __FILE__, __LINE__,__FUNCTION__,fmt,##arg)

class EvClient
{
  public:
    EvClient ( std::string selfname, std::string server = "127.0.0.1", uint16_t port = 18888 );
    virtual ~EvClient();

  public:
    bool SubMsg ( std::string msgname );
    void SubMsgCancel ( std::string msgname );
    void PubMsg ( std::string msgname, std::vector<uint8_t> vecmsg );
    bool GetMsg ( std::string& msgname, std::vector<uint8_t>& vecmsg );

    /* do not use PubLog, use PRINT_LOG instead */
    int PubLog ( std::string level, std::string filename,
                 int fileline, std::string funcname, const char* fmt, ... );
    bool GetLog ( std::string& logstr );

  protected:
    bool GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocol );
    int SendToServer ( std::vector<uint8_t>& vecProtocol );

  private:
    static void ConnWriteCb ( bufferevent* bev, void* user_data );
    static void ConnReadCb ( bufferevent* bev, void* user_data );
    static void ConnEventCb ( bufferevent* bev, short events, void* user_data );
    static void TimerCbkHeartBeat ( int fd, short event, void* user_data );

    bool Connect();

  private:
    event_base* m_base;
    bufferevent* m_bev;
    event* m_evtimer;
    std::string m_ip_server;
    uint16_t m_port_server;
    std::string m_selfname;
    sockaddr_in m_addr_server;
    std::vector<uint8_t> m_vecBuffer;
    std::mutex m_mutex;
    std::thread m_thread_event;
};

#endif /*  EVCLIENT_INC  */

