#include "StationService.h"
#include "../utils/ConnectionPool.h"
#include <QDebug>

//获取所有站点
std::vector<std::unique_ptr<Stationlocal>> StationService::getAllStations() {
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

//获取各站点的可用库存数量
QMap<int, int> StationService::getStationInventoryCounts() {
    auto db = ConnectionPool::getThreadLocalConnection();
    if (!db.isOpen()) return {};
    return stationDao.selectStationInventoryCounts(db);
}