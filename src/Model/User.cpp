#include"User.h"

#include<QString>

User::User(QString id, QString name, QString password, int role, double credit, bool is_active):
    id(id), name(name), password(password), role(role), credit(credit), is_active(is_active){}

// getters
const QString& User::get_id() const{ return id; }
const QString& User::get_name() const{ return name; }
const QString& User::get_password() const{ return password; }   
int User::get_role() const{ return role; }
double User::get_credit() const{ return credit; }
bool User::get_is_active() const{ return is_active; }

// setters
void User::set_credit(double credit) { this->credit = credit; }
void User::set_is_active(bool is_active) { this->is_active = is_active; }
void User::set_password(QString&& password) { this->password = std::move(password); }