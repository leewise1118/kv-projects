#pragma once
#include "../fio/file_io.h"
#include "../utils/IOManager.h"
#include "../utils/type.h"
#include "./log_record.h"
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

namespace bitcask {

// IOManager 抽象 IO 管理对象，可以介入不同的 IO 类型。需要保证多线程传递安全。
class DataFile {
  public:
    // TODO:
    DataFile( shared_ptr< u32 > file_id, shared_ptr< u64 > write_off,
              unique_ptr< IOManager > io_manager )
        : file_id( file_id )
        , write_off( write_off )
        , io_manager( std::move( io_manager ) ) {
    }
    u64 get_write_off() const {
        return *write_off;
    }

    u32 get_file_id() const {
        return *file_id;
    }

    LogRecord read_log_record( u64 offset );

  private:
    // 数据文件的 ID，用于标识数据文件
    shared_ptr< u32 > file_id;

    // 当前写偏移，记录该数据文件写到了哪个位置
    shared_ptr< u64 > write_off;

    // IO 管理对象，通过多态的形式管理不同的 IO 类型。
    unique_ptr< IOManager > io_manager;
};

} // namespace bitcask