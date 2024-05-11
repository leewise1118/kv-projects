#pragma once
#include "../utils/type.h"
#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;

namespace bitcask {

enum LogRecordType {
    // 正常 put 的数据
    NORMAL = 1,
    // 被删除的数据标识，墓碑值
    DELETED = 2,
};

// 数据位置索引信息，描述数据存储到了那个位置
class LogRecordPos {
  public:
    LogRecordPos( u32 fid, u64 oset )
        : file_id( fid )
        , offset( oset ){};
    u32 file_id; // 文件 id
    u64 offset;  // 文件偏移量

    // 重载==符号
    bool operator==( const LogRecordPos &p ) const {
        if ( file_id == p.file_id && offset == p.offset ) {
            return true;
        }
        return false;
    }
};

class LogRecord {
  public:
    vector< u8 >  key;
    vector< u8 >  value;
    LogRecordType rec_type;

    vector< u8 > encode();
};

} // namespace bitcask