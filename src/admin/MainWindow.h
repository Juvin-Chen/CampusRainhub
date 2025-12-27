/*
    管理员后台主窗口
*/
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>

class QStackedWidget;
class QWidget;
class QPushButton;
class QLabel;
class QLineEdit;
class QComboBox;
class QTableWidget;

class AuthService;
class AdminService;
class User;

class AdminMainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AdminMainWindow(QWidget *parent = nullptr);
    ~AdminMainWindow() override;

private slots:
    void onRefreshTimer();

private:
    enum class Page {
        Login = 0,
        Dashboard,
        GearManage,
        UserManage,
        OrderManage
    };

    void setupUi();
    QWidget* createLoginPage();
    QWidget* createDashboardPage();
    QWidget* createGearManagePage();
    QWidget* createUserManagePage();
    QWidget* createOrderManagePage();
    
    void switchPage(Page page);
    void handleLogout();
    
    // 数据刷新
    void refreshDashboardData();
    void refreshGearManageData();
    void refreshUserManageData();
    void refreshOrderManageData();
    
    // 天气信息（模拟）
    QString getWeatherInfo() const;

    // 服务层
    std::unique_ptr<AuthService> m_authService;
    std::unique_ptr<AdminService> m_adminService;
    
    // 当前管理员
    std::shared_ptr<User> m_currentAdmin;

    QStackedWidget *m_stack { nullptr };
    QTimer *m_refreshTimer { nullptr };
    
    // 登录页面
    QLineEdit *m_loginUserIdInput { nullptr };
    QLineEdit *m_loginPasswordInput { nullptr };
    
    // Dashboard页面
    QLabel *m_adminLabel { nullptr };
    QLabel *m_weatherLabel { nullptr };
    QLabel *m_onlineDevicesLabel { nullptr };
    QLabel *m_borrowedGearsLabel { nullptr };
    QLabel *m_faultCountLabel { nullptr };
    QTableWidget *m_stationTable { nullptr };
    
    // 雨具管理页面
    QComboBox *m_gearStationCombo { nullptr };
    QComboBox *m_gearSlotCombo { nullptr };
    QTableWidget *m_gearTable { nullptr };
    
    // 用户管理页面
    QLineEdit *m_userSearchInput { nullptr };
    QTableWidget *m_userTable { nullptr };
    
    // 订单管理页面
    QTableWidget *m_orderTable { nullptr };
};

