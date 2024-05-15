use std::result;
use thiserror::Error;

#[derive(Error, Debug, Clone, Copy)]
pub enum Errors {
    #[error("failed to read from data file")]
    FailedToReadFromDataFile,

    #[error("failed to write to data file")]
    FailedToWriteToDataFile,

    #[error("failed to sync data file")]
    FailedToSyncDataFile,

    #[error("failed to open data file")]
    FailedToOpenDataFile,

    #[error("the key is empty")]
    KeyIsEmpty,

    #[error("the key is not found in database")]
    KeyNotFound,

    #[error("memory index failed to update")]
    IndexUpdateFailed,

    #[error("the data file is not found in database")]
    DataFileNotFound,

    #[error("the dir path is empty")]
    DirPathIsEmpty,

    #[error("the data file size is Invalid")]
    DataFileSizeIsInvalid,

    #[error("failed to create database dir")]
    FailedToCreateDatabaseDir,

    #[error("failed to read database dir")]
    FailedToReadDatabaseDir,

    #[error("database dir maybe corrupted")]
    DataDirCorrupted,
}

pub type Result<T> = result::Result<T, Errors>;
