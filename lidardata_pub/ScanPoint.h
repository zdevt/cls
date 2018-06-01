
/*
 * =========================================================================
 *
 *       FileName:  ScanPoint.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:58:47
 *  Last Modified:  2018-03-06 15:58:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef SCANPOINT_H
#define SCANPOINT_H

#include <math.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

class ScanPoint
{
  public:
    ScanPoint();
    ~ScanPoint();

  public:
    bool IsValid();
    float GetAngle();
    float GetX();
    float GetY();
    float GetDistance();

  public:
    uint8_t flag;
    uint16_t ang; /* 0.1 */
    uint16_t dis; /* mm */
};

#pragma pack(pop)


#endif // SCANPOINT_H
