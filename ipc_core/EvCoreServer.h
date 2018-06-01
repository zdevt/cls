
/*
 * =========================================================================
 *
 *       FileName:  EvCoreServer.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-24 10:31:18
 *  Last Modified:  2018-01-27 14:06:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef EVCORESERVER_INC
#define EVCORESERVER_INC

#include "EvTcpHandler.h"
#include "../common/ProtocolInc.h"

class HeartBeatInfo
{
  public:
    HeartBeatInfo()
    {
    }

    HeartBeatInfo ( std::string n )
    {
      time ( &tm );
      name = n;
    }

    HeartBeatInfo ( time_t t, std::string n )
    {
      tm = t;
      name = n;
    }
  public:
    std::string name;
    time_t tm;
};


class EvCoreServer : public EvTcpHandler
{
  public:
    EvCoreServer();
    virtual ~EvCoreServer();

  private:
    void ProcHeartBeat ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );
    void ProcPub ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );
    void ProcPubQuery ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );
    void ProcSub ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );
    void ProcSubCancel ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );

    void CheckClientStatus();

  private:
    void CoreTask();

  private:
    virtual bool GetOneProtocolData ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal );
    virtual void HandleOneProtocolData ( bufferevent* bev, std::vector<uint8_t>& vecProtocal );

  private:
    bool m_runFlag;
    std::thread m_thread_core;
    std::mutex m_mutex;
    std::map<bufferevent*, HeartBeatInfo> m_map_bfevnt_heartbeatinfo;
    std::map<std::string, std::list<bufferevent*> > m_map_pubname_listsubbev;
};

#endif /*  EVCORESERVER_INC  */

