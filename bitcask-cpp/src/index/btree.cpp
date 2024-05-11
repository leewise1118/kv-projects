#include "btree.h"
#include <mutex>
#include <shared_mutex>

namespace bitcask {

bool BTree::put( vector< u8 > key, LogRecordPos pos ) {
    // 写锁，独占
    unique_lock< shared_mutex > Wlock( RWLock );
    auto                        result = tree->insert( { key, pos } );
    return result.second;
}

LogRecordPos BTree::get( vector< u8 > key ) {
    // 读锁，共享
    shared_lock< shared_mutex > Rlock( RWLock );
    auto                        result = tree->at( key );

    return result;
}

bool BTree::del( vector< u8 > key ) {
    // 读锁，共享
    shared_lock< shared_mutex > Rlock( RWLock );
    auto                        iter = tree->find( key );
    if ( iter != tree->end() ) {
        tree->erase( iter );
        return true;
    }
    return false;
}
} // namespace bitcask