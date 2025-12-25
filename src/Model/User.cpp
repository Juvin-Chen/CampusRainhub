#include "User.h"

// 构造函数使用 std::move 优化字符串拷贝
User::User(QString id, QString name, double credit, int role):id(std::move(id)),name(std::move(name)),credit(credit),role(role){}

// Getters 实现
const QString& User::get_id() const { return id; }
const QString& User::get_name() const { return name; }
double User::get_credit() const { return credit; }
int User::get_role() const { return role; }
const RainGear* User::get_current_gear() const { return current_gear.get(); } //unique需要调用get()转换为指针

//检查是否可借
bool User::can_borrow(const RainGear* gear) const {
    if (!gear) return false; 
    return credit >= gear->get_deposit();
}

//扣款
bool User::deduct(double amount) {
    if(credit>=amount){
        credit-=amount;
        return true;
    }
    return false;
}

//充值/押金退还
void User::recharge(double amount) {
    credit+=amount;
}

//借伞实现
void User::take_gear(RainGear* gear) {
    current_gear.reset(gear);  //unique_ptr不能直接赋值原始指针，需用reset()接管这个原始指针的所有权
}

//是否持有伞
bool User::has_gear() const {
    return current_gear!=nullptr;
}

//还伞实现,并不是将伞销毁，而是把伞还给系统
std::unique_ptr<RainGear> User::return_gear(){
    if(!current_gear) return nullptr;
    return std::move(current_gear); //move后原unique_ptr自动置空
}