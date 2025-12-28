/*
    管理员雨具管理服务实现
*/
#include "Admin_GearService.h"
#include "../utils/ConnectionPool.h"

//获取所有雨具
QVector<GearInfoDTO> Admin_GearService::getAllGears(int stationId, int slotId) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    return gearDao.selectAllDTO(db, stationId, slotId);
}

//更新雨具状态
bool Admin_GearService::updateGearStatus(const QString& gearId, int newStatus) {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return false;
    return gearDao.updateStatus(db, gearId, newStatus);
}

//获取总借出数量
int Admin_GearService::getTotalBorrowedCount() {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return 0;
    return gearDao.countByStatus(db, 2); //status=2是Borrowed
}

//获取总故障数量
int Admin_GearService::getTotalBrokenCount() {
    QSqlDatabase db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return 0;
    return gearDao.countByStatus(db, 3); //status=3是Broken
}
