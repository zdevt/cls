
/*
 * =========================================================================
 *
 *       FileName:  ProtocolInc.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-01-27 08:19:46
 *  Last Modified:  2018-02-08 15:13:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef PROTOCOLINC_INC
#define PROTOCOLINC_INC

#include <stdint.h>
#include <time.h>

#include <string>
#include <vector>

#define PROTOCOL_HEADER_MAGIC   (0xeb90)

#pragma pack(push)
#pragma pack(1)

class protocol_header_t
{
  public:
    bool IsValid()
    {
      return ( magic_word == PROTOCOL_HEADER_MAGIC && dataLen <= 32 * 1024 );
    }

    uint32_t NeedRead()
    {
      return ( sizeof ( protocol_header_t ) + dataLen );
    }

  public:
    uint16_t magic_word; /* PROTOCOL_HEADER_MAGIC */
    uint16_t id;
    uint16_t type;       /*  EnumCmd */
    uint16_t subType;    /*  EnumSubCmd */
    uint32_t status;
    uint32_t dataLen;    /* <= 32*1024 */
    uint8_t  data[0];
};

#define PROTOCOL_HEADER_SIZE  (sizeof (protocol_header_t ))

enum EnumCmd
{
  PROTOCOL_HEARTBEAT  = 0x01,
  PROTOCOL_PUB        = 0x02,
  PROTOCOL_PUB_QUERY  = 0x03,
  PROTOCOL_SUB        = 0x04,
  PROTOCOL_SUB_CANCEL = 0x05,

  PROTOCOL_MAX,
};

#pragma pack(pop)


bool GetOneProtocol ( std::vector<uint8_t>& vecBuffer, std::vector<uint8_t>& vecProtocal );
std::vector<uint8_t> MakeProtocol ( uint16_t type, uint32_t dataLen, uint8_t* data,
                                    uint16_t id = 0, uint16_t subType = 0x0, uint32_t status = 0x0, uint16_t magic_word = 0xeb90 );

#endif /*  PROTOCOLINC_INC  */

