#include "StationService.h"
#include "../utils/ConnectionPool.h"
#include <QDebug>

//获取所有站点
QVector<std::unique_ptr<Stationlocal>> StationService::getAllStations() {
    auto db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    return stationDao.selectAll(db);
}

//获取单个站点
std::unique_ptr<Stationlocal> StationService::getStationDetail(Station stationId) {
    auto db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return nullptr;
    return stationDao.selectById(db, stationId);
}