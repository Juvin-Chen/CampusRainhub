/*
  定义共同抽象基类雨具类RainGear，作为所有雨具的基类
*/
#pragma once

#include <QString>
#include "GlobalEnum.hpp"

// 抽象基类，描述雨具的共有属性与行为
class RainGear {
public:
    RainGear(QString id, GearType type, GearStatus status=GearStatus::Available, Station station_id=Station::Unknown, int slot_id=0):id(std::move(id)),type(type),status(status),station_id(station_id),slot_id(slot_id){}
    virtual ~RainGear()=default;

    // getters
    QString get_id() const { return id; }
    GearType get_type() const { return type; }
    GearStatus get_status() const { return status; }
    Station get_station_id() const { return station_id; }
    int get_slot_id() const { return slot_id; }
    bool is_available() const { return status == GearStatus::Available; }  // 唯一可借状态

    // setters
    void set_status(GearStatus s) { this->status = s; }
    void set_station_id(Station station_id) { this->station_id = station_id; }
    void set_slot_id(int slot_id) { this->slot_id = slot_id; }

    // 纯虚接口：不同品类押金与图标不同。
    virtual double get_deposit() const = 0; // 获取押金金额
    virtual QString get_iconpath() const = 0; // 获取图标资源路径
private:
    QString id; // 雨具编号 RFID
    GearType type; // 雨具类型
    GearStatus status; // 状态
    Station station_id=Station::Unknown;
    int slot_id=0; // 槽位
};
