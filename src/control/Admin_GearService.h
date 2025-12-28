/*
    管理员雨具管理服务
*/
#pragma once

#include <QString>
#include <QVector>
#include "../dao/GearDao.h"

class Admin_GearService {
public:
    QVector<GearInfoDTO> getAllGears(int stationId = 0, int slotId = 0); //获取所有雨具
    bool updateGearStatus(const QString& gearId, int newStatus); //更新雨具状态
    int getTotalBorrowedCount(); //获取总借出数量
    int getTotalBrokenCount(); //获取总故障数量
private:
    GearDao gearDao;
};
