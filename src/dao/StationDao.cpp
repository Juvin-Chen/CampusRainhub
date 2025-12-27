#include"StationDao.h"
#include"GearDao.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QStringList>


//select_all，查出所有站点包含的所有的雨具的完整信息
std::vector<std::unique_ptr<Stationlocal>> StationDao::selectAll(QSqlDatabase& db) {
    std::vector<std::unique_ptr<Stationlocal>> stationList;
    stationList.reserve(20);
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM station ORDER BY station_id"));

    if (!query.exec()) {
        qCritical() << "查询站点失败:" << query.lastError().text();
        return stationList;
    }

    //用于查询某个站点的所有雨具的信息，将雨具信息传给Stationlocal类
    GearDao gearDao;
    while (query.next()) {
        //从station表读基础数据信息
        int idInt=query.value("station_id").toInt();
        Station sid=static_cast<Station>(idInt);
        double x=query.value("pos_x").toDouble();
        double y=query.value("pos_y").toDouble();
        bool isOnline=(query.value("status").toInt()==1);
        QString badSlotsStr =query.value("unavailable_slots").toString();

        //创建Stationlocal对象
        auto stationObj=std::make_unique<Stationlocal>(sid, x, y);
        stationObj->set_online(isOnline);

        //解析故障槽位字符串，数据库中是以逗号分隔的字符串如"1,5"
        if (!badSlotsStr.isEmpty()) {
            QStringList slotList = badSlotsStr.split(',', Qt::SkipEmptyParts);
            for (const QString& s : slotList) {
                int slotIndex = s.toInt();
                //标记该槽位不可用
                stationObj->mark_unavailable(slotIndex);
            }
        }

        //调用GearDao查出该站点下的所有伞
        auto gears = gearDao.selectByStation(db, sid);
        for (size_t i = 0; i < gears.size(); ++i) {
            auto& gear = gears[i];
            if (gear) {
                int slotIdx = gear->get_slot_id();
                stationObj->add_gear(slotIdx, std::move(gear));
            }
        }

        stationList.push_back(std::move(stationObj));
    }

    return stationList;
}

//select_by_id，查出单个站点包含的所有的雨具的完整信息
std::unique_ptr<Stationlocal> StationDao::selectById(QSqlDatabase& db, Station stationId) {
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM station WHERE station_id = ?"));
    query.addBindValue(static_cast<int>(stationId));

    if (!query.exec()) {
        qCritical() << "查询站点失败:" << query.lastError().text();
        return nullptr;
    }

    if (query.next()) {
        //从station表读基础数据信息
        double x = query.value("pos_x").toDouble();
        double y = query.value("pos_y").toDouble();
        bool isOnline = (query.value("status").toInt() == 1);
        QString badSlotsStr = query.value("unavailable_slots").toString();
        //创建Stationlocal对象
        auto stationObj = std::make_unique<Stationlocal>(stationId, x, y);
        stationObj->set_online(isOnline);

        if (!badSlotsStr.isEmpty()) {
            QStringList slotList = badSlotsStr.split(',', Qt::SkipEmptyParts);
            for (const QString& s : slotList) {
                stationObj->mark_unavailable(s.toInt());
            }
        }

        //填充雨具
        GearDao gearDao;
        auto gears = gearDao.selectByStation(db, stationId);
        for (size_t i = 0; i < gears.size(); ++i) {
            auto& gear = gears[i];
            if (gear) {
                int slotIdx = gear->get_slot_id();
                stationObj->add_gear(slotIdx, std::move(gear));
            }
        }
        return stationObj;
    }

    return nullptr;
}