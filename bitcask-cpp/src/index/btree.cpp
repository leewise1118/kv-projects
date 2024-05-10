#include "BTree.h"

bool BTree::put( vector<u8> key, LogRecordPos pos ) {
    RWLock.lock();
    auto result = tree->insert( { key, pos } );
    RWLock.unlock();

    return result.second;
}

LogRecordPos BTree::get( vector<u8> key ) {
    RWLock.lock_shared();
    auto result = tree->at( key );
    RWLock.unlock_shared();

    return result;
}

bool BTree::del( vector<u8> key ) {
    RWLock.lock();
    auto iter = tree->find( key );
    if ( iter != tree->end() ) {
        tree->erase( iter );
        return true;
    }  
    return false;
   
}