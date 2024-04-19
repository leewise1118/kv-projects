use std::{collections::HashMap, fmt::Error, io::SeekFrom, sync::Arc};

use crate::{
    data::{
        data_file::DataFile,
        log_record::{self, LogRecord, LogRecordPos, LogRecordType},
    },
    errors::{Errors, Result},
    index,
    options::Options,
};
use bytes::Bytes;
use parking_lot::RwLock;

/// bitcask 存储引擎实例结构体
pub struct Engine {
    /// 配置项
    options: Arc<Options>,

    /// 当前活跃数据文件
    active_file: Arc<RwLock<DataFile>>,

    /// 旧的数据文件
    older_files: Arc<RwLock<HashMap<u32, DataFile>>>,

    /// 内存索引
    index: Box<dyn index::Indexer>,
}

impl Engine {
    pub fn put(&self, key: Bytes, value: Bytes) -> Result<()> {
        if key.is_empty() {
            return Err(Errors::KeyIsEmpty);
        }

        // 构造LogRecord
        let mut record = LogRecord {
            key: key.to_vec(),
            value: value.to_vec(),
            rec_type: LogRecordType::NORMAL,
        };

        // 追加写入到活跃数据文件中
        let log_record_pos = self.append_log_record(&mut record)?;

        // 更新内存索引
        let ok = self.index.put(key.to_vec(), log_record_pos);

        if !ok {
            return Err(Errors::IndexUpdateFailed);
        }

        Ok(())
    }

    /// 根据 key 获取对应的数据
    pub fn get(&self, key: Bytes) -> Result<Bytes> {
        // 判断key的有效性
        if key.is_empty() {
            return Err(Errors::KeyIsEmpty);
        }

        // 从内存索引中获取 key 对应的位置信息
        let pos = self.index.get(key.to_vec());
        let log_record_pos = pos.unwrap();

        // 如果key不存在，则直接返回
        if pos.is_none() {
            return Err(Errors::KeyNotFound);
        }

        // 从对应的数据文件中获取对应的 LogRecord
        let active_file = self.active_file.read();
        let older_files = self.older_files.read();

        let log_record = match active_file.get_file_id() == log_record_pos.file_id {
            true => active_file.read_log_record(log_record_pos.offset)?,
            false => {
                let old_file = older_files.get(&log_record_pos.file_id);
                if old_file.is_none() {
                    return Err(Errors::DataFileNotFound);
                }
                old_file.unwrap().read_log_record(log_record_pos.offset)?
            }
        };

        // 判断 LogRecord 的类型
        if log_record.rec_type == LogRecordType::DELETED {
            return Err(Errors::KeyNotFound);
        }

        // 返回对应的value信息
        Ok(log_record.value.into())
    }

    fn append_log_record(&self, record: &mut LogRecord) -> Result<LogRecordPos> {
        let dir_path = self.options.dir_path.clone();

        // 对输入数据进行编码
        let enc_record = record.encode();
        let record_len = enc_record.len() as u64;

        // 获取当前活跃文件
        let mut active_file = self.active_file.write();

        // 判断当前活跃文件是否到达了阈值
        if active_file.get_write_off() + record_len > self.options.data_file_size {
            // 将当前活跃文件进行持久化
            active_file.sync()?;

            let current_fid = active_file.get_file_id();

            // 将旧的数据文件放入map
            let mut older_files = self.older_files.write();

            let old_file = DataFile::new(&dir_path, current_fid)?;
            older_files.insert(current_fid, old_file);

            // 打开行的数据文件
            let new_file = DataFile::new(&dir_path, current_fid + 1)?;

            *active_file = new_file;
        }

        // 追加写入数据到活跃文件中
        let write_off = active_file.get_write_off();
        active_file.write(&enc_record)?;

        if self.options.sync_writes {
            active_file.sync()?;
        }

        // 构造内存索引信息
        Ok(LogRecordPos {
            file_id: active_file.get_file_id(),
            offset: write_off,
        })
    }
}
