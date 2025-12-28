-- rainhub 测试数据插入脚本
-- 本文件只插入用户和站点数据，雨具数据请运行data_insert2.0.sql

use rainhub_db;

-- 插入测试用户
-- 首次登录时 password 为 null, is_active 为 0, 需要激活设置密码
-- role: 0=学生, 1=教职工, 9=管理员
insert into users (user_id, password, real_name, role, credit, is_active) values
('202483290399', null, 'cqw', 0, 100.00, 0),
('202383290032', null, 'wy', 0, 50.00, 0),
('202483290447', null, 'ljy', 0, 30.00, 0),
('202511420317', null, 'cka', 0, 100.00, 0),
('T001', null, 'teacher1', 1, 200.00, 0),
('T002', null, 'teacher2', 1, 200.00, 0),
('admin', '123456', 'admin', 9, 9999.00, 1)
on duplicate key update user_id=user_id;

-- 插入站点数据
-- station_id 自增，对应 GlobalEnum.hpp 中的Station枚举
insert into station (name, pos_x, pos_y, status, unavailable_slots) values
('文德楼', 0.20, 0.40, 1, ''),
('明德楼', 0.75, 0.20, 1, ''),
('图书馆', 0.15, 0.15, 1, ''),
('长望楼', 0.35, 0.20, 1, ''),
('藕舫楼', 0.55, 0.25, 1, ''),
('北辰楼', 0.50, 0.35, 1, ''),
('西苑宿舍楼1', 0.10, 0.70, 1, ''),
('西苑宿舍楼2', 0.25, 0.80, 1, ''),
('西苑宿舍楼3', 0.40, 0.90, 1, ''),
('中苑宿舍楼4', 0.55, 0.75, 1, ''),
('中苑宿舍楼5', 0.70, 0.85, 1, ''),
('中苑宿舍楼6', 0.85, 0.75, 1, ''),
('体育馆', 0.65, 0.55, 1, ''),
('行政楼', 0.80, 0.40, 1, '')
on duplicate key update name=name;

select 'data_insert.sql executed successfully!' as message;
select concat('users: ', count(*)) as count from users;
select concat('stations: ', count(*)) as count from station;
