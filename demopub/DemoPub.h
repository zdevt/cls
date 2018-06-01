
/*
 * =========================================================================
 *
 *       FileName:  DemoPub.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-26 09:12:27
 *  Last Modified:  2018-02-08 14:12:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef DEMOPUB_INC
#define DEMOPUB_INC

#include "../common/EvClient.h"
#include "../timer/EvTimer.h"


class DemoPub : public EvClient
{
  public:
    DemoPub ( std::string server = "127.0.0.1", uint16_t port = 18888 );
    virtual ~DemoPub();

  private:
    bool m_runFlag;
    std::mutex m_mutex;
};

#endif /*  DEMOPUB_INC  */

