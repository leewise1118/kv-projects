#include "file_io.h"
#include <cstddef>
#include <cstdio>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace bitcask {

u64 FileIO::read( vector< u8 > &buf, u64 offset ) {
    shared_lock< shared_mutex > ReadLock( this->mutex );

    // 获取文件大小
    file->seekg( file->end );
    u64 file_size = file->tellg();

    cout << file_size << endl;
    // 判断读取数据的大小
    if ( offset >= file_size ) {
        return 0;
    }
    u64 read_size = 0;
    if ( file_size - offset < buf.size() ) {
        read_size = file_size - offset;
    } else {
        read_size = buf.size();
    }

    this->file->seekg( file->beg + offset );
    if ( !this->file->read( reinterpret_cast< char * >( buf.data() ),
                            read_size ) ) {
        throw runtime_error( "Failed to read file" );
    }
    return read_size;
}

u64 FileIO::write( vector< u8 > &buf ) {
    unique_lock< shared_mutex > WriteLock( this->mutex );

    if ( !this->file->write( reinterpret_cast< char * >( buf.data() ),
                             buf.size() ) ) {
        throw runtime_error( "Failed to write file" );
    }
    return buf.size();
}

void FileIO::sync() {
}

} // namespace bitcask
