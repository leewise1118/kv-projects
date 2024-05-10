#include "./data/log_record.h"
#include "./index/BTree.h"
#include <iostream>

using namespace std;

int main( int argc, char **argv ) {
    cout << "hello world!" << endl;
    auto i = 0;
    return 0;
}

// test

void test_btree_put() {
    BTree        bt;
    string       str1 = "hello world";
    vector< u8 > vec_str1( str1.begin(), str1.end() );
    LogRecordPos pos( 1, 10 );
    auto         res1 = bt.put( vec_str1, pos );
}