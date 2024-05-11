#pragma once
#include "../utils/type.h"
#include <iostream>
#include <memory>
using namespace std;

namespace bitcask {

class DataFile {
  public:
  private:
    shared_ptr< u32 > file_id;
    shared_ptr< u64 > write_off;
};

} // namespace bitcask