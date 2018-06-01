
/*
 * =========================================================================
 *
 *       FileName:  ScanStartAck.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:59:00
 *  Last Modified:  2018-03-06 15:59:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "ScanStartAck.h"

ScanStartAck::ScanStartAck()
{
  Clear();
}

ScanStartAck::~ScanStartAck()
{
}

bool ScanStartAck::IsValid()
{
  return  ( startFlag == 0xa5 &&
            len == 5 && type == 0x81 );
}

float ScanStartAck::GetSpeed()
{
  return ( speed / 15.0 );
}

void ScanStartAck::Clear()
{
  startFlag = 0x0;
  speed = 0x0;
  len = 0x0;
  type = 0x0;
}

