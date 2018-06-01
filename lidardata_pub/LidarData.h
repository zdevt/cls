
/*
 * =========================================================================
 *
 *       FileName:  LidarData.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:57:24
 *  Last Modified:  2018-03-06 15:57:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef LIDARDATA_H
#define LIDARDATA_H

#include <vector>
#include <memory>
#include <list>
#include <functional>

#include <math.h>
#include <sys/time.h>

#include "ScanStartAck.h"
#include "ScanPoint.h"
#include "ScanEndAck.h"

#pragma pack(push)
#pragma pack(1)

class LidarData
{
  public:
    LidarData();
    ~LidarData();

  public:
    bool IsValid();
    void Clear();
    void SetTime();
    void SetCurXY ( int x, int y );
    void SetCurXY ( std::function<void ( int&, int& ) > callback );
    void SetAngle ( int32_t angle );
    int ValidPointsCount();
    int NeedRead();
    static int GetMaxRange();

  public:
    ScanStartAck m_start;
    ScanPoint m_point[360];
    ScanEndAck m_end;
    uint32_t m_timeS;
    uint32_t m_us;
    int32_t m_cur_x;
    int32_t m_cur_y;
    int32_t m_angle;
};

#pragma pack(pop)

#endif // LIDARDATA_H
