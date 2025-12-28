-- rainhub 校园智能共享雨具系统
-- 数据库初始化脚本

-- 创建数据库
create database if not exists rainhub_db 
    character set utf8mb4 
    collate utf8mb4_unicode_ci;

use rainhub_db;

-- 用户表
-- role: 0=学生, 1=教职工, 9=管理员
-- is_active: 0=未激活(首次登录需设置密码), 1=已激活
create table if not exists users (
    user_id varchar(20) not null,
    password varchar(64) null,
    real_name varchar(20) not null,
    role int not null default 0,
    credit decimal(10, 2) not null default 0.00,
    is_active tinyint(1) not null default 0,
    primary key (user_id),
    index idx_role (role)
) engine=innodb default charset=utf8mb4;

-- 站点表
-- station_id 对应 GlobalEnum.hpp 中的 Station 枚举值
-- status: 1=在线, 0=离线
-- unavailable_slots: 故障槽位，逗号分隔，如 "1,5,8"
create table if not exists station (
    station_id int not null auto_increment,
    name varchar(50) not null,
    pos_x float not null,
    pos_y float not null,
    status int not null default 1,
    unavailable_slots varchar(50) not null default '',
    primary key (station_id)
) engine=innodb default charset=utf8mb4;

-- 雨具表
-- type_id: 1=普通塑料伞, 2=高质量抗风伞, 3=专用遮阳伞, 4=雨衣
-- status: 0=Unknown, 1=Available可用, 2=Borrowed借出, 3=Broken损坏
-- slot_id: 1-12 对应每个站点的12个槽位
-- 槽位分配规则: 1-4普通塑料伞, 5-8高质量抗风伞, 9-10遮阳伞, 11-12雨衣
create table if not exists raingear (
    gear_id varchar(20) not null,
    type_id int not null,
    station_id int null,
    slot_id int null,
    status int not null default 1,
    primary key (gear_id),
    index idx_station (station_id),
    index idx_status (status),
    foreign key (station_id) references station(station_id) on delete set null on update cascade
) engine=innodb default charset=utf8mb4;

-- 借还记录表
-- return_time 为 null 表示未归还
create table if not exists record (
    record_id bigint not null auto_increment,
    user_id varchar(20) not null,
    gear_id varchar(20) not null,
    borrow_time datetime not null,
    return_time datetime null,
    cost decimal(10, 2) not null default 0.00,
    primary key (record_id),
    index idx_user (user_id),
    index idx_gear (gear_id),
    foreign key (user_id) references users(user_id) on delete restrict on update cascade,
    foreign key (gear_id) references raingear(gear_id) on delete restrict on update cascade
) engine=innodb default charset=utf8mb4;

select 'init_db.sql executed successfully!' as message;
