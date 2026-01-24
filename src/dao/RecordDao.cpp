#include "RecordDao.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

//add借出记录
bool RecordDao::addBorrowRecord(QSqlDatabase& db, const QString& userId, const QString& gearId) {
    QSqlQuery query(db);
    // 使用系统时间，格式化为字符串存储，完全避免时区问题
    QDateTime borrowTime = QDateTime::currentDateTime();
    QString borrowTimeStr = borrowTime.toString("yyyy-MM-dd hh:mm:ss");
    
    query.prepare(QStringLiteral("INSERT INTO record (user_id, gear_id, borrow_time, cost) VALUES (?, ?, STR_TO_DATE(?, '%Y-%m-%d %H:%i:%s'), 0.0)"));
    query.addBindValue(userId);
    query.addBindValue(gearId);
    query.addBindValue(borrowTimeStr);

    if (!query.exec()) {
        qCritical() << "插入借还记录失败:" << query.lastError().text();
        return false;
    }
    return true;
}

//根据ID查找借伞未归还的记录
std::optional<BorrowRecord> RecordDao::selectUnfinishedByUserId(QSqlDatabase& db, const QString& userId) {
    QSqlQuery query(db);
    //return_time IS NULL就是返回时间为空的就是未归还的
    query.prepare(QStringLiteral("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost FROM record WHERE user_id = ? AND return_time IS NULL LIMIT 1"));
    query.addBindValue(userId);

    if (!query.exec()) {
        qCritical() << "查询未归还记录失败:" << query.lastError().text();
        return std::nullopt;
    }
    if (query.next()) {
        // 从数据库读取时间，使用字符串格式读取，完全避免时区问题
        QString borrowTimeStr = query.value("borrow_time").toString();
        QDateTime borrowTime = QDateTime::fromString(borrowTimeStr, "yyyy-MM-dd hh:mm:ss");
        
        // 如果读取的时间无效，报错
        if (!borrowTime.isValid()) {
            qCritical() << "从数据库读取的借出时间无效！字符串:" << borrowTimeStr;
            return std::nullopt;
        }
        
        // 确保是本地时间（fromString 默认就是本地时间，但明确转换一下更安全）
        borrowTime = borrowTime.toLocalTime();
        
        QDateTime returnTime;
        QString returnTimeStr = query.value("return_time").toString();
        if (!returnTimeStr.isEmpty()) {
            returnTime = QDateTime::fromString(returnTimeStr, "yyyy-MM-dd hh:mm:ss");
            if (returnTime.isValid()) {
                returnTime = returnTime.toLocalTime();
            }
        }
        return BorrowRecord(query.value("record_id").toLongLong(), query.value("user_id").toString(), query.value("gear_id").toString(), borrowTime, returnTime, query.value("cost").toDouble());
    }
    return std::nullopt;
}

//更新还伞结账信息,这里传入record_id作为参数
bool RecordDao::updateReturnInfo(QSqlDatabase& db, qint64 recordId, const QDateTime& returnTime, double cost) {
    QSqlQuery query(db);
    // 使用字符串格式存储，完全避免时区问题
    QString returnTimeStr = returnTime.toString("yyyy-MM-dd hh:mm:ss");
    //更新return_time为传入的时间，写入费用（确保与计费时使用的时间一致）
    query.prepare(QStringLiteral("UPDATE record SET return_time = STR_TO_DATE(?, '%Y-%m-%d %H:%i:%s'), cost = ? WHERE record_id = ?"));
    query.addBindValue(returnTimeStr);
    query.addBindValue(cost);
    query.addBindValue(recordId);

    if (!query.exec()) {
        qCritical() << "更新还伞记录失败:" << query.lastError().text();
        return false;
    }
    
    // 验证更新是否成功
    if (query.numRowsAffected() == 0) {
        qWarning() << "更新还伞记录：未找到对应的记录ID" << recordId;
        return false;
    }
    
    return true;
}



//管理员后台Part
//获取最近订单
QVector<OrderInfoDTO> RecordDao::selectRecent(QSqlDatabase& db, int limit) {
    QVector<OrderInfoDTO> result;
    QSqlQuery query(db);
    query.prepare(QString("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost FROM record ORDER BY borrow_time DESC LIMIT %1").arg(limit));
    
    if (!query.exec()) { return result; }
    
    while (query.next()) {
        QString returnTime = query.value("return_time").isNull() ? QString() 
            : query.value("return_time").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        result.append(OrderInfoDTO{
            query.value("record_id").toLongLong(), 
            query.value("user_id").toString(), 
            query.value("gear_id").toString(), 
            query.value("borrow_time").toDateTime().toString("yyyy-MM-dd hh:mm:ss"), 
            returnTime, 
            query.value("cost").toDouble()
        });
    }
    return result;
}