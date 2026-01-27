/*
    认证相关页面集合
    包含：刷卡页、用户输入页、首次登录页、登录页、修改密码页
*/
#pragma once

#include <QWidget>
#include <memory>
#include "../../Model/User.h"

class QLabel;
class QLineEdit;
class AuthService;

// 刷卡提示页
class CardReadPage : public QWidget {
    Q_OBJECT
public:
    explicit CardReadPage(QWidget *parent = nullptr);

signals:
    void confirmed();   // 确认进入手动输入
    void backClicked(); // 返回首页

private:
    void setupUi();
};

// 用户信息输入页
class UserInputPage : public QWidget {
    Q_OBJECT
public:
    explicit UserInputPage(AuthService *authService, QWidget *parent = nullptr);
    void clearInputs();  // 清空输入框

signals:
    void firstLogin(const QString &userId, const QString &userName);  // 首次登录
    void normalLogin(const QString &userId, const QString &userName); // 正常登录
    void backClicked();

private:
    void setupUi();
    void onSubmit();

    AuthService *m_authService;
    QLineEdit *m_inputUser;
    QLineEdit *m_inputName;
};

// 首次登录设置密码页
class FirstLoginPage : public QWidget {
    Q_OBJECT
public:
    explicit FirstLoginPage(AuthService *authService, QWidget *parent = nullptr);
    void setUserInfo(const QString &userId, const QString &userName);
    void clearInputs();

signals:
    void registerSuccess();  // 注册成功
    void backClicked();

private:
    void setupUi();
    void onSubmit();

    AuthService *m_authService;
    QString m_userId;
    QString m_userName;
    QLabel *m_userInfoLabel;
    QLineEdit *m_inputNewPass;
    QLineEdit *m_inputConfirmPass;
};

// 密码登录页
class LoginPage : public QWidget {
    Q_OBJECT
public:
    explicit LoginPage(AuthService *authService, QWidget *parent = nullptr);
    void setUserInfo(const QString &userId, const QString &userName);
    void clearInputs();

signals:
    void loginSuccess(std::shared_ptr<User> user);  // 登录成功
    void changePassword();                           // 修改密码
    void backClicked();

private:
    void setupUi();
    void onLogin();
    void onForgotPassword();  // 处理忘记密码点击
    void onChangePassword();  // 处理修改密码点击

    AuthService *m_authService;
    QString m_userId;
    QString m_userName;
    QLabel *m_userInfoLabel;
    QLineEdit *m_inputPass;
};

//修改密码页
class ResetPwdPage : public QWidget {
    Q_OBJECT
public:
    explicit ResetPwdPage(AuthService *authService, QWidget *parent = nullptr);
    void setUserId(const QString &userId, const QString &userName);
    void clearInputs();

signals:
    void resetSuccess();
    void backClicked();

private:
    void setupUi();
    void onSubmit();

    AuthService *m_authService;
    QString m_userId;
    QString m_userName;
    QLineEdit *m_inputOld;
    QLineEdit *m_inputNew;
    QLineEdit *m_inputConfirm;
};

