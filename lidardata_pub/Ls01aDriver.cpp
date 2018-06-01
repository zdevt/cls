
/*
 * =========================================================================
 *
 *       FileName:  Ls01aDriver.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-01 15:57:39
 *  Last Modified:  2018-04-20 14:09:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <thread>

#ifdef __linux__
  #include <sys/prctl.h>
#endif

#include "Ls01aDriver.h"


Ls01aDriver::Ls01aDriver ( std::string devname )
  : m_runFlag ( true ), m_scanFlag ( false ),
    m_serialport ( devname, 230400, 'n', 8, 1 )
{
  m_vecBuffer.reserve ( 8192 );
  m_vecBuffer.clear();

  m_threadReadLidarData = std::thread ( &Ls01aDriver::ReadLidarData, this );
  m_threadSendLidarData = std::thread ( &Ls01aDriver::SendLidarData, this );

  m_sp_EvClient = NULL;
}

Ls01aDriver::~Ls01aDriver()
{
  m_runFlag = false;

  if ( m_threadReadLidarData.joinable() )
    m_threadReadLidarData.join();

  if ( m_threadSendLidarData.joinable() )
    m_threadSendLidarData.join();
}

void Ls01aDriver::InitialEvClient ( std::string ip,  uint16_t port )
{
  if ( !m_sp_EvClient )
    m_sp_EvClient = std::make_shared<EvClient> ( "lidarpub", ip, port );
}

Ls01aDriver* Ls01aDriver::GetInstance ( std::string devname )
{
  static Ls01aDriver instance ( devname );
  return &instance;
}

int Ls01aDriver::SendCmd ( uint8_t* pcmd, int cmdlen )
{
  return m_serialport.Write ( reinterpret_cast<void*> ( pcmd ), cmdlen );
}

int Ls01aDriver::StartScan()
{
  uint8_t cmd[] = { 0xa5, 0x2c, 0xe1, 0xaa, 0xbb, 0xcc, 0xdd};
  int ret = SendCmd ( cmd, ( int ) sizeof ( cmd ) );
  SLEEP_MS ( 10 );
  return ret;
}

int Ls01aDriver::StopScan()
{
  uint8_t cmd[] = { 0xa5, 0x21, 0xe1, 0xaa, 0xbb, 0xcc, 0xdd};
  int ret = SendCmd ( cmd, ( int ) sizeof ( cmd ) );
  SLEEP_MS ( 10 );
  return ret;
}

int Ls01aDriver::SingleScan ( LidarData& single_scan_data )
{
  uint8_t cmd[] = { 0xa5, 0x22, 0xe1, 0xaa, 0xbb, 0xcc, 0xdd};
  int ret = SendCmd ( cmd, ( int ) sizeof ( cmd ) );

  if ( ret > 0 )
    ret = m_serialport.ReadPackage (
            reinterpret_cast<void*> ( &single_scan_data ),
            single_scan_data.NeedRead(), single_scan_data.NeedRead() );

  return ret;
}

int Ls01aDriver::Scan()
{
  uint8_t cmd[] = {0xa5, 0x20, 0xe1, 0xaa, 0xbb, 0xcc, 0xdd};
  int ret = SendCmd ( cmd, ( int ) sizeof ( cmd ) );
  SLEEP_MS ( 10 );
  return ret;
}

void Ls01aDriver::ReadLidarData ( )
{
  #ifdef __linux__
  prctl ( PR_SET_NAME, __FUNCTION__ );
  #endif

  int ret = 0;
  int errcnt = 0;

  const int cBufferSize = 1024;
  uint8_t pBuffer[cBufferSize] = { 0 };

  Scan();

  while ( m_runFlag )
  {
    if ( m_scanFlag )
    {
      ret =  m_serialport.Read ( pBuffer, cBufferSize );

      if ( ret > 0 )
      {
        {
          std::lock_guard<std::mutex> guard ( m_mutex );

          if ( m_vecBuffer.size() < 8192 )
            m_vecBuffer.insert ( m_vecBuffer.end(), pBuffer, pBuffer + ret );
        }
        errcnt = 0;
      }
      else if ( ret == 0 )
      {
        SLEEP_MS ( 1 );
        continue;
      }

      if ( errcnt++ > 3 )
      {
        StopScan();
        SLEEP_MS ( 10 );
        StartScan();
        SLEEP_MS ( 10 );
        Scan();
        SLEEP_MS ( 10 );
        fprintf ( stderr, "read error %d restart!\n", errcnt );
        errcnt = 0;
      }
    }
    else
      SLEEP_MS ( 100 );
  }
}

void Ls01aDriver::SendLidarData ( )
{
  #ifdef __linux__
  prctl ( PR_SET_NAME, __FUNCTION__ );
  #endif

  int len = 0;
  bool getDataFlag = false;
  LidarData temp;
  std::vector<uint8_t> vecTemp;
  vecTemp.resize ( sizeof ( temp ), 0 );

  while ( m_runFlag )
  {
    if ( m_scanFlag )
    {
      m_mutex.lock();
      len = m_vecBuffer.size();

      if ( len >= 6 )
      {
        for ( int i = 0; i < len - 6; ++i )
        {
          ScanStartAck* p = reinterpret_cast<ScanStartAck*> ( &m_vecBuffer[i] );

          if ( p->IsValid() )
          {
            if ( i > 0 )
            {
              m_vecBuffer.erase ( m_vecBuffer.begin(), m_vecBuffer.begin() + i );
              fprintf ( stderr, "discard i=%d\n", i );
            }

            len = m_vecBuffer.size();

            if ( len >= temp.NeedRead() )
            {
              memcpy ( reinterpret_cast<void*> ( &temp ), &m_vecBuffer[0], temp.NeedRead() );

              if ( temp.IsValid() )
              {
                m_vecBuffer.erase ( m_vecBuffer.begin(), m_vecBuffer.begin() + temp.NeedRead() );
                getDataFlag = true;
                break;
              }
            }
          }
        }
      }

      m_mutex.unlock();

      if ( getDataFlag )
      {
        uint8_t* p = reinterpret_cast<uint8_t*> ( &temp );
        std::copy ( p, p + sizeof ( temp ), vecTemp.begin() );

        if ( m_sp_EvClient )
          m_sp_EvClient->PubMsg ( "lidardata", vecTemp );

        getDataFlag = false;
      }
      else
        SLEEP_MS ( 100 );
    }
    else
      SLEEP_MS ( 100 );
  }
}

int Ls01aDriver::Start()
{
  StartScan();
  m_scanFlag = true;
  sleep ( 1 );
  return 0;
}

int Ls01aDriver::Stop()
{
  m_scanFlag = false;
  StopScan();
  return 0;
}

#ifdef TEST

bool runflag = true;
void SigInt ( int signal )
{
  runflag = false;
}

int main ( int argc, char* argv[] )
{
  ( void ) argc;
  ( void ) argv;
  signal ( SIGINT,  SigInt );

  Ls01aDriver* p = Ls01aDriver::GetInstance ( "/dev/null" );

  if ( argc == 2 )
    p->InitialEvClient ( argv[1] );
  else if ( argc == 3 )
    p->InitialEvClient ( argv[1], atoi ( argv[2] ) );
  else
    p->InitialEvClient ( );

  while ( runflag )
    SLEEP_S ( 3 );

  return 0;
}

#endif


