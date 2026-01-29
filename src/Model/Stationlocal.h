/*
  定义了客户端用来表示借还站点的类 Stationlocal。
  posX 和 posY 表示站点的地理位置坐标，inventory是该站点当前库存的雨具列表。
  该类提供了添加雨具到库存和从库存中取出雨具的方法，以及查询当前库存数量的方法。
*/

#pragma once

#include <QSet>
#include <QString>
#include <memory>
#include <vector>

#include "GlobalEnum.hpp"
#include "RainGear.hpp"
#include "RainGearFactory.h"
#include "StationUtils.h"

constexpr int Station_capacity = 12; // 以一个站点最多容纳12把雨具为例，索引范围1-12

// 站点实体类，维护库存与位置信息
class Stationlocal{
public:
    Stationlocal(Station station, double posX, double posY);

    // getters
    Station get_station() const { return station; }
    QString get_name() const { return StationUtils::getChineseName(station); }
    double get_posX() const { return posX; }
    double get_posY() const { return posY; }
    bool get_online() const { return online; }
    // 库存总数量与可用数量
    int get_inventory_count() const;
    int get_available_count() const;
    // 判断某个槽位是否有雨具（不管是否可用）
    bool has_gear(int index) const;
    // 判断某个槽位的雨具是否可借（存在且状态为Available）
    bool is_gear_available(int index) const;
    // 判断某个槽位是否故障
    bool is_slot_broken(int index) const;


    // 管理员权限，可以添加、取出、标记雨具
    void add_gear(int index, std::unique_ptr<RainGear> gear); // 添加雨具到库存
    std::unique_ptr<RainGear> take_gear(int index); // 从库存中取出雨具
    void mark_unavailable(int index); // 标记雨具不可用（坏了）
    void mark_available(int index); // 标记雨具可用（修好了）
    void set_online(bool online); // 设置站点在线状态
 
private:
    Station station;
    double posX,posY;
    bool online = true; // 默认站点是在线状态
    std::vector<std::unique_ptr<RainGear>> inventory;  // 索引0不使用，索引1-12对应slot_id  
    QSet<int> unavailable_gears;   // 记录坏掉的槽位
};
