
/*
 * =========================================================================
 *
 *       FileName:  EvTimer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-25 10:41:54
 *  Last Modified:  2018-04-26 14:19:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "EvTimer.h"

EvTimer::EvTimer()
{
  evthread_use_pthreads();
  m_base = event_base_new();
  m_thread_event = std::thread ( event_base_dispatch, m_base );
}

EvTimer::~EvTimer()
{
  timeval delay = { 0, 300 * 1000 };
  event_base_loopexit ( m_base, &delay );

  if ( m_thread_event.joinable() )
    m_thread_event.join();

  for ( auto& e : m_map_evtimer_flag )
    evtimer_del ( e.first );

  event_base_free ( m_base );
}

EvTimer* EvTimer::GetInstance()
{
  static EvTimer instance;
  return &instance;
}

void* EvTimer::TimerNew ( event_callback_fn cbk, void* arg, int intervalms, bool flag )
{
  event* evtimer = NULL;
  std::lock_guard<std::mutex> lck ( m_mutex );

  uint32_t flag_timer  = ( flag ) ? ( EV_TIMEOUT | EV_PERSIST ) : ( EV_TIMEOUT );
  evtimer = event_new ( m_base, -1, flag_timer, cbk, arg );

  if ( evtimer )
  {
    timeval tv = { intervalms / 1000, ( intervalms * 1000 ) % ( 1000 * 1000 ) };
    event_add ( evtimer, &tv );
    m_map_evtimer_flag.insert ( {evtimer, flag} );
    fprintf ( stderr, "%s %d \n", __FILE__, __LINE__ );
    return reinterpret_cast<void*> ( evtimer );
  }

  fprintf ( stderr, "TimerNew %p!\n", evtimer );
  return NULL;
}

void EvTimer::TimerDelete ( void* tmev )
{
  std::lock_guard<std::mutex> lck ( m_mutex );
  auto iter = m_map_evtimer_flag.find ( reinterpret_cast<event*> ( tmev ) );

  if ( iter != m_map_evtimer_flag.end() )
  {
    evtimer_del ( iter->first );
    m_map_evtimer_flag.erase ( iter );
  }
}

#ifdef TEST

void cbk ( int fd, short event, void* p )
{
  ( void ) fd;
  ( void ) event;
  fprintf ( stderr, "timer one time %p\n", p );
}

void cbk2 ( int fd, short event, void* p )
{
  ( void ) fd;
  ( void ) event;
  fprintf ( stderr, "timer persisit %p\n", p );
}

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

  int a = 1;
  int b = 2;

  //onetime
  void* tim1 = EvTimer::GetInstance()->TimerNew ( cbk, &a, 2000, false );
  //persist timer
  void* timer = EvTimer::GetInstance()->TimerNew ( cbk2, &b, 1000, true );
  //delete timer
  std::this_thread::sleep_for ( std::chrono::seconds ( 5 ) );
  EvTimer::GetInstance()->TimerDelete ( timer );

  fprintf ( stderr, "%p %p\n", tim1, timer );

  while ( runflag )
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );

  return 0;
}
#endif


