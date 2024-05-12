#include "test.h"
#include "fio/file_io.h"
#include "utils/macro.h"
#include <thread>
#include <vector>

using namespace std;

namespace bitcask {

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
}
void test_file_io_write() {
    string path = "./b.data";
    FileIO file_io( path );

    string       str = "key-a";
    vector< u8 > buf( str.begin(), str.end() );
    u64          write_size_1 = file_io.write( buf );
    ASSERT_EQ( 5, write_size_1 );
}
void test() {
    test_btree_put();
    test_btree_get();
    test_btree_del();
    test_btree_mutilthread_put();

    test_file_io_write();
    test_file_io_read();
}

} // namespace bitcask