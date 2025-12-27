#pragma once

#include<QString>
#include<QDateTime>
#include<memory>

#include"../dao/RecordDao.h"
#include"../dao/GearDao.h
#include"../dao/UserDao.h"
#include"../model/GlobalEnum.hpp"

//给前端反馈借伞结果
struct ServiceResult{
    bool success;
    QString message; //提示信息
    double cost=0.0; //费用
};

class BorrowService{
public:
    //借伞
    ServiceResult borrowGear(const QString& userId, const QString& gearId);
    //还伞
    ServiceResult returnGear(const QString& userId, const QString& gearId, Station stationId, int slotId);
private:
    //计算费用
    double calculateCost(const QDateTime& borrowTime, const QDateTime& returnTime, GearType type);
    UserDao userDao;
    GearDao gearDao;
    RecordDao recordDao;
};

