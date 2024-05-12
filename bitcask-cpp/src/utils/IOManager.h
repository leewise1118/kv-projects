#pragma once

#include "type.h"
#include <vector>

using namespace std;

namespace bitcask {

class IOManager {
  public:
    IOManager()                               = default;
    IOManager( const IOManager & )            = default;
    IOManager( IOManager && )                 = default;
    IOManager &operator=( const IOManager & ) = default;
    IOManager &operator=( IOManager && )      = default;
    virtual ~IOManager()                      = default;

    virtual u64  read( vector< u8 > &buf, u64 offset ) = 0;
    virtual u64  write( vector< u8 > &buf )            = 0;
    virtual void sync()                                = 0;
};

} // namespace bitcask