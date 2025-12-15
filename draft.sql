-- 这里放一些常用的查询语句，需要用的时候可以直接copy，方便一点

-- 这个是删除数据库，用于重建
DROP DATABASE IF EXISTS rainhub_db; 

-- 查看数据库中的内容
use rainhub_db;
show tables;

-- 查询四个表的语句
select * from raingear;
select * from users;
select * from station;
select * from record;
