#pragma once

#include<QSqlDatabase>
#include<vector>
#include<memory>
#include<QString>
#include<optional>

#include"../Model/RainGear.hpp"
#include"../Model/GlobalEnum.hpp"
#include"../Model/RainGear_subclasses.hpp"

class GearDao{
public:
    std::unique_ptr<RainGear> selectById(QSqlDatabase& db, const QString& id); //根据id查询雨具

    std::vector<std::unique_ptr<RainGear>> selectByStation(QSqlDatabase& db, Station station); //根据站点查询雨具
    
    bool isSlotOccupied(QSqlDatabase& db, Station station, int slot_id); //检查槽位是否被占用
    bool insert(QSqlDatabase& db, const QString& gearId, GearType type, Station stationId, int slotId); //插入雨具
    bool deleteById(QSqlDatabase& db, const QString& id); //删除雨具
    bool updateStatusAndLocation(QSqlDatabase& db, const QString& id, GearStatus status, Station station, int slot_id); //更新雨具状态和位置
};