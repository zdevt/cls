
/*
 * =========================================================================
 *
 *       FileName:  LidarData.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:57:11
 *  Last Modified:  2018-03-06 15:57:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "LidarData.h"


LidarData::LidarData()
{
  Clear();
}

LidarData::~LidarData()
{

}

void LidarData::SetTime()
{
  timeval tv;
  gettimeofday ( &tv, NULL );
  m_timeS = ( uint32_t ) tv.tv_sec;
  m_us = ( uint32_t ) tv.tv_usec;
}

void LidarData::SetCurXY ( int x, int y )
{
  m_cur_x = x;
  m_cur_y = y;
}

void LidarData::SetCurXY ( std::function<void ( int&, int& ) > callback )
{
  callback ( m_cur_x, m_cur_y );
}

void LidarData::SetAngle ( int32_t angle )
{
  m_angle = angle;
}

bool LidarData::IsValid()
{
  return ( m_start.IsValid() && m_end.IsValid() );
}

void LidarData::Clear()
{
  m_start.Clear();
  m_end.Clear();
}

int LidarData::ValidPointsCount()
{
  int i = 0;

  for ( unsigned long j = 0; j < sizeof ( m_point ) / sizeof ( m_point[0] ); ++j )
  {
    if ( m_point[j].IsValid() )
      i++;
  }

  return i;
}

int LidarData::GetMaxRange()
{
  return 8000;
}

int LidarData::NeedRead()
{
  return ( sizeof ( m_start ) + sizeof ( m_point ) + sizeof ( m_end ) );
}

