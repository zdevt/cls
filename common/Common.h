
/*
 * =========================================================================
 *
 *       FileName:  Common.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2018-03-07 09:58:30
 *  Last Modified:  2018-03-07 10:31:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */


#ifndef COMMON_INC
#define COMMON_INC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

//#include <netinet/in.h>
#include <arpa/inet.h>
//#include <sys/socket.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <list>
#include <string>
#include <functional>


#define  SLEEP_S(x)  ( std::this_thread::sleep_for(std::chrono::seconds(x)))
#define  SLEEP_MS(x) ( std::this_thread::sleep_for(std::chrono::milliseconds(x)))
#define  SLEEP_US(x) ( std::this_thread::sleep_for(std::chrono::microseconds(x)))



#endif /*  COMMON_INC  */

