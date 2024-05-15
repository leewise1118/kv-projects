#pragma once

#include "../utils/result.h"
#include "../utils/type.h"
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// 封装了一套类似Rust的File类

namespace bitcask {

/**
 * @brief OpenOptions类，用于设置文件打开模式
 *  TODO: 未完成的功能有: 验证模式是否合法。
 */
class OpenOptions {
  public:
    OpenOptions()  = default;
    ~OpenOptions() = default;

    OpenOptions &read( bool value ) {
        this->ReadMode = value;
        return *this;
    }
    OpenOptions &write( bool value ) {
        this->WriteMode = value;
        return *this;
    }
    OpenOptions &append( bool value ) {
        this->AppendMode = value;
        return *this;
    }
    OpenOptions &truncate( bool value ) {
        this->TruncateMode = value;
        return *this;
    }
    OpenOptions &ate( bool value ) {
        this->AteMode = value;
        return *this;
    }
    OpenOptions &binary( bool value ) {
        this->BinaryMode = value;
        return *this;
    }

    Result< i32, string_view > getMode();
    bool                       verify();

  private:
    bool ReadMode = false; // 以输入模式打开文件，即读取文件
    bool WriteMode =
        false; // 以输出模式打开文件，即写入文件，如果文件已经存在，那么原有的内容将被清空。如果文件不存在，则创建一个新文件.
    bool AppendMode   = false; // 追加模式打开文件
    bool TruncateMode = false; // 如果文件已存在，原有内容被清空
    bool AteMode      = false; // 文件打开后立即移动到文件末尾
    bool BinaryMode   = false; // 以二进制打开文件
};

// TODO: Dir类
class Dir {
  public:
    Dir( string &dir )
        : m_dir( dir ) {
    }

    bool is_dir() {
    }

  private:
    string m_dir;
};

// TODO: Path类
class Path {
  public:
    Path( string &path )
        : m_path( path ) {
    }
    ~Path() = default;

    Path &join( string &path ) {
        if ( !path.empty() ) {
            m_path += "/" + path;
        }
        return *this;
    }
    Path &extend( string &extend_type ) {
        if ( !extend_type.empty() ) {
            m_path += "." + extend_type;
        }
        return *this;
    }

  private:
    string m_path;
};

/*
 *  File类，目前已经完成了open, close, size, write, read等功能
 *  TODO: 未完成的功能有: (需要时完成)  sync,
 *
 *  TODO: 重构，用 fread, fwrite, fseek, fclose等函数进行重构
 */
class File {
  public:
    File( const char *path, OpenOptions options );
    File( string &path, OpenOptions options );
    File( string_view path, OpenOptions options );
    ~File() {
        if ( is_open() ) {
            close();
        }
    }

    bool is_open() {
        return file.is_open();
    }

    bool is_close() {
        return !file.is_open();
    }

    void close() {
        file.close();
    }
    // 将文件指针移动到文件开头
    void seek_to_start() {
        file.seekg( 0, file.beg );
    }
    // 将文件指针移动到文件末尾
    void seek_to_end() {
        file.seekg( 0, file.end );
    }

    // 从文件开头偏移offset个字节
    void seek_from_start( u64 offset ) {
        file.seekg( file.beg + offset );
    }

    // 从文件末尾偏移offset个字节
    void seek_from_end( u64 offset ) {
        file.seekg( file.end - offset );
    }

    // 从当前位置偏移offset个字节
    void seek_from_current( u64 offset ) {
        file.seekg( file.cur + offset );
    }

    void open( string &path, OpenOptions options ) {
        if ( file.is_open() ) {
            file.close();
        }

        auto mode = options.getMode();
        if ( mode.is_err() ) {
            std::cout << "open with invalid mode" << std::endl;
            throw runtime_error( "Invalid mode" );
        }
        file.open( path, mode.unwrap() );
        if ( !file ) {
            throw runtime_error( "Failed to open file" );
        }
    }

    void open( const char *path, OpenOptions options ) {
        if ( file.is_open() ) {
            file.close();
        }
        auto mode = options.getMode();
        if ( mode.is_err() ) {

            std::cout << "open with invalid mode" << std::endl;
            throw runtime_error( "Invalid mode" );
        }
        file.open( path, mode.unwrap() );
        if ( !file ) {
            throw runtime_error( "Failed to open file" );
        }
    }

    void open( string_view path, OpenOptions options ) {
        if ( file.is_open() ) {
            file.close();
        }
        auto mode = options.getMode();
        if ( mode.is_err() ) {
            std::cout << "open with invalid mode" << std::endl;
            throw runtime_error( "Invalid mode" );
        }
        file.open( path, mode.unwrap() );
        if ( !file ) {
            throw runtime_error( "Failed to open file" );
        }
    }

    u64 size() {
        auto original_position = file.cur;
        seek_to_end();
        u64 size = file.tellg();
        seek_from_start( original_position );
        return size;
    }

    Result< u64, string_view > write( vector< u8 > &buf ) {
        if ( !file.write( reinterpret_cast< char * >( buf.data() ),
                          buf.size() ) ) {
            return Err( string_view( "Failed to write file" ) );
        } else {
            return Ok( buf.size() );
        }
    }

    Result< u64, string_view > read( vector< u8 > &buf, u64 offset ) {
        u64 file_size = size();
        u64 read_size = 0;
        if ( file_size - offset < buf.size() ) {
            read_size = file_size - offset;
        } else {
            read_size = buf.size();
        }

        file.seekg( file.beg + offset );
        if ( !file.read( reinterpret_cast< char * >( buf.data() ),
                         read_size ) ) {
            return Err( string_view( "Failed to read file" ) );
        }
        return Ok( read_size );
    }

    static bool remove( string &path ) {
        return std::remove( path.c_str() );
    }
    static bool remove( const char *path ) {
        return std::remove( path );
    }
    static bool remove( string_view path ) {
        return std::remove( path.data() );
    }

    Result< u64, string_view > read_to_end( vector< u8 > &buf ) {
        u64 file_size = size();
        buf.resize( file_size );
        auto res = read( buf, 0 );
        if ( res.is_err() ) {
            return Err( res.unwrap_err() );
        }
        return Ok( file_size );
    }

    Result< u64, string_view > read_to_string( string &buf ) {
        u64 file_size = size();
        buf.resize( file_size );

        vector< u8 > vec_buf;
        auto         res = read_to_end( vec_buf );
        if ( res.is_err() ) {
            return Err( res.unwrap_err() );
        }
        buf.assign( vec_buf.begin(), vec_buf.end() );
        return Ok( file_size );
    }

  private:
    fstream file;
};

} // namespace bitcask