
/*
 * =========================================================================
 *
 *       FileName:  ProtocolInc.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-27 08:20:58
 *  Last Modified:  2018-01-27 09:27:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include "ProtocolInc.h"


bool GetOneProtocol ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal )
{
  protocol_header_t* p = NULL;
  uint32_t len = vecBuffer.size();

  if ( len >= PROTOCOL_HEADER_SIZE )
  {
    for ( uint32_t i = 0;  i <= len - PROTOCOL_HEADER_SIZE; ++i )
    {
      p = reinterpret_cast<protocol_header_t*> ( &vecBuffer[i] );

      if ( p->IsValid() )
      {
        if ( i > 0 )
          vecBuffer.erase ( vecBuffer.begin(), vecBuffer.begin() + i );

        if ( vecBuffer.size() >= p->NeedRead() )
        {
          vecProtocal = vecBuffer;
          vecProtocal.resize ( p->NeedRead() );
          vecBuffer.erase ( vecBuffer.begin(), vecBuffer.begin() + p->NeedRead() );
          return true;
        }
        else
          return false;
      }
    }
  }

  if ( vecBuffer.size() >= 2 * PROTOCOL_HEADER_SIZE )
    vecBuffer.erase ( vecBuffer.begin(), vecBuffer.begin() + PROTOCOL_HEADER_SIZE );

  return false;
}

std::vector<uint8_t> MakeProtocol ( uint16_t type, uint32_t dataLen, uint8_t* data,
                                    uint16_t id, uint16_t subType, uint32_t status, uint16_t magic_word )
{
  std::vector<uint8_t> vec ( sizeof ( protocol_header_t ), 0 );
  protocol_header_t* p = reinterpret_cast<protocol_header_t*> ( &vec[0] );
  p->magic_word = magic_word;
  p->id = id;
  p->type = type;
  p->subType = subType;
  p->status = status;
  p->dataLen = dataLen;
  vec.insert ( vec.end(), data, data + dataLen );
  return vec;
}
