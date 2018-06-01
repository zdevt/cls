
/*
 * =========================================================================
 *
 *       FileName:  EvTimer.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-25 10:42:01
 *  Last Modified:  2018-03-05 18:37:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef EVTIMER_INC
#define EVTIMER_INC

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


#define TIMERNEW(cbk,arg,timems,flag)  EvTimer::GetInstance()->TimerNew(cbk,arg,timems,flag)
#define TIMERDEL(t) EvTimer::GetInstance()->TimerDelete(t)


class EvTimer
{
  private:
    EvTimer();
    virtual ~EvTimer();

  public:
    static EvTimer* GetInstance();

  public:
    void* TimerNew ( event_callback_fn cbk, void* arg, int intervalms, bool flag ); /* flag false:onetime; true:persist */
    void TimerDelete ( void* tmev );

  private:
    event_base* m_base;
    std::mutex m_mutex;
    std::thread m_thread_event;
    std::map<event*, bool> m_map_evtimer_flag;
};

#endif /*  EVTIMER_INC  */

