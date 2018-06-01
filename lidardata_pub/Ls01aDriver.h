
/*
 * =========================================================================
 *
 *       FileName:  Ls01aDriver.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:57:54
 *  Last Modified:  2018-04-20 14:00:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef LS01ADRIVER_INC
#define LS01ADRIVER_INC

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <vector>

#include "SerialPort.h"
#include "LidarData.h"

#include "../common/EvClient.h"
#include "../common/Common.h"

class Ls01aDriver
{
  private:
    explicit Ls01aDriver ( std::string devname );
    virtual ~Ls01aDriver();

    void ReadLidarData ( );
    void SendLidarData ( );

  private:
    int SendCmd ( uint8_t* pcmd, int cmdlen );

    int StartScan();
    int SingleScan ( LidarData& single_scan_data );
    int Scan();
    int StopScan();

  public:
    static Ls01aDriver* GetInstance ( std::string devname );

    int Start();
    int Stop();

    void InitialEvClient ( std::string ip = "127.0.0.1", uint16_t port = 18888 );

  private:
    bool m_runFlag;
    bool m_scanFlag;

    std::thread m_threadReadLidarData;
    std::thread m_threadSendLidarData;

    std::mutex m_mutex;
    std::vector<uint8_t> m_vecBuffer;

    std::shared_ptr<EvClient> m_sp_EvClient;

    SerialPort m_serialport;
    LidarData m_single_scan_data;
};

#endif /*  LS01ADRIVER_INC  */

