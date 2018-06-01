/*
 * =====================================================================================
 *
 *       Filename:  DebugModule.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2017年09月22日 08时54分05秒
 *  Last Modified:  2017年09月22日 08时54分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef  DEBUGMODULE_INC
#define  DEBUGMODULE_INC

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <unistd.h>

#define  MAX_BUFF_SIZE      (1024)
#define  MY_IPCMSGTYPE_ID   (8888)

#define DEBUG_FUN_PTS_POS       (0)
#define DEBUG_FUN_NAME_POS      (2)
#define DEBUG_FUN_ARG_STARG_POS (3)

typedef std::vector<std::string> vecstr_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  long msgtype; /* must > 0 */
  char buff[MAX_BUFF_SIZE];
} debugMsg_t;
#pragma pack(pop)

typedef void ( *pDebugFun ) ( vecstr_t& vecstr );
typedef std::map< std::string, pDebugFun> map_funname_pfun_t;


class DebugModule
{
  private:
    DebugModule();
    ~DebugModule();

  private:
    static void Task ( void );
    static bool RecvMsg ( std::string& strmsg );
    static bool GetPts ( std::string& pts );
    static void GetVecStr ( std::string strmsg, vecstr_t& vecstr );

  public:
    static void StartTask();
    static void SendMsg ( int argc, char* argv[] );

  public:
    static void PrintSock ( vecstr_t& vecstr );
    static void Log ( vecstr_t& vecstr );
    static void ListAllClient ( vecstr_t& vecstr );
    static void Help ( vecstr_t& vecstr );
    static void GetLog ( vecstr_t& vecstr );

  private:
    static std::thread m_thread;
    static map_funname_pfun_t m_map_funname_pfun;
};

#endif   /* ----- #ifndef DebugModule.h ----- */

