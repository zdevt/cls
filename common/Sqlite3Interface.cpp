/*
 * =====================================================================================
 *
 *       Filename:  Sqlite3Interface.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/14/2017 08:30:19 AM
 *  Last Modified:  2018-03-06 15:25:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt (),
 *   Organization:
 *
 * =====================================================================================
 */

#include "Sqlite3Interface.h"

Sqlite3Interface::Sqlite3Interface ( std::string dbfilename )
  : m_dbfilename ( dbfilename ), m_pdb ( NULL )
{
  Open();
}

Sqlite3Interface::~Sqlite3Interface()
{
  Close();
}

bool Sqlite3Interface::Open()
{
  if ( SQLITE_OK != sqlite3_open ( m_dbfilename.c_str(), &m_pdb ) )
    return false;

  return true;
}

void Sqlite3Interface::Close()
{
  if ( m_pdb )
    sqlite3_close ( m_pdb );
}

bool Sqlite3Interface::exec ( std::string sql )
{
  std::lock_guard<std::mutex> lock ( m_mutex );

  char* zErrMsg = NULL;

  if ( SQLITE_OK != sqlite3_exec ( m_pdb, sql.c_str(), NULL, NULL, &zErrMsg ) )
  {
    sqlite3_free ( zErrMsg );
    return false;
  }

  return true;
}

bool Sqlite3Interface::BlobWrite ( std::string sql, std::vector<uint8_t>& vecuint8 )
{
  std::lock_guard<std::mutex> lock ( m_mutex );

  sqlite3_prepare ( m_pdb, sql.c_str(), -1, &m_stmt, NULL );
  sqlite3_bind_blob ( m_stmt, 1, &vecuint8[0], vecuint8.size(), NULL );
  sqlite3_step ( m_stmt );
  sqlite3_finalize ( m_stmt );
  return true;
}

bool Sqlite3Interface::BlobRead ( std::string sql, std::vector<uint8_t>& vecuint8 )
{
  std::lock_guard<std::mutex> lock ( m_mutex );

  sqlite3_prepare ( m_pdb, sql.c_str(), -1, &m_stmt, NULL );
  sqlite3_step ( m_stmt );

  char* data = ( char* ) sqlite3_column_blob ( m_stmt, 0 );
  int iLen = sqlite3_column_bytes ( m_stmt, 0 );
  vecuint8.resize ( iLen, 0 );
  std::copy ( data, data + iLen, vecuint8.begin() );

  sqlite3_finalize ( m_stmt );

  return true;
}

bool Sqlite3Interface::BlobRead ( std::string sql, vecvecuint8_t& vecvecuint8 )
{
  std::lock_guard<std::mutex> lock ( m_mutex );

  int ret = 0;
  sqlite3_prepare ( m_pdb, sql.c_str(), -1, &m_stmt, NULL );
  vecvecuint8.resize ( 0 );

  std::vector<uint8_t> vecBuffer ( 0 );

  do
  {
    ret = sqlite3_step ( m_stmt );

    char* data = ( char* ) sqlite3_column_blob ( m_stmt, 0 );
    int iLen = sqlite3_column_bytes ( m_stmt, 0 );
    vecBuffer.resize ( iLen, 0 );

    std::copy ( data, data + iLen, vecBuffer.begin() );
    vecvecuint8.insert ( vecvecuint8.end(), vecBuffer );
  }
  while ( SQLITE_ROW == ret );

  sqlite3_finalize ( m_stmt );

  return true;
}

bool Sqlite3Interface::GetTable ( std::string sql, vec_mapstrstr_t& vec_mapstrstr )
{
  std::lock_guard<std::mutex> lock ( m_mutex );

  map_strstr_t map_strstr;
  vec_mapstrstr.resize ( 0 );

  char** dbRes = NULL;
  char* zErrMsg = NULL;
  int r, c;

  if ( SQLITE_OK == sqlite3_get_table ( m_pdb, sql.c_str(), &dbRes, &r, &c, &zErrMsg ) )
  {
    for ( int i = 0; i < r; i++ )
    {
      for ( int j = 0; j < c; j++ )
        map_strstr.insert ( map_strstr_t::value_type ( dbRes[j], dbRes[c + j + c * i] ) );

      if ( map_strstr.size() > 0 )
        vec_mapstrstr.insert ( vec_mapstrstr.end(), map_strstr );

      map_strstr.clear();
    }

    if ( dbRes )
      sqlite3_free_table ( dbRes );

    return true;
  }

  if ( NULL != zErrMsg )
    sqlite3_free ( zErrMsg );

  return false;
}

