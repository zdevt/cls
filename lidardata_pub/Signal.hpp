
/*
 * =========================================================================
 *
 *       FileName:  Signal.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2017-12-22 13:50:14
 *  Last Modified:  2018-03-07 09:05:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zt ()
 *   Organization:
 *
 * =========================================================================
 */

#ifndef SIGNAL__INC
#define SIGNAL__INC

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <string>

#include <functional>
#include <map>

template <typename... Args>
class Signal
{
  public:
    Signal() : current_id_ ( 0 )
    {
    }

    Signal ( Signal const& other ) : current_id_ ( 0 )
    {
      ( void ) other;
    }

    template <typename F, typename... A>
    int connect_member ( F&& f, A&& ... a ) const
    {
      slots_.insert ( {++current_id_, std::bind ( f, a... ) } );
      return current_id_;
    }

    int connect ( std::function<void ( Args... ) > const& slot ) const
    {
      slots_.insert ( std::make_pair ( ++current_id_, slot ) );
      return current_id_;
    }

    void disconnect ( int id ) const
    {
      slots_.erase ( id );
    }

    void disconnect_all() const
    {
      slots_.clear();
    }

    void emit ( Args... p )
    {
      for ( auto it : slots_ )
      {
        it.second ( p... );
      }
    }

    Signal& operator= ( Signal const& other )
    {
      ( void ) other;
      disconnect_all();
    }

  private:
    mutable std::map<int, std::function<void ( Args... ) > > slots_;
    mutable int current_id_;
};

#endif /*  SIGNAL__INC  */

