#include "test.h"
#include "fio/file.h"
#include "fio/file_io.h"
#include "utils/Result.h"
#include "utils/macro.h"
#include "utils/type.h"
#include <string>
#include <string_view>
#include <thread>
#include <vector>

using namespace std;

using namespace bitcask;

void test_btree_put() {
    BTree        bt;
    string       str1 = "hello world";
    vector< u8 > vec_str1( str1.begin(), str1.end() );
    LogRecordPos pos( 1, 10 );
    auto         res1 = bt.put( vec_str1, pos );
    ASSERT_EQ( res1, true );

    string       str2 = "aa";
    vector< u8 > vec_str2( str2.begin(), str2.end() );
    LogRecordPos pos2( 2, 20 );
    auto         res2 = bt.put( vec_str2, pos2 );
    ASSERT_EQ( res2, true );
}
void test_btree_mutilthread_put() {
    // 创建多线程程序测试put函数的线程安全性
    BTree            bt;
    vector< thread > threads;
    for ( int i = 0; i < 10; i++ ) {
        threads.push_back( thread( [ &bt, i ]() {
            string       str = "hello world" + to_string( i );
            vector< u8 > vec_str( str.begin(), str.end() );
            LogRecordPos pos( 1, 10 );
            auto         res = bt.put( vec_str, pos );
            ASSERT_EQ( res, true );
        } ) );
    }
    for ( auto &thread : threads ) {
        thread.join();
    }
}
void test_btree_get() {
    BTree        bt;
    string       str1 = "hello world";
    vector< u8 > vec_str1( str1.begin(), str1.end() );
    LogRecordPos pos1( 1, 10 );
    auto         res1 = bt.put( vec_str1, pos1 );
    ASSERT_EQ( res1, true );

    string       str2 = "aa";
    vector< u8 > vec_str2( str2.begin(), str2.end() );
    LogRecordPos pos2( 2, 20 );
    auto         res2 = bt.put( vec_str2, pos2 );
    ASSERT_EQ( res2, true );

    LogRecordPos pos3 = bt.get( vec_str1 );
    ASSERT_EQ( pos3.file_id, pos1.file_id );
    ASSERT_EQ( pos3.offset, pos1.offset );

    LogRecordPos pos4 = bt.get( vec_str2 );
    ASSERT_EQ( pos4.file_id, pos2.file_id );
    ASSERT_EQ( pos4.offset, pos2.offset );
}

void test_btree_del() {
    BTree        bt;
    string       str1 = "hello world";
    vector< u8 > vec_str1( str1.begin(), str1.end() );
    LogRecordPos pos1( 1, 10 );
    auto         res1 = bt.put( vec_str1, pos1 );
    ASSERT_EQ( res1, true );

    string       str2 = "aa";
    vector< u8 > vec_str2( str2.begin(), str2.end() );
    LogRecordPos pos2( 2, 20 );
    auto         res2 = bt.put( vec_str2, pos2 );
    ASSERT_EQ( res2, true );

    auto del1 = bt.del( vec_str1 );
    ASSERT_EQ( del1, true );

    auto del2 = bt.del( vec_str2 );
    ASSERT_EQ( del2, true );
    string       str3 = "not exits";
    vector< u8 > vec_str3( str3.begin(), str3.end() );

    auto del3 = bt.del( vec_str3 );
    ASSERT_EQ( del3, false );
}

