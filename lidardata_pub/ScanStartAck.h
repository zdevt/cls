
/*
 * =========================================================================
 *
 *       FileName:  ScanStartAck.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:59:12
 *  Last Modified:  2018-03-06 15:59:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#ifndef SCANSTARTACK_H
#define SCANSTARTACK_H

#include <math.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

class ScanStartAck
{
  public:
    ScanStartAck();
    ~ScanStartAck();

  public:
    bool IsValid();
    float GetSpeed();
    void Clear();

  public:
    uint8_t startFlag; /* 0xa5 */
    uint8_t speed; /* /15 = rpm/s */
    uint8_t len; /* 5 */
    uint8_t resered[3];
    uint8_t type; /* 0x81 */
};

#pragma pack(pop)

#endif // SCANSTARTACK_H
