
/*
 * =========================================================================
 *
 *       FileName:  ScanEndAck.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:58:23
 *  Last Modified:  2018-03-06 15:58:31
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef SCANENDACK_H
#define SCANENDACK_H

#include <math.h>
#include <stdint.h>
#include <string.h>

#pragma pack(push)
#pragma pack(1)

class ScanEndAck
{
  public:
    ScanEndAck();
    ~ScanEndAck();

  public:
    bool IsValid();
    void Clear();

  public:
    uint8_t endAck[5];
};

#pragma pack(pop)

#endif // SCANENDACK_H
