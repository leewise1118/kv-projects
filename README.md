# bitcask 数据库

## 数据读写流程
数据读写流程分为两步，先写磁盘数据文件，再更新内存索引。

## 数据库启动流程
1. 加载数据目录中的文件，打开其文件描述符.
    1. 打开用户传递的数据目录
        - 如果目录下没有文件，则表示空的数据库，直接返回。
2. 遍历数据文件中的内容，构建内存索引.


## Rust crate介绍
### Bytes
