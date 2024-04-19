#pragma once
#include "../data/log_record.h"
#include <cstdint>
#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>
using namespace std;

using u8 = uint8_t;

class Indexer {
  public:
    virtual ~Indexer() = default;

    virtual bool         put( vector< u8 > key, LogRecordPos pos ) = 0;
    virtual LogRecordPos get( vector< u8 > key )                   = 0;
    virtual bool         del( vector< u8 > key )                   = 0;
};
class BTree : public Indexer {
  public:
    BTree()
        : tree( make_shared< map< vector< u8 >, LogRecordPos > >() ) {
    }

    bool         put( vector< u8 > key, LogRecordPos pos ) override;
    LogRecordPos get( vector< u8 > key ) override;
    bool         del( vector< u8 > key ) override;

  private:
    shared_ptr< map< vector< u8 >, LogRecordPos > > tree;
    shared_mutex                                    RWLock;
};
