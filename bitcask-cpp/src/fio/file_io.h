#pragma once
#include "../data/data_file.h"
#include "../utils/IOManager.h"
#include "../utils/type.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

namespace bitcask {

class FileIO : public IOManager {
  public:
    FileIO( string &file_path ) {
        file = make_shared< fstream >( file_path, fstream::in | fstream::out |
                                                      fstream::app );
        if ( !*file ) {
            throw runtime_error( "Failed to open file" );
        }
    }
    ~FileIO() override {
        file->close();
    }
    u64  read( vector< u8 > &buf, u64 offset ) override;
    u64  write( vector< u8 > &buf ) override;
    void sync() override;

  private:
    shared_mutex          mutex;
    shared_ptr< fstream > file;
};

} // namespace bitcask