/*
数据库连接池类，用于管理数据库连接，避免频繁创建和销毁连接，提高性能。
这里用的是单例模式，也就是只有一个实例，所有的线程都共享这个实例。
 */

#pragma once

#include <QSqlDatabase>

class ConnectionPool{
    public:
        static QSqlDatabase getThreadLocalConnection(); //获取线程本地连接
        static void removeThreadConnection();  //移除线程本地连接
};