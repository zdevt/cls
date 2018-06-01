
/*
 * =========================================================================
 *
 *       FileName:  DemoPub.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-26 09:12:23
 *  Last Modified:  2018-04-26 14:26:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "DemoPub.h"


DemoPub::DemoPub ( std::string server, uint16_t port )
  : EvClient ( "DemoPub", server, port )
{
}

DemoPub::~DemoPub()
{
}

// test code
void cbk2 ( int fd, short event, void* p )
{
  ( void ) fd;
  ( void ) event;

  static int i = 0;
  fprintf ( stderr, "." );
  /* user code */
  ++i;
  DemoPub* pDemopub = reinterpret_cast<DemoPub*> ( p );

  if ( i % 0x20  == 0 )
    pDemopub->PRINT_LOG ( "fatal", "0x%02x %p %s\n", i, NULL, "fatal test" );

  if ( i % 0x10  == 0 )
    pDemopub->PRINT_LOG ( "error", "0x%02x %p %s\n", i, NULL, "timer error cbk2" );

  if ( i % 0x8 == 0 )
    pDemopub->PRINT_LOG ( "alarm", "0x%02x %p %s\n", i, NULL, "timer alarm cbk2" );

  if ( i % 0x4 == 0 )
    pDemopub->PRINT_LOG ( "info", "0x%02x %p %s\n", i, NULL, "timer info cbk2" );

  if ( i % 0x2 == 0 )
    pDemopub->PRINT_LOG ( "debug", "0x%02x %p %s\n", i, NULL, "timer debug cbk2" );
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
  signal ( SIGINT,  SigInt );

  /* demo usage*/
  #if 1
  std::shared_ptr<DemoPub> b = NULL;

  if ( argc == 2 )
    b = std::make_shared<DemoPub> ( argv[1] );
  else if ( argc == 3 )
    b = std::make_shared<DemoPub> ( argv[1], atoi ( argv[2] ) );
  else
    b = std::make_shared<DemoPub> ( );

  void* p = TIMERNEW ( cbk2, b.get(), 1000, true );
  #else

  DemoPub a;
  void* p = TIMERNEW ( cbk2, &a, 1000, true );
  #endif

  while ( runflag )
  {
    //a.PRINT_LOG ( "info", "0x%02x %p %s\n", ++i, NULL, ".... test log123456" );
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
  }

  TIMERDEL ( p );

  return 0;
}


