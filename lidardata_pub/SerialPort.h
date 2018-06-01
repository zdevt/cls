/*
 * =====================================================================================
 *
 *       Filename:  SerialPort.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-06 15:56:53
 *  Last Modified:  2018-03-07 08:44:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef  SERIALPORT_INC
#define  SERIALPORT_INC

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include <string.h>
#include <string>
#include <map>
#include <vector>

#include <termios.h>

#define RETRY_TIME 0x10

typedef struct termios termios_serial_t;

class SerialPort
{
  public:
    SerialPort ( std::string devname, unsigned int band_rate,
                 char parity, int databits, int stopbits,
                 int vtime = 1, int vmin = 60 );

    virtual ~SerialPort();

    int Write ( const void* pWbuffer, int wSize );
    int Write ( std::vector<unsigned char>& vecWrite );

    int Read ( void* pBuffer, int iBufferSize );
    int ReadPackage ( void* pBuffer, int iBufferSize, int expectReadSize );

  private:
    int Init();
    int DeInit();

    int SetSerial ( unsigned int band_rate, char parity, int databits, int stopbits );

  private:
    int m_fd;
    std::string m_devname;
    unsigned int m_band_rate;
    termios_serial_t m_oldtio;
    termios_serial_t m_newtio;
    char m_parity;
    int m_databits;
    int m_stopbits;
    int m_vtime;
    int m_vmin;
};

#endif   /* ----- #ifndef SerialPort.h ----- */

