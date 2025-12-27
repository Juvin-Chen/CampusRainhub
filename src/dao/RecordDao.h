#pragma once
#include<QSqlDatabase>
#include<QString>
#include<optional>


#include"../Model/BorrowRecord.h"

class RecordDao {
public:
    //add借出记录
    bool addBorrowRecord(QSqlDatabase& db, const QString& userId, const QString& gearId);

    //查找未归还记录,这里需要返回 BorrowRecord 对象给 Service 层用来算钱
    std::optional<BorrowRecord> selectUnfinishedByUserId(QSqlDatabase& db, const QString& userId);

    //结单,更新归还时间与费用
    bool updateReturnInfo(QSqlDatabase& db, qint64 recordId, double cost);
};