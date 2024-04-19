#pragma once
#include <cstdint>
#include <iostream>

using u32 = uint32_t;
using u64 = uint64_t;

class LogRecordPos {
  public:
    LogRecordPos( u32 fid, u64 oset )
        : file_id( fid )
        , offset( oset ){};
    u32 file_id;
    u64 offset;
};