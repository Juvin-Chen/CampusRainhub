/*
定义了用户类User，用于管理用户信息和借还雨具的操作。
*/
#pragma once

#include <QString>
#include <memory>
#include "RainGear.hpp" 

class User {
public:
    User(QString id, QString password, QString name, int role, double credit, int avatar_id, bool is_active);
    ~User() = default;
    //getters
    const QString& get_id() const;
    const QString& get_password() const;
    const QString& get_name() const;
    int get_role() const;
    double get_credit() const;
    int get_avatar_id() const;
    bool get_is_active() const;
private:
    QString id;
    QString password;
    QString name;
    int role;
    double credit;  //一卡通余额
    int avatar_id; //头像资源索引
    bool is_active; //是否已激活，0:未激活需首次设置密码, 1:已激活
};