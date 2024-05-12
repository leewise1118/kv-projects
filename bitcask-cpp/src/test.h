#pragma once

#include "./data/log_record.h"
#include "./index/btree.h"
#include "utils/macro.h"
#include "utils/type.h"

#include <iostream>
using namespace std;

namespace bitcask {

void test_btree_put();
void test_btree_get();
void test_btree_del();
void test_btree_mutilthread_put();

void test_file_io_read();
void test_file_io_write();

void test();

} // namespace bitcask