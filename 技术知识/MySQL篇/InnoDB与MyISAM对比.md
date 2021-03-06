## InnoDB存储引擎介绍

InnoDB引擎是Mysql的默认的存储引擎，他有很多自己的特性，下面一一列举。

- **支持事务**，InnoDB存储引擎主要就是为了在线事务处理（OLTP）的应用而设计的。
- **行锁设计**，支持外键，非锁定读。
- 支持多版本的并发控制（MVCC）来获得高并发性。
- 提供了插入缓冲、二次写、自适应哈希索引、预读等高性能和高可用的功能。

上面这些算是 InnoDB 存储引擎的一些特点了，也是它的优势所在，为什么 InnoDB 引擎会使用如此广泛，就是因为它能有很好的性能。

## MyISAM储存引擎介绍

- **不支持事务**，它的设计目标是面向在线分析的应用（OLAP）。
- 支持全文索引。
- **表锁设计**。
- 它的缓冲池只缓冲**索引文件**，**不缓冲数据文件**，所以 MyISAM 存储引擎表由 `MYD` 和 `MYI` 组成，前者存储数据文件，后者存储索引文件。

## 存储引擎之间的对比

这一部分，主要简要的介绍一下各个存储引擎之间的差别，及主要的作用及特点。

#### 特性对比

| 特性               | MyISAM                                                     | InnoDB                                                       | BDB         | Memory                                                       | Archive                                | NDB  |
| ------------------ | ---------------------------------------------------------- | ------------------------------------------------------------ | ----------- | ------------------------------------------------------------ | -------------------------------------- | ---- |
| 存储限制           | 无                                                         | 64TB                                                         | 无          | 有                                                           | 无                                     | 有   |
| 事务               |                                                            | 支持                                                         | 支持        |                                                              |                                        |      |
| 锁级别             | 表锁                                                       | 行锁                                                         | page        | 表                                                           | 行                                     | 行   |
| MVCC（并发控制）   |                                                            | 支持                                                         |             |                                                              | 支持                                   | 支持 |
| 全文索引           | 支持                                                       |                                                              |             |                                                              |                                        |      |
| 集群索引           |                                                            | 支持                                                         |             |                                                              |                                        |      |
| 数据缓存和索引缓存 |                                                            | 支持                                                         |             | 支持                                                         |                                        | 支持 |
| 数据压缩           | 支持                                                       |                                                              |             |                                                              | 支持                                   |      |
| 批量插入速度       | 高                                                         | 低                                                           | 高          | 高                                                           | 很高                                   | 高   |
| 集群数据库支持     |                                                            |                                                              |             |                                                              |                                        | 支持 |
| 外键支持           |                                                            | 支持                                                         |             |                                                              |                                        |      |
| 适用场景           | 不需要事务的操作；插入、更新少，读取频繁；频繁的统计计算。 | 需要事务的操作；更新数据需要使用行级锁；大数据量读写；大型互联网应用。 | 类似 InnoDB | 数据量不大，需要被频繁的访问，而且数据丢失不会对业务产生比较严重的影响。 | 存储引擎基本上用于数据归档，作为日志表 | 集群 |