/*
  定义了用户类User
*/
#pragma once

#include <QString>
#include <memory>
#include "RainGear.hpp" 

class User {
public:
    User(QString id, QString name, QString password, int role, double credit, bool is_active);
    ~User() = default;
    // getters
    const QString& get_id() const;
    const QString& get_name() const;
    const QString& get_password() const;
    int get_role() const;
    double get_credit() const;
    bool get_is_active() const;
    
    // setters
    void set_credit(double credit);
    void set_is_active(bool is_active);
    void set_password(QString&& password);

private:
    QString id;
    QString name;
    QString password;
    int role;
    double credit;  // 一卡通余额
    bool is_active; // 是否已激活，0:未激活需首次设置密码, 1:已激活
};