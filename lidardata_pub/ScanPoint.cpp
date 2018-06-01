
/*
 * =========================================================================
 *
 *       FileName:  ScanPoint.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:58:34
 *  Last Modified:  2018-03-06 15:58:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "ScanPoint.h"

ScanPoint::ScanPoint()
{

}

ScanPoint::~ScanPoint()
{

}

bool ScanPoint::IsValid()
{
  return ( flag == 0xff && dis != 0 && dis < 6500 );
}

float ScanPoint::GetAngle()
{
  return ang * 0.1;
}

float ScanPoint::GetX()
{
  return GetDistance() * sin ( ( GetAngle() / 180.0 ) * 3.1415926 );
}

float ScanPoint::GetY()
{
  return GetDistance() * cos ( ( GetAngle() / 180.0 ) * 3.1415926 );
}

float ScanPoint::GetDistance()
{
  //return dis * 0.001;
  return dis;
}

