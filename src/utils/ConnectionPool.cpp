#include<QSqlDatabase>
#include<QSqlQuery>
#include<QDebug>
#include<QString>
#include<QSqlError>
#include<QThread>
#include<QDateTime>

#include "ConnectionPool.h"

QSqlDatabase ConnectionPool::getThreadLocalConnection(){
    QString connectionName=QString("Conn_%1").arg((quint64)QThread::currentThreadId());
    if(QSqlDatabase::contains(connectionName)){
        return QSqlDatabase::database(connectionName);
    }else{
        QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL",connectionName);
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("rainhub_db"); //数据库名
        db.setUserName("root"); //用户名
        db.setPassword("root"); //密码
        if(!db.open()){
            qCritical()<<"Failed to connect to database: "<<db.lastError().text();
        }else{
            qInfo()<<"Connected to database: "<<db.databaseName();
            // 设置数据库连接时区为本地时区，确保时间存储和读取时不做时区转换
            QSqlQuery timezoneQuery(db);
            // 获取系统时区偏移（小时）
            QDateTime localTime = QDateTime::currentDateTime();
            QDateTime utcTime = localTime.toUTC();
            int offsetSeconds = localTime.secsTo(utcTime);
            int offsetHours = offsetSeconds / 3600;
            QString timezoneStr = QString("+%1:00").arg(offsetHours, 2, 10, QChar('0'));
            if (offsetHours < 0) {
                timezoneStr = QString("%1:00").arg(offsetHours, 2, 10, QChar('0'));
            }
            // 设置 MySQL 会话时区
            if (!timezoneQuery.exec(QString("SET time_zone = '%1'").arg(timezoneStr))) {
                qWarning() << "设置数据库时区失败，可能影响时间计算:" << timezoneQuery.lastError().text();
            } else {
                qInfo() << "数据库时区已设置为:" << timezoneStr;
            }
        }
        return db;
    }
}

void ConnectionPool::removeThreadConnection(){
    QString connectionName=QString("Conn_%1").arg((quint64)QThread::currentThreadId());
    if(QSqlDatabase::contains(connectionName)){
        QSqlDatabase::removeDatabase(connectionName);
    }
}