void test_file_io_read() {
    string path = "../../../../tmp/test_read.data";
    FileIO file_io( path );

    string       str = "key-a";
    vector< u8 > buf( str.begin(), str.end() );
    u64          write_size_1 = file_io.write( buf );
    ASSERT_EQ( 5, write_size_1 );

    string       str2 = "key-bb";
    vector< u8 > buf2( str2.begin(), str2.end() );
    u64          write_size_2 = file_io.write( buf2 );
    ASSERT_EQ( 6, write_size_2 );

    vector< u8 > read_buf1;
    read_buf1.resize( 5 );
    u64    read_size1 = file_io.read( read_buf1, 0 );
    string res1( read_buf1.begin(), read_buf1.end() );
    ASSERT_EQ( read_size1, 5 );
    ASSERT_EQ( res1, str );

    vector< u8 > read_buf2;
    read_buf2.resize( 7 );
    u64    read_size2 = file_io.read( read_buf2, 5 );
    string res2( read_buf2.begin(), read_buf2.end() - 1 );
    ASSERT_EQ( read_size2, 6 );
    ASSERT_EQ( res2, "key-bb" );

    file_io.close();
    remove( path.c_str() );
}

void test_file_io_write() {
    string path = "../../../../tmp/test_write.data";
    FileIO file_io( path );

    string       str = "key-a";
    vector< u8 > buf( str.begin(), str.end() );
    u64          write_size_1 = file_io.write( buf );
    ASSERT_EQ( 5, write_size_1 );

    string       str2 = "key-bb";
    vector< u8 > buf2( str2.begin(), str2.end() );
    u64          write_size_2 = file_io.write( buf2 );
    ASSERT_EQ( 6, write_size_2 );

    file_io.close();
    remove( path.c_str() );
}

auto Div( double x, double y ) -> Result< double, string > {
    if ( y == 0.0 )
        return Err( string{ "Division by zero" } );
    else
        return Ok( x / y );
}

void test_Result() {
    auto res1 = Div( 1.0, 2.0 ).unwrap();
}

void test_file_constructor_and_open() {
    OpenOptions options;
    options.read( true ).write( true ).append( true );
    File file( "../../../../tmp/test_file.data", options );
    cout << file.is_open() << endl;

    file.open( "../../../../tmp/test_file2.data", options );
    cout << file.is_open() << endl;
}

void test_file_read_and_write() {
    OpenOptions options;
    options.read( true ).write( true ).append( true );
    File   file( "../../../../tmp/test_file_read_and_write.data", options );
    string str1 = "hello world";
    vector< u8 > write_buf1( str1.begin(), str1.end() );
    auto         write_size1 = file.write( write_buf1 ).unwrap();
    ASSERT_EQ( write_size1, 11 );

    string       str2 = "key-a";
    vector< u8 > write_buf2( str2.begin(), str2.end() );
    auto         write_size2 = file.write( write_buf2 ).unwrap();
    ASSERT_EQ( write_size2, 5 );

    vector< u8 > read_buf1;
    read_buf1.resize( 11 );

    auto read_size1 = file.read( read_buf1, 0 ).unwrap();
    ASSERT_EQ( read_size1, 11 );
    ASSERT_EQ( string( read_buf1.begin(), read_buf1.end() ), str1 );

    vector< u8 > read_buf2;
    read_buf2.resize( 7 );
    auto read_size2 = file.read( read_buf2, 11 ).unwrap();
    ASSERT_EQ( read_size2, 5 );
    ASSERT_EQ( string( read_buf2.begin(), read_buf2.end() - 2 ), str2 );

    auto read_size3 = file.read( read_buf2, 1 ).unwrap();
    ASSERT_EQ( read_size3, 7 );
    ASSERT_EQ( string( read_buf2.begin(), read_buf2.end() ),
               string( "ello wo" ) );
}

string_view test_string_view_1() {
    char  *c_str1 = "hello world";
    string st2    = "1231";

    string_view sv1( c_str1 );
    return sv1;
}
string_view test_string_view_2() {
    string      str1 = "hello world";
    string_view sv2( str1 );
    return sv2;
}
void test_string_view() {
    string_view sv1 = test_string_view_1();
    cout << sv1 << endl;
    string_view sv2 = test_string_view_2();
    cout << sv2 << endl;
}

void test() {
    // test_btree_put();
    // test_btree_get();
    // test_btree_del();
    // test_btree_mutilthread_put();

    // test_file_io_write();
    // test_file_io_read();
    // test_Result();

    // test_file_read_and_write();
    test_string_view();
}
