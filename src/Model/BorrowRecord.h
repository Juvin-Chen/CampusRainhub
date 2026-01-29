#pragma once

#include<QString>
#include<QDateTime>

class BorrowRecord{
public:
    BorrowRecord(qint64 recordId, QString userId, QString gearId, QDateTime borrowTime, QDateTime returnTime, double cost):recordId(recordId), userId(userId), gearId(gearId), borrowTime(borrowTime), returnTime(returnTime), cost(cost) {}
    ~BorrowRecord() = default;

    // getters
    qint64 get_record_id() const { return recordId; }
    const QString& get_user_id() const { return userId; }
    const QString& get_gear_id() const { return gearId; }
    const QDateTime& get_borrow_time() const { return borrowTime; }
    const QDateTime& get_return_time() const { return returnTime; }
    double get_cost() const { return cost; }

private:
    qint64 recordId;        // 对应record_id
    QString userId;         // 对应user_id
    QString gearId;         // 对应gear_id
    QDateTime borrowTime;   // 对应borrow_time
    QDateTime returnTime;   // 对应return_time (如果没还的话可能为空/无效)
    double cost;            // 对应cost
};