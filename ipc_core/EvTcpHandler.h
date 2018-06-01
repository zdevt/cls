
/*
 * =========================================================================
 *
 *       FileName:  EvTcpHandler.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-23 08:22:01
 *  Last Modified:  2018-01-27 14:26:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef EVTCPHANDLER_INC
#define EVTCPHANDLER_INC

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


class EvTcpHandler
{
  public:
    EvTcpHandler ( uint16_t port );
    virtual ~EvTcpHandler();

  protected:
    void Start();

    int SendDataToClient ( bufferevent* bev, std::vector<uint8_t>& data );
    bool DisconnectClient ( bufferevent* bev );

  private:
    virtual bool GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal ) = 0;
    virtual void HandleOneProtocolData ( bufferevent* bev, std::vector<uint8_t>& vecProtocal ) = 0;

  private:
    void Init();
    void Deinit();
    void TaskEvent();
    void TaskGetProtocolDataAndHandle();
    void GetProtocalData ( std::map<bufferevent*, std::list<std::vector<uint8_t> > >& map_bfevnt_listvecProtocol );

  private:
    static void ListenerCb ( evconnlistener* listenner, evutil_socket_t fd, sockaddr* sa, int socklen, void* user_data );
    static void ConnWriteCb ( bufferevent* bev, void* user_data );
    static void ConnReadCb ( bufferevent* bev, void* user_data );
    static void ConnEventCb ( bufferevent* bev, short events, void* user_data );

  private:
    bool m_runFlag;
    event_base* m_base;
    evconnlistener* m_listenner;
    uint16_t m_port;
    std::map<bufferevent*, std::vector<uint8_t> > m_map_bfevnt_vecuint8;
    std::map<bufferevent*, std::list<std::vector<uint8_t> > > m_map_bfevnt_listvecProtocol;
    std::mutex m_mutex;
    std::thread m_thread_event;
    std::thread m_thread_getpro_and_hand;
};

#endif /*  EVTCPHANDLER_INC  */

