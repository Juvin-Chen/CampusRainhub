#include"UserDao.h"

#include<QsqlQuery>
#include<QSqlError>
#include<QDebug>
#include<QVariant>

//select_by_id
std::optional<User> UserDao::selectById(QsqlDatabase& db, const QString& id){
    QsqlQuery query(db);
    query.prepare(QStringLiteral("SELECT user_id, real_name, credit, role, is_active, password "
    "FROM users WHERE user_id = :uid LIMIT 1")); //查到一个就不再继续往下查了，id是唯一的
    query.bindValue(":uid", id); //绑定参数，避免sql注入
    if(!query.exec()){
        qWarning() << "[UserDao::selectById] Error: " << query.lastError().text();
        return std::nullopt;
    }
    if(query.next()){
        return User(query.value("user_id").toString(), query.value("real_name").toString(), query.value("credit").toDouble(), query.value("role").toString(), query.value("is_active").toBool(), query.value("password").toString());
    }
    return std::nullopt;
}

//select_by_id_and_name
std::optional<User> UserDao::selectByIdAndName(QsqlDatabase& db, const QString& id, const QString& name){
    QsqlQuery query(db);
    query.prepare(QStringLiteral("SELECT user_id, real_name, credit, role, is_active, password "
    "FROM users WHERE user_id = :uid AND real_name = :name LIMIT 1"));
    query.bindValue(":uid", id);
    query.bindValue(":name", name);
    if(!query.exec()){
        qWarning() << "[UserDao::selectByIdAndName] Error: " << query.lastError().text();
        return std::nullopt;
    }
    if(query.next()){
        return User(query.value("user_id").toString(), query.value("real_name").toString(), query.value("credit").toDouble(), query.value("role").toString(), query.value("is_active").toBool(), query.value("password").toString());
    }
    return std::nullopt;
}

//update_password