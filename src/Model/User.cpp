#include"User.h"

#include<QString>

User::User(QString id, QString password, QString name, int role, double credit, int avatar_id, bool is_active):
    id(id), password(password), name(name), role(role), credit(credit), avatar_id(avatar_id), is_active(is_active){
}

const QString& User::get_id() const{ return id; }
const QString& User::get_password() const{ return password; }   
const QString& User::get_name() const{ return name; }
int User::get_role() const{ return role; }
double User::get_credit() const{ return credit; }
int User::get_avatar_id() const{ return avatar_id; }
bool User::get_is_active() const{ return is_active; }