
/*
 * =========================================================================
 *
 *       FileName:  DemoSub.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-27 17:03:57
 *  Last Modified:  2018-02-08 14:28:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef DEMOSUB_INC
#define DEMOSUB_INC

#include "../common/EvClient.h"


class DemoSub : public EvClient
{
  public:
    DemoSub ( std::string server = "127.0.0.1", uint16_t port = 18888 );
    virtual ~DemoSub();

  private:

};

#endif /*  DEMOSUB_INC  */

