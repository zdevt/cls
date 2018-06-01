
/*
 * =========================================================================
 *
 *       FileName:  ScanEndAck.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:58:12
 *  Last Modified:  2018-03-06 15:58:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#include "ScanEndAck.h"


ScanEndAck::ScanEndAck()
{
  Clear();
}

ScanEndAck::~ScanEndAck()
{

}

bool ScanEndAck::IsValid()
{
  return ( endAck[0] == 0xff &&
           endAck[1] == 0xaa &&
           endAck[2] == 0xbb &&
           endAck[3] == 0xcc &&
           endAck[4] == 0xdd );
}

void ScanEndAck::Clear()
{
  memset ( reinterpret_cast<void*> ( endAck ), 0, 5 );
}
