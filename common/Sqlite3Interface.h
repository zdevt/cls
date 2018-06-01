/*
 * =====================================================================================
 *
 *       Filename:  Sqlite3Interface.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/14/2017 08:30:27 AM
 *  Last Modified:  2018-03-06 15:23:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */
#ifndef  SQLITE3INTERFACE_INC
#define  SQLITE3INTERFACE_INC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <mutex>

#include <sqlite3.h>

typedef std::map<std::string, std::string> map_strstr_t;
typedef std::vector< map_strstr_t > vec_mapstrstr_t;

typedef std::vector< std::vector<uint8_t> > vecvecuint8_t;

class Sqlite3Interface
{
  public:
    explicit Sqlite3Interface ( std::string dbfilename );
    virtual ~Sqlite3Interface();

  private:
    bool Open();
    void Close();

  public:
    bool exec ( std::string sql );
    bool BlobWrite ( std::string sql, std::vector<uint8_t>& vecuint8 );
    bool BlobRead ( std::string sql, std::vector<uint8_t>& vecuint8 );/*  只读一次  */
    bool BlobRead ( std::string sql, vecvecuint8_t& vecvecuint8 ); /*  读取所有结果 */
    bool GetTable ( std::string sql, vec_mapstrstr_t& vec_mapstrstr );

  private:
    std::string m_dbfilename;
    sqlite3* m_pdb;
    sqlite3_stmt* m_stmt;
    std::mutex m_mutex;
};


#endif   /* ----- #ifndef Sqlite3Interface.h ----- */

