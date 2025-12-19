-- 插入初始测试数据
-- 插入测试用户（首次登录时password为NULL，is_active为0，需要激活设置密码）
INSERT INTO users (user_id, password, real_name, role, credit, avatar_id, is_active) VALUES
('202483290399', NULL, 'cqw', 0, 100.00, 1, 0),
('202383290032', NULL, 'wy', 0, 50.00, 1, 0),
('202483290447', NULL, 'ljy', 0, 30.00, 1, 0),
('202511420317', NULL, 'cka', 0, 100.00, 1, 0),
('T001', NULL, 'teacher1', 1, 200.00, 2, 0),
('T002', NULL, 'teacher2', 1, 200.00, 2, 0),
('admin', '123456', 'admin', 9, 9999.00, 3, 1)
ON DUPLICATE KEY UPDATE user_id=user_id;

-- 插入站点数据（根据GlobalEnum.hpp中的Station枚举）
-- unavailable_slots字段：空字符串表示无故障槽位，示例"1,5"表示第1和第5号槽位故障
INSERT INTO station (name, pos_x, pos_y, status, unavailable_slots) VALUES
('文德楼', 0.30, 0.40, 1, ''),
('明德楼', 0.35, 0.45, 1, ''),
('图书馆', 0.50, 0.30, 1, ''),
('长望楼', 0.40, 0.35, 1, ''),
('藕舫楼', 0.45, 0.40, 1, ''),
('北辰楼', 0.55, 0.50, 1, ''),
('西苑宿舍楼1', 0.20, 0.70, 1, ''),
('西苑宿舍楼2', 0.25, 0.75, 1, ''),
('西苑宿舍楼3', 0.30, 0.80, 1, ''),
('中苑宿舍楼4', 0.60, 0.65, 1, ''),
('中苑宿舍楼5', 0.65, 0.70, 1, ''),
('中苑宿舍楼6', 0.70, 0.75, 1, ''),
('体育馆', 0.75, 0.60, 1, ''),
('行政楼', 0.50, 0.55, 1, '')
ON DUPLICATE KEY UPDATE name=name;

-- 雨具数据全部迁移到 data_insert2.0.sql，避免像之前一样重复插入...
-- 本脚本仅保留用户和站点数据


SELECT 'Data initialization completed successfully!' AS message;
SELECT COUNT(*) AS user_count FROM users;
SELECT COUNT(*) AS station_count FROM station;
SELECT COUNT(*) AS gear_count FROM raingear;

