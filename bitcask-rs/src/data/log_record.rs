/// 数据位置索引信息，描述数据存储到了哪个位置
#[derive(Clone, Copy)]
pub struct LogRecordPos {
    /// 文件id
    pub(crate) file_id: u32,
    /// 偏移信息
    pub(crate) offset: u64,
}
