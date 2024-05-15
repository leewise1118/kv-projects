pub mod btree;
use crate::data::log_record::LogRecordPos;

/// Indexer 索引trait ，后续如果想要接入其他数据结构，则直接实现这个接口。
pub trait Indexer: Sync + Send {
    /// put 向索引中存储 key 对应的数据位置信息
    fn put(&self, key: Vec<u8>, pos: LogRecordPos) -> bool;

    /// get 根据 key 取出对应的索引位置信息
    fn get(&self, key: Vec<u8>) -> Option<LogRecordPos>;

    /// delete 根据 key 删除对应的索引位置信息
    fn delete(&self, key: Vec<u8>) -> bool;
}

pub fn new_index() {}
