/*
数据库连接池类，用于管理数据库连接，避免频繁创建和销毁连接，提高性能。
其实就是一个静态工具类
 */

#pragma once

#include <QSqlDatabase>

class ConnectionPool{
    public:
        static QSqlDatabase getThreadLocalConnection(); //获取线程本地连接
        static void removeThreadConnection();  //移除线程本地连接
};