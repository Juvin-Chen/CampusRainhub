/*
定义了用户类User，用于管理用户信息和借还雨具的操作。
*/
#pragma once

#include <QString>
#include <memory>
#include "RainGear.h" 

class User {
public:
    User(QString id, QString name, double credit, int role);
    ~User() = default;

    //getters
    const QString& get_id() const;
    const QString& get_name() const;
    double get_credit() const;
    int get_role() const;
    const RainGear* get_current_gear() const;

    //业务逻辑接口
    
    //检查能不能借雨具
    bool can_borrow(const RainGear* gear) const;

    //扣款,成功返回 true,余额不足返回 false
    bool deduct(double amount);

    //充值/退还押金
    void recharge(double amount);

    void take_gear(RainGear* gear);  //借伞
    bool has_gear() const; //持有伞
    std::unique_ptr<RainGear> return_gear(); //还伞

private:
    QString id;
    QString name;
    double credit;  //一卡通余额
    int role;       //0:学生, 1:教职工, 9:管理员
    std::unique_ptr<RainGear> current_gear; 
};