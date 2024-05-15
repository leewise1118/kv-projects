use log::error;
use parking_lot::RwLock;
use std::fs::OpenOptions;

use std::path::PathBuf;
use std::sync::Arc;
use std::{fs::File, io::Write};

#[cfg(target_os = "windows")]
use std::os::windows::prelude::FileExt;

#[cfg(target_os = "macos")]
use std::os::unix::prelude::FileExt;

use super::IOManager;
use crate::errors::{Errors, Result};

/// FileIO 标准系统文件 IO
pub struct FileIO {
    /// 系统文件描述符
    fd: Arc<RwLock<File>>,
}

impl FileIO {
    pub fn new(file_name: PathBuf) -> Result<Self> {
        match OpenOptions::new()
            .create(true)
            .read(true)
            .write(true)
            .append(true)
            .open(file_name)
        {
            Ok(f) => {
                return Ok(Self {
                    fd: Arc::new(RwLock::new(f)),
                });
            }
            Err(e) => {
                error!("failed to open data file {}", e);
                return Err(Errors::FailedToOpenDataFile);
            }
        }
    }
}
impl IOManager for FileIO {
    fn read(&self, buf: &mut [u8], offset: u64) -> Result<usize> {
        let read_guard = self.fd.read();

        #[cfg(target_os = "windows")]
        match read_guard.seek_read(buf, offset) {
            Ok(n) => return Ok(n),
            Err(e) => {
                error!("read from data file err: {}", e);
                return Err(Errors::FailedToReadFromDataFile);
            }
        }

        #[cfg(target_os = "macos")]
        match read_guard.read_at(buf, offset) {
            Ok(n) => return Ok(n),
            Err(e) => {
                error!("read from data file err: {}", e);
                return Err(Errors::FailedToReadFromDataFile);
            }
        }
    }

    fn write(&self, buf: &[u8]) -> Result<usize> {
        let mut write_guard = self.fd.write();
        match write_guard.write(buf) {
            Ok(n) => return Ok(n),
            Err(e) => {
                error!("write to data file err: {}", e);
                return Err(Errors::FailedToWriteToDataFile);
            }
        };
    }

    fn sync(&self) -> Result<()> {
        let read_guard = self.fd.read();
        if let Err(e) = read_guard.sync_all() {
            error!("failed to sync data file: {}", e);
            return Err(Errors::FailedToSyncDataFile);
        };
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use std::{fs, path::PathBuf};

    use super::*;

    #[test]
    fn test_file_io_write() {
        let path = PathBuf::from("./tmp/a.data");
        let file_io = FileIO::new(path.clone());
        assert!(file_io.is_ok());

        let fio = file_io.ok().unwrap();
        let res1 = fio.write("key-a".as_bytes());
        assert!(res1.is_ok());
        assert_eq!(5, res1.ok().unwrap());

        let res2 = fio.write("key-b".as_bytes());
        assert!(res2.is_ok());
        assert_eq!(5, res2.ok().unwrap());

        let res3 = fs::remove_file(path.clone());
        assert!(res3.is_ok());
    }

    #[test]
    fn test_file_io_read() {
        let path = PathBuf::from("./tmp/b.data");
        let file_io = FileIO::new(path.clone());
        assert!(file_io.is_ok());

        let fio = file_io.ok().unwrap();
        let res1 = fio.write("key-a".as_bytes());
        assert!(res1.is_ok());
        assert_eq!(5, res1.ok().unwrap());

        let res2 = fio.write("key-b".as_bytes());
        assert!(res2.is_ok());
        assert_eq!(5, res2.ok().unwrap());

        let mut buf1 = [0u8; 5];
        let read_res1 = fio.read(&mut buf1, 0);
        assert!(read_res1.is_ok());
        assert_eq!(read_res1.unwrap(), 5);
        assert_eq!(buf1, "key-a".as_bytes());

        let mut buf2 = [0u8; 5];
        let read_res2 = fio.read(&mut buf2, 5);
        assert!(read_res2.is_ok());
        assert_eq!(read_res2.clone().unwrap(), 5);
        assert_eq!(buf2, "key-b".as_bytes());

        let res = fs::remove_file(path.clone());
        assert!(res.is_ok());
    }

    #[test]
    fn test_file_io_sync() {
        let path = PathBuf::from("./tmp/c.data");
        let file_io = FileIO::new(path.clone());
        assert!(file_io.is_ok());

        let fio = file_io.ok().unwrap();
        let res1 = fio.write("key-a".as_bytes());
        assert!(res1.is_ok());
        assert_eq!(5, res1.ok().unwrap());

        let res2 = fio.write("key-b".as_bytes());
        assert!(res2.is_ok());
        assert_eq!(5, res2.ok().unwrap());

        let sync_res = fio.sync();
        assert!(sync_res.is_ok());

        let res3 = fs::remove_file(path.clone());
        assert!(res3.is_ok());
    }
}
