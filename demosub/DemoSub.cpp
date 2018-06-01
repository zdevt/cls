
/*
 * =========================================================================
 *
 *       FileName:  DemoSub.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-27 17:03:48
 *  Last Modified:  2018-02-08 14:58:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "DemoSub.h"

DemoSub::DemoSub ( std::string server, uint16_t port )
  : EvClient ( "DemoPub", server, port )
{
}

DemoSub::~DemoSub()
{
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
  DemoSub* b = NULL;

  if ( argc == 2 )
    b = new DemoSub ( argv[1] );
  else if ( argc == 3 )
    b = new DemoSub ( argv[1], atoi ( argv[2] ) );
  else
    b = new DemoSub();

  while ( ! b->SubMsg ( "1111" ) )
    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );

  std::string msgname;
  std::vector<uint8_t> msg;

  while ( runflag )
  {
    if ( b->GetMsg ( msgname, msg ) )
    {
      fprintf ( stderr, "msgname = %s, msg len= %d\n", msgname.c_str(), ( int ) msg.size() );
    }

    std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
  }

  delete b;

  return 0;
}

