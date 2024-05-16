use std::{collections::HashMap, fmt::Error, fs, io::SeekFrom, path::PathBuf, sync::Arc};

use crate::{
    data::{
        data_file::{DataFile, DATA_FILE_NAME_EXTENSION},
        log_record::{LogRecord, LogRecordPos, LogRecordType},
    },
    errors::{Errors, Result},
    index,
    options::Options,
};
use bytes::Bytes;
use log::warn;
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

    /// 数据库启动时的文件id, 只用于记载索引时使用，不能在其他地方更新或使用
    file_ids: Vec<u32>,
}

impl Engine {
    /// 打开 bitcask 引擎存储实例。
    pub fn open(opts: Options) -> Result<Self> {
        let options = opts.clone();
        // 校验配置项是否合法
        if let Some(e) = check_options(opts) {
            return Err(e);
        }

        // 判断目录是否存在，如果不存在则创建这个目录
        let dir_path = options.dir_path.clone();
        if !dir_path.is_dir() {
            if let Err(e) = fs::create_dir_all(dir_path.clone()) {
                warn!("create database dir err: {}", e);
                return Err(Errors::FailedToCreateDatabaseDir);
            }
        }

        // 加载文件
        let mut data_files = load_data_files(dir_path.clone())?;

        // 设置 file id 信息。
        let mut file_ids = Vec::new();
        for v in data_files.iter() {
            file_ids.push(v.get_file_id());
        }

        // 将旧文件保存在 older_files中
        let mut older_files = HashMap::new();
        if data_files.len() > 1 {
            for _ in 0..=data_files.len() - 2 {
                let data_file = data_files.pop().unwrap();
                older_files.insert(data_file.get_file_id(), data_file);
            }
        }

        // 拿到当前活跃文件，即列表最后一个文件
        let active_file = match data_files.pop() {
            Some(v) => v,
            None => DataFile::new(dir_path.clone(), 0)?,
        };

        // 构建存储引擎实例
        let mut engine = Self {
            options: Arc::new(options.clone()),
            active_file: Arc::new(RwLock::new(active_file)),
            older_files: Arc::new(RwLock::new(older_files)),
            index: Box::new(index::new_index(options.index_type)),
            file_ids,
        };

        // 从数据文件中加载索引
        engine.load_index_from_data_files();

        Ok(engine)
    }

    pub fn put(&self, key: Bytes, value: Bytes) -> Result<()> {
        // 判断key的有效性
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
            true => active_file.read_log_record(log_record_pos.offset)?.record,
            false => {
                let old_file = older_files.get(&log_record_pos.file_id);
                if old_file.is_none() {
                    return Err(Errors::DataFileNotFound);
                }
                old_file
                    .unwrap()
                    .read_log_record(log_record_pos.offset)?
                    .record
            }
        };

        // 判断 LogRecord 的类型
        if log_record.rec_type == LogRecordType::DELETED {
            return Err(Errors::KeyNotFound);
        }

        // 返回对应的value信息
        Ok(log_record.value.into())
    }

    /// 追加写数据到当前活跃文件中
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

            let old_file = DataFile::new(dir_path.clone(), current_fid)?;
            older_files.insert(current_fid, old_file);

            // 打开行的数据文件
            let new_file = DataFile::new(dir_path, current_fid + 1)?;

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

    /// 从数据文件中加载内存索引
    /// 遍历文件中的内容，并依次处理其中的记录
    fn load_index_from_data_files(&mut self) -> Result<()> {
        // 数据文件为空，直接返回
        if self.file_ids.is_empty() {
            return Ok(());
        }

        let active_files = self.active_file.read();
        let older_files = self.older_files.read();

        // 遍历文件id
        for (i, file_id) in self.file_ids.iter().enumerate() {
            let mut offset = 0;
            loop {
                let log_record_res = match *file_id == active_files.get_file_id() {
                    true => active_files.read_log_record(offset),
                    false => {
                        let older_file = older_files.get(file_id).unwrap();
                        older_file.read_log_record(offset)
                    }
                };

                let (log_record, size) = match log_record_res {
                    Ok(v) => (v.record, v.size),
                    Err(e) => {
                        if e == Errors::ReadDataFileEOF {
                            break;
                        }
                        return Err(e);
                    }
                };
                // 构建内存索引
                let log_record_pos = LogRecordPos {
                    file_id: *file_id,
                    offset,
                };
                match log_record.rec_type {
                    LogRecordType::NORMAL => {
                        self.index.put(log_record.key.to_vec(), log_record_pos);
                    }
                    LogRecordType::DELETED => {
                        self.index.delete(log_record.key.to_vec());
                    }
                }

                // 递增offset， 下一次读取的时候从新的位置开始
                offset += size;
            }

            // 设置活跃文件的offset
            if i == self.file_ids.len() - 1 {
                active_files.set_write_off(offset);
            }
        }
        Ok(())
    }
}

fn check_options(opts: Options) -> Option<Errors> {
    let dir_path = opts.dir_path.to_str();
    if dir_path.is_none() || dir_path.unwrap().len() == 0 {
        return Some(Errors::DataFileNotFound);
    }

    if opts.data_file_size <= 0 {
        return Some(Errors::DataFileSizeIsInvalid);
    }
    None
}

// 从数据目录中加载数据文件
fn load_data_files(dir_path: PathBuf) -> Result<Vec<DataFile>> {
    // 读取数据目录
    let dir = fs::read_dir(dir_path.clone());
    if dir.is_err() {
        return Err(Errors::FailedToReadDatabaseDir);
    }
    let mut file_ids: Vec<u32> = Vec::new();
    let mut data_files: Vec<DataFile> = Vec::new();

    for file in dir.unwrap() {
        if let Ok(entry) = file {
            // 拿到文件名
            let file_os_str = entry.file_name();
            let file_name = file_os_str.to_str().unwrap();

            // 判断文件是不是以 .data 扩展名
            if file_name.ends_with(DATA_FILE_NAME_EXTENSION) {
                let split_name: Vec<&str> = file_name.split(".").collect();
                let file_id = match split_name[0].parse::<u32>() {
                    Ok(fid) => fid,
                    Err(_) => {
                        return Err(Errors::DataDirCorrupted);
                    }
                };

                file_ids.push(file_id);
            }
        }
    }

    // 如果没有数据文件，则直接返回。
    if file_ids.is_empty() {
        return Ok(data_files);
    }
    // 对文件id进行排序，从小到大进行记载，
    file_ids.sort();

    // 遍历文件所有id，以此打开文件
    for file_id in file_ids {
        let file = DataFile::new(dir_path.clone(), file_id)?;
        data_files.push(file);
    }

    Ok(data_files)
}
