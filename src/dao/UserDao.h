#pragma once

#include<QsqlDatabase>
#include<QsqlQuery>
#include<QVector>
#include<optional>
#include<QString>

#include<../utils/ConnectionPool.h>
#include<../model/User.h>

class UserDao{
public:
    //根据ID查询用户
    std::optional<User> selectById(QsqlDatabase& db, const QString& id);
    //根据ID和姓名查询用户
    std::optional<User> selectByIdAndName(QsqlDatabase& db, const QString& id, const QString& name);
    //更新密码，（1）进行新用户的激活，（2）负责老用户的密码更改
    bool updatePassword(QsqlDatabase& db, const QString& id, const QString& name,const QString& newPassword);
    //更新余额，正负数都ok
    bool updateBalance(QsqlDatabase& db, const QString& id,double amountchange);
    //获取所有用户
    QVector<User> selectAll(QsqlDatabase& db);
};

