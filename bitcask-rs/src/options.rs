use std::path::PathBuf;

#[derive(Clone)]
/// 配置项
pub struct Options {
    /// 数据库目录
    pub dir_path: PathBuf,

    /// 数据文件大小
    pub data_file_size: u64,

    /// 是否每次写都持久化
    pub sync_writes: bool,

    /// 索引类型
    pub index_type: IndexType,
}

#[derive(Clone)]
pub enum IndexType {
    /// Btree 索引
    BTree,

    /// 跳表索引
    SkipLisk,
}
