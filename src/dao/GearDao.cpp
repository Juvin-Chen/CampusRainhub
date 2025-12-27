#include"GearDao.h"
#include"../Model/RainGearFactory.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QVariant>

//select_by_id
std::unique_ptr<RainGear> GearDao::selectById(QSqlDatabase& db, const QString& id){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM raingear WHERE gear_id = ?"));
    query.addBindValue(id);
    if(!query.exec()){ return nullptr; }

    if(query.next()){
        QString gearId = query.value("gear_id").toString();
        GearType type = static_cast<GearType>(query.value("type_id").toInt());
        
        auto gear = RainGearFactory::create_raingear(type, gearId);
        
        if(gear){
            gear->set_status(static_cast<GearStatus>(query.value("status").toInt()));
            gear->set_station_id(static_cast<Station>(query.value("station_id").toInt()));
            gear->set_slot_id(query.value("slot_id").toInt());
        }
        return gear;
    }
    return nullptr;
}

//select_by_station
std::vector<std::unique_ptr<RainGear>> GearDao::selectByStation(QSqlDatabase& db, Station station){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT * FROM raingear WHERE station_id = ?"));
    query.addBindValue(static_cast<int>(station));
    if(!query.exec()){ return std::vector<std::unique_ptr<RainGear>>(); }

    std::vector<std::unique_ptr<RainGear>> gears;
    gears.reserve(16);
    while(query.next()){
        QString gearId = query.value("gear_id").toString();
        GearType type = static_cast<GearType>(query.value("type_id").toInt());

        auto gear = RainGearFactory::create_raingear(type, gearId);
        
        if (gear) {
            gear->set_status(static_cast<GearStatus>(query.value("status").toInt()));
            gear->set_station_id(static_cast<Station>(query.value("station_id").toInt()));
            gear->set_slot_id(query.value("slot_id").toInt());            
            gears.push_back(std::move(gear));
        }
    }
    return gears;
}

//check_slot_occupied
bool GearDao::isSlotOccupied(QSqlDatabase& db, Station station, int slot_id){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT count(*) FROM raingear WHERE station_id = ? AND slot_id = ?"));
    query.addBindValue(static_cast<int>(station));
    query.addBindValue(slot_id);
    
    if(!query.exec()){ return false; }
    if(query.next()){
        return query.value(0).toInt() > 0;
    }
    return false;
}

//insert
bool GearDao::insert(QSqlDatabase& db, const QString& gearId, GearType type, Station stationId, int slotId){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("INSERT INTO raingear (gear_id, type_id, station_id, slot_id, status) VALUES (?, ?, ?, ?, 1)"));
    query.addBindValue(gearId);
    query.addBindValue(static_cast<int>(type));
    query.addBindValue(static_cast<int>(stationId));
    query.addBindValue(slotId);
    return query.exec();
}

//delete_by_id
bool GearDao::deleteById(QSqlDatabase& db, const QString& id){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("DELETE FROM raingear WHERE gear_id = ?"));
    query.addBindValue(id);
    return query.exec();
}

//update_status_and_location
bool GearDao::updateStatusAndLocation(QSqlDatabase& db, const QString& id, GearStatus status, Station station, int slot_id){
    QSqlQuery query(db);
    query.prepare(QStringLiteral("UPDATE raingear SET status = ?, station_id = ?, slot_id = ? WHERE gear_id = ?"));
    query.addBindValue(static_cast<int>(status));
    
    query.addBindValue(static_cast<int>(station)); 
    query.addBindValue(slot_id);
    query.addBindValue(id);
    
    return query.exec();
}