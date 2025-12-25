#include<QSqlDatabase>
#include<QDebug>
#include<QString>
#include<QSqlError>
#include<QThread>

#include "ConnectionPool.h"

QSqlDatabase ConnectionPool::getThreadLocalConnection(){
    QString connectionName=QString("Conn_%1").arg((quint64)QThread::currentThreadId());
    if(QSqlDatabase::contains(connectionName)){
        return QSqlDatabase::database(connectionName);
    }
    else{
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
