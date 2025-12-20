#include "AdminMainWindow.h"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDialog>
#include <QDialogButtonBox>
#include <QBrush>
#include <QColor>
#include <QAbstractItemView>
#include <QSqlQuery>
#include "Control/DBHelper.h"

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Login);
    setWindowTitle("RainHub 管理员后台");
    resize(900, 700);  // 与客户端相同的尺寸
    
    // 创建定时刷新器（每3秒刷新一次，实现实时同步）
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &AdminMainWindow::onRefreshTimer);
}

void AdminMainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createLoginPage());         // 0 Login
    m_stack->addWidget(createDashboardPage());     // 1 Dashboard
    m_stack->addWidget(createGearManagePage());    // 2 GearManage
    m_stack->addWidget(createUserManagePage());    // 3 UserManage
    m_stack->addWidget(createOrderManagePage());   // 4 OrderManage
    m_stack->addWidget(createSystemSettingsPage()); // 5 SystemSettings

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QWidget* AdminMainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("管理员后台登录"), page);
    title->setStyleSheet("font-size:28px; font-weight:700; color: #2c3e50;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("请输入管理员账号和密码"), page);
    subtitle->setStyleSheet("font-size:16px; color: #7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    auto *inputUserId = new QLineEdit(page);
    auto *inputPassword = new QLineEdit(page);
    inputUserId->setPlaceholderText(tr("请输入管理员账号"));
    inputPassword->setPlaceholderText(tr("请输入密码"));
    inputPassword->setEchoMode(QLineEdit::Password);
    inputUserId->setFixedWidth(300);
    inputPassword->setFixedWidth(300);
    
    form->addRow(tr("管理员账号："), inputUserId);
    form->addRow(tr("密码："), inputPassword);

    auto *btnLogin = new QPushButton(tr("登录"), page);
    btnLogin->setFixedWidth(160);
    btnLogin->setStyleSheet("font-size:16px; padding:10px; background-color: #3498db; color: white;");
    connect(btnLogin, &QPushButton::clicked, this, [this, inputUserId, inputPassword] {
        const QString userId = inputUserId->text().trimmed();
        const QString password = inputPassword->text();

        if (userId.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入管理员账号和密码"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到数据库，请检查服务是否已启动。"));
            return;
        }

        if (performAdminLogin(userId, password)) {
            inputPassword->clear();
            switchPage(Page::Dashboard);
            m_refreshTimer->start(3000); // 开始定时刷新（3秒一次）
        }
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addWidget(btnLogin, 0, Qt::AlignCenter);
    return page;
}

bool AdminMainWindow::performAdminLogin(const QString &userId, const QString &password)
{
    // 查询管理员账号（role=9）
    auto record = DatabaseManager::fetchUserByIdAndNameAndPassword(userId, userId, password);
    if (!record) {
        QMessageBox::warning(this, tr("登录失败"), tr("账号或密码错误"));
        return false;
    }
    
    if (record->role != 9) {
        QMessageBox::warning(this, tr("权限不足"), tr("该账号不是管理员账号"));
        return false;
    }
    
    m_currentAdminId = record->userId;
    m_currentAdminName = record->realName;
    
    QMessageBox::information(this, tr("登录成功"), tr("欢迎，%1").arg(m_currentAdminName));
    return true;
}

QWidget* AdminMainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏
    auto *sidebar = new QWidget(page);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet(
        "QWidget { background-color: #34495e; }"
        "QPushButton { "
        "  text-align: left; "
        "  padding: 15px 20px; "
        "  font-size: 14px; "
        "  color: white; "
        "  border: none; "
        "  background-color: transparent; "
        "}"
        "QPushButton:hover { background-color: #2c3e50; }"
        "QPushButton:pressed { background-color: #1a252f; }"
    );
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    // 导航按钮
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    m_navGearManage = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    connect(m_navGearManage, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    m_navUserManage = new QPushButton(tr("👤 用户管理"), sidebar);
    connect(m_navUserManage, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    m_navOrderManage = new QPushButton(tr("📋 订单/流水"), sidebar);
    connect(m_navOrderManage, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    m_navSystemSettings = new QPushButton(tr("⚙️ 系统设置"), sidebar);
    connect(m_navSystemSettings, &QPushButton::clicked, this, [this] { switchPage(Page::SystemSettings); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(m_navGearManage);
    sidebarLayout->addWidget(m_navUserManage);
    sidebarLayout->addWidget(m_navOrderManage);
    sidebarLayout->addWidget(m_navSystemSettings);
    sidebarLayout->addStretch();
    
    // 右侧主内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    // 顶部栏：标题、天气、管理员信息
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("管理员后台"), contentArea);
    title->setStyleSheet("font-size:24px; font-weight:700; color: #2c3e50;");
    
    m_weatherLabel = new QLabel(getWeatherInfo(), contentArea);
    m_weatherLabel->setStyleSheet("font-size:14px; color: #7f8c8d; padding: 8px 16px; background-color: #ecf0f1; border-radius: 4px;");
    
    auto *adminLabel = new QLabel(tr("管理员：%1").arg(m_currentAdminName), contentArea);
    adminLabel->setStyleSheet("font-size:14px; color: #e74c3c; font-weight:600;");
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(m_weatherLabel);
    topBar->addSpacing(10);
    topBar->addWidget(adminLabel);
    contentLayout->addLayout(topBar);

    // 统计信息栏
    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(12);
    
    m_onlineDevicesLabel = new QLabel(tr("设备在线: 计算中..."), contentArea);
    m_borrowedGearsLabel = new QLabel(tr("雨具借出: 计算中..."), contentArea);
    m_faultCountLabel = new QLabel(tr("待处理故障: 计算中..."), contentArea);
    
    QString statsStyle = "font-size:14px; padding: 12px 20px; background-color: #ecf0f1; border-radius: 6px; font-weight:600;";
    m_onlineDevicesLabel->setStyleSheet(statsStyle);
    m_borrowedGearsLabel->setStyleSheet(statsStyle);
    m_faultCountLabel->setStyleSheet(statsStyle);
    
    statsLayout->addWidget(m_onlineDevicesLabel);
    statsLayout->addWidget(m_borrowedGearsLabel);
    statsLayout->addWidget(m_faultCountLabel);
    statsLayout->addStretch();
    contentLayout->addLayout(statsLayout);

    // 地图区域
    auto *mapContainer = new QWidget(contentArea);
    mapContainer->setMinimumHeight(500);
    mapContainer->setStyleSheet("background-color: #ecf0f1; border: 2px solid #bdc3c7; border-radius: 8px;");
    loadMapStations(mapContainer);
    contentLayout->addWidget(mapContainer, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);
    
    // 初始化数据
    refreshDashboardData();
    
    return page;
}

QString AdminMainWindow::getWeatherInfo() const
{
    // 模拟天气信息（实际可以接入天气API）
    QDateTime now = QDateTime::currentDateTime();
    int hour = now.time().hour();
    QString weather;
    if (hour >= 6 && hour < 12) {
        weather = tr("天气: 小雨 18°C");
    } else if (hour >= 12 && hour < 18) {
        weather = tr("天气: 多云 22°C");
    } else {
        weather = tr("天气: 小雨 16°C");
    }
    return weather;
}

void AdminMainWindow::loadMapStations(QWidget *mapContainer)
{
    if (!mapContainer) return;
    
    if (!DatabaseManager::init()) {
        qWarning() << "[Admin] 数据库连接失败，无法加载站点";
        return;
    }
    
    auto allStations = DatabaseManager::fetchAllStations();
    
    // 查询所有站点的库存信息
    QMap<int, int> stationInventory;
    for (const auto &station : allStations) {
        auto gears = DatabaseManager::fetchGearsByStation(station.stationId);
        int availableCount = 0;
        for (const auto &gear : gears) {
            if (gear.status == 1) {
                availableCount++;
            }
        }
        stationInventory[station.stationId] = availableCount;
    }
    
    // 绘制站点
    for (const auto &station : allStations) {
        int stationId = station.stationId;
        QString name = station.name;
        double posX = station.posX;
        double posY = station.posY;
        
        // 创建站点按钮
        auto *stationBtn = new QPushButton(mapContainer);
        stationBtn->setFixedSize(30, 30);
        stationBtn->setCursor(Qt::PointingHandCursor);
        
        int availableCount = stationInventory.value(stationId, 0);
        QString color = "#2ecc71"; // 默认绿色
        if (availableCount < 2) {
            color = "#e74c3c"; // 红色
        } else if (availableCount < 5) {
            color = "#f1c40f"; // 黄色
        }
        
        stationBtn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1;"
            "  border: 2px solid white;"
            "  border-radius: 15px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %1;"
            "  border: 3px solid #3498db;"
            "  border-radius: 15px;"
            "}"
        ).arg(color));
        
        // 设置工具提示
        stationBtn->setToolTip(QString("%1\n可借雨具：%2 把").arg(name).arg(availableCount));
        
        // 点击站点显示详细信息
        connect(stationBtn, &QPushButton::clicked, this, [this, stationId, name, availableCount]() {
            onStationClicked(stationId);
        });
        
        // 添加站点名称标签
        auto *nameLabel = new QLabel(name, mapContainer);
        nameLabel->setStyleSheet("font-size: 11px; font-weight: 600; color: #2c3e50; background-color: rgba(255, 255, 255, 200); padding: 2px 6px; border-radius: 3px;");
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->adjustSize();
        
        // 延迟设置位置
        QTimer::singleShot(100, this, [mapContainer, stationBtn, nameLabel, posX, posY]() {
            int containerWidth = mapContainer->width();
            int containerHeight = mapContainer->height();
            
            int x = static_cast<int>(containerWidth * posX) - 15;
            int y = static_cast<int>(containerHeight * posY) - 15;
            
            stationBtn->move(x, y);
            nameLabel->move(x - nameLabel->width() / 2 + 15, y + 32);
        });
    }
}

void AdminMainWindow::onStationClicked(int stationId)
{
    auto gears = DatabaseManager::fetchGearsByStation(stationId);
    auto stations = DatabaseManager::fetchAllStations();
    QString stationName;
    for (const auto &s : stations) {
        if (s.stationId == stationId) {
            stationName = s.name;
            break;
        }
    }
    
    int totalCount = gears.size();
    int availableCount = 0;
    int borrowedCount = 0;
    int brokenCount = 0;
    
    for (const auto &gear : gears) {
        if (gear.status == 1) availableCount++;
        else if (gear.status == 2) borrowedCount++;
        else if (gear.status == 3) brokenCount++;
    }
    
    QString msg = QString("<h3>%1 (站点ID: %2)</h3>"
        "<p><b>总雨具数：</b>%3 把</p>"
        "<p><b>可借：</b>%4 把</p>"
        "<p><b>已借出：</b>%5 把</p>"
        "<p><b>故障：</b>%6 把</p>")
        .arg(stationName).arg(stationId).arg(totalCount)
        .arg(availableCount).arg(borrowedCount).arg(brokenCount);
    
    QMessageBox::information(this, tr("站点详情"), msg);
}

void AdminMainWindow::refreshDashboardData()
{
    if (!DatabaseManager::init()) return;
    
    // 更新天气
    if (m_weatherLabel) {
        m_weatherLabel->setText(getWeatherInfo());
    }
    
    // 计算设备在线率
    auto stations = DatabaseManager::fetchAllStations();
    int onlineCount = 0;
    for (const auto &s : stations) {
        if (s.status == 1) onlineCount++;
    }
    double onlineRate = stations.isEmpty() ? 0 : (onlineCount * 100.0 / stations.size());
    if (m_onlineDevicesLabel) {
        m_onlineDevicesLabel->setText(tr("设备在线: %1%").arg(QString::number(onlineRate, 'f', 0)));
    }
    
    // 计算借出雨具数
    int borrowedCount = 0;
    for (const auto &s : stations) {
        auto gears = DatabaseManager::fetchGearsByStation(s.stationId);
        for (const auto &gear : gears) {
            if (gear.status == 2) borrowedCount++;
        }
    }
    if (m_borrowedGearsLabel) {
        m_borrowedGearsLabel->setText(tr("雨具借出: %1把").arg(borrowedCount));
    }
    
    // 计算故障数
    int faultCount = 0;
    for (const auto &s : stations) {
        auto gears = DatabaseManager::fetchGearsByStation(s.stationId);
        for (const auto &gear : gears) {
            if (gear.status == 3) faultCount++;
        }
    }
    if (m_faultCountLabel) {
        m_faultCountLabel->setText(tr("待处理故障: %1").arg(faultCount));
    }
}

void AdminMainWindow::onRefreshTimer()
{
    // 定时刷新各个页面的数据（实现实时同步）
    int currentPage = m_stack->currentIndex();
    if (currentPage == 1) { // Dashboard
        refreshDashboardData();
    } else if (currentPage == 2) { // GearManage
        refreshGearManageData();
    } else if (currentPage == 3) { // UserManage
        refreshUserManageData();
    } else if (currentPage == 4) { // OrderManage
        refreshOrderManageData();
    }
}

void AdminMainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    
    // 更新导航栏按钮样式
    QString normalStyle = "QPushButton { background-color: transparent; }";
    QString selectedStyle = "QPushButton { background-color: #2c3e50; }";
    
    if (m_navGearManage) m_navGearManage->setStyleSheet(normalStyle);
    if (m_navUserManage) m_navUserManage->setStyleSheet(normalStyle);
    if (m_navOrderManage) m_navOrderManage->setStyleSheet(normalStyle);
    if (m_navSystemSettings) m_navSystemSettings->setStyleSheet(normalStyle);
    
    switch (page) {
        case Page::GearManage:
            if (m_navGearManage) m_navGearManage->setStyleSheet(selectedStyle);
            refreshGearManageData();
            break;
        case Page::UserManage:
            if (m_navUserManage) m_navUserManage->setStyleSheet(selectedStyle);
            refreshUserManageData();
            break;
        case Page::OrderManage:
            if (m_navOrderManage) m_navOrderManage->setStyleSheet(selectedStyle);
            refreshOrderManageData();
            break;
        case Page::SystemSettings:
            if (m_navSystemSettings) m_navSystemSettings->setStyleSheet(selectedStyle);
            break;
        case Page::Dashboard:
            refreshDashboardData();
            break;
        default:
            break;
    }
}

QWidget* AdminMainWindow::createGearManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏（复用）
    auto *sidebar = new QWidget(page);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("QWidget { background-color: #34495e; }");
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    m_navGearManage = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    m_navGearManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navGearManage, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    m_navUserManage = new QPushButton(tr("👤 用户管理"), sidebar);
    m_navUserManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navUserManage, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    m_navOrderManage = new QPushButton(tr("📋 订单/流水"), sidebar);
    m_navOrderManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navOrderManage, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    m_navSystemSettings = new QPushButton(tr("⚙️ 系统设置"), sidebar);
    m_navSystemSettings->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navSystemSettings, &QPushButton::clicked, this, [this] { switchPage(Page::SystemSettings); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(m_navGearManage);
    sidebarLayout->addWidget(m_navUserManage);
    sidebarLayout->addWidget(m_navOrderManage);
    sidebarLayout->addWidget(m_navSystemSettings);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("雨具管理"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 筛选区域
    auto *filterLayout = new QHBoxLayout();
    auto *stationLabel = new QLabel(tr("选择站点："), contentArea);
    m_gearStationCombo = new QComboBox(contentArea);
    m_gearStationCombo->addItem(tr("-- 全部站点 --"), 0);
    if (DatabaseManager::init()) {
        auto stations = DatabaseManager::fetchAllStations();
        for (const auto &station : stations) {
            m_gearStationCombo->addItem(station.name, station.stationId);
        }
    }
    m_gearStationCombo->setFixedWidth(200);
    
    auto *slotLabel = new QLabel(tr("选择槽位："), contentArea);
    m_gearSlotCombo = new QComboBox(contentArea);
    m_gearSlotCombo->addItem(tr("-- 全部槽位 --"), 0);
    for (int i = 1; i <= 12; ++i) {
        m_gearSlotCombo->addItem(QStringLiteral("#%1").arg(i), i);
    }
    m_gearSlotCombo->setFixedWidth(150);
    
    auto *btnRefresh = new QPushButton(tr("刷新"), contentArea);
    btnRefresh->setFixedWidth(100);
    connect(btnRefresh, &QPushButton::clicked, this, [this] { refreshGearManageData(); });
    connect(m_gearStationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, [this] { refreshGearManageData(); });
    
    filterLayout->addWidget(stationLabel);
    filterLayout->addWidget(m_gearStationCombo);
    filterLayout->addWidget(slotLabel);
    filterLayout->addWidget(m_gearSlotCombo);
    filterLayout->addWidget(btnRefresh);
    filterLayout->addStretch();

    // 雨具表格
    m_gearTable = new QTableWidget(contentArea);
    m_gearTable->setColumnCount(6);
    m_gearTable->setHorizontalHeaderLabels({
        tr("雨具ID"), tr("类型"), tr("站点"), tr("槽位"), tr("状态"), tr("操作")
    });
    m_gearTable->horizontalHeader()->setStretchLastSection(true);
    m_gearTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_gearTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addLayout(filterLayout);
    contentLayout->addWidget(m_gearTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshGearManageData()
{
    if (!m_gearTable || !DatabaseManager::init()) return;
    
    m_gearTable->setRowCount(0);
    
    int selectedStationId = m_gearStationCombo ? m_gearStationCombo->currentData().toInt() : 0;
    int selectedSlotId = m_gearSlotCombo ? m_gearSlotCombo->currentData().toInt() : 0;
    
    QVector<DatabaseManager::GearRecord> allGears;
    
    if (selectedStationId > 0) {
        allGears = DatabaseManager::fetchGearsByStation(selectedStationId);
    } else {
        // 查询所有站点的雨具
        auto stations = DatabaseManager::fetchAllStations();
        for (const auto &station : stations) {
            auto gears = DatabaseManager::fetchGearsByStation(station.stationId);
            allGears.append(gears);
        }
    }
    
    auto stations = DatabaseManager::fetchAllStations();
    QMap<int, QString> stationNames;
    for (const auto &s : stations) {
        stationNames[s.stationId] = s.name;
    }
    
    QStringList typeNames = {tr("未知"), tr("普通塑料伞"), tr("高质量抗风伞"), tr("专用遮阳伞"), tr("雨衣")};
    QStringList statusNames = {tr("未知"), tr("可借"), tr("已借出"), tr("故障")};
    
    for (const auto &gear : allGears) {
        if (selectedSlotId > 0 && gear.slotId != selectedSlotId) continue;
        
        int row = m_gearTable->rowCount();
        m_gearTable->insertRow(row);
        
        m_gearTable->setItem(row, 0, new QTableWidgetItem(gear.gearId));
        m_gearTable->setItem(row, 1, new QTableWidgetItem(
            gear.typeId >= 1 && gear.typeId <= 4 ? typeNames[gear.typeId] : tr("未知")));
        m_gearTable->setItem(row, 2, new QTableWidgetItem(
            stationNames.value(gear.stationId, tr("未知"))));
        m_gearTable->setItem(row, 3, new QTableWidgetItem(
            gear.slotId > 0 ? QStringLiteral("#%1").arg(gear.slotId) : tr("无")));
        
        auto *statusItem = new QTableWidgetItem(
            gear.status >= 1 && gear.status <= 3 ? statusNames[gear.status] : tr("未知"));
        if (gear.status == 1) {
            statusItem->setForeground(QBrush(QColor("#2ecc71"))); // 绿色
        } else if (gear.status == 2) {
            statusItem->setForeground(QBrush(QColor("#95a5a6"))); // 灰色
        } else if (gear.status == 3) {
            statusItem->setForeground(QBrush(QColor("#e74c3c"))); // 红色
        }
        m_gearTable->setItem(row, 4, statusItem);
        
        // 操作按钮
        auto *btnModify = new QPushButton(tr("修改状态"));
        btnModify->setStyleSheet("font-size:12px; padding:4px 8px;");
        connect(btnModify, &QPushButton::clicked, this, [this, gear]() {
            // 修改状态对话框
            QDialog dialog(this);
            dialog.setWindowTitle(tr("修改雨具状态"));
            auto *layout = new QVBoxLayout(&dialog);
            
            auto *label = new QLabel(tr("雨具ID: %1\n当前状态: %2")
                .arg(gear.gearId)
                .arg(gear.status == 1 ? tr("可借") : (gear.status == 2 ? tr("已借出") : tr("故障"))));
            layout->addWidget(label);
            
            auto *combo = new QComboBox(&dialog);
            combo->addItem(tr("可借"), 1);
            combo->addItem(tr("已借出"), 2);
            combo->addItem(tr("故障"), 3);
            combo->setCurrentIndex(gear.status - 1);
            layout->addWidget(combo);
            
            auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout->addWidget(btnBox);
            
            if (dialog.exec() == QDialog::Accepted) {
                int newStatus = combo->currentData().toInt();
                // 使用DatabaseManager更新状态
                if (DatabaseManager::updateGearStatus(gear.gearId, newStatus)) {
                    QMessageBox::information(this, tr("成功"), tr("雨具状态已更新"));
                    refreshGearManageData();
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("更新失败，请重试"));
                }
            }
        });
        
        m_gearTable->setCellWidget(row, 5, btnModify);
    }
    
    m_gearTable->resizeColumnsToContents();
}

QWidget* AdminMainWindow::createUserManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏（复用）
    auto *sidebar = new QWidget(page);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("QWidget { background-color: #34495e; }");
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    m_navGearManage = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    m_navGearManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navGearManage, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    m_navUserManage = new QPushButton(tr("👤 用户管理"), sidebar);
    m_navUserManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navUserManage, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    m_navOrderManage = new QPushButton(tr("📋 订单/流水"), sidebar);
    m_navOrderManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navOrderManage, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    m_navSystemSettings = new QPushButton(tr("⚙️ 系统设置"), sidebar);
    m_navSystemSettings->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navSystemSettings, &QPushButton::clicked, this, [this] { switchPage(Page::SystemSettings); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(m_navGearManage);
    sidebarLayout->addWidget(m_navUserManage);
    sidebarLayout->addWidget(m_navOrderManage);
    sidebarLayout->addWidget(m_navSystemSettings);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("用户管理"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 搜索区域
    auto *searchLayout = new QHBoxLayout();
    m_userSearchInput = new QLineEdit(contentArea);
    m_userSearchInput->setPlaceholderText(tr("输入学号/工号或姓名搜索"));
    m_userSearchInput->setFixedWidth(300);
    auto *btnSearch = new QPushButton(tr("搜索"), contentArea);
    btnSearch->setFixedWidth(100);
    connect(btnSearch, &QPushButton::clicked, this, [this] { refreshUserManageData(); });
    connect(m_userSearchInput, &QLineEdit::returnPressed, this, [this] { refreshUserManageData(); });
    
    searchLayout->addWidget(m_userSearchInput);
    searchLayout->addWidget(btnSearch);
    searchLayout->addStretch();

    // 用户表格
    m_userTable = new QTableWidget(contentArea);
    m_userTable->setColumnCount(6);
    m_userTable->setHorizontalHeaderLabels({
        tr("学号/工号"), tr("姓名"), tr("角色"), tr("余额"), tr("状态"), tr("操作")
    });
    m_userTable->horizontalHeader()->setStretchLastSection(true);
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addLayout(searchLayout);
    contentLayout->addWidget(m_userTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshUserManageData()
{
    if (!m_userTable || !DatabaseManager::init()) return;
    
    m_userTable->setRowCount(0);
    
    QString searchText = m_userSearchInput ? m_userSearchInput->text().trimmed() : QString();
    auto allUsers = DatabaseManager::fetchAllUsers();
    
    QStringList roleNames = {tr("学生"), tr("教职工"), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr("管理员")};
    
    for (const auto &user : allUsers) {
        // 搜索过滤
        if (!searchText.isEmpty()) {
            if (!user.userId.contains(searchText, Qt::CaseInsensitive) && 
                !user.realName.contains(searchText, Qt::CaseInsensitive)) {
                continue;
            }
        }
        
        int row = m_userTable->rowCount();
        m_userTable->insertRow(row);
        
        m_userTable->setItem(row, 0, new QTableWidgetItem(user.userId));
        m_userTable->setItem(row, 1, new QTableWidgetItem(user.realName));
        m_userTable->setItem(row, 2, new QTableWidgetItem(
            user.role >= 0 && user.role < roleNames.size() ? roleNames[user.role] : tr("未知")));
        m_userTable->setItem(row, 3, new QTableWidgetItem(
            QString::number(user.credit, 'f', 2) + tr(" 元")));
        m_userTable->setItem(row, 4, new QTableWidgetItem(
            user.isActive ? tr("已激活") : tr("未激活")));
        
        // 操作按钮
        auto *btnResetPwd = new QPushButton(tr("重置密码"));
        btnResetPwd->setStyleSheet("font-size:12px; padding:4px 8px;");
        connect(btnResetPwd, &QPushButton::clicked, this, [this, user]() {
            QDialog dialog(this);
            dialog.setWindowTitle(tr("重置密码"));
            auto *layout = new QVBoxLayout(&dialog);
            
            auto *label = new QLabel(tr("用户: %1 (%2)").arg(user.userId).arg(user.realName));
            layout->addWidget(label);
            
            auto *inputPwd = new QLineEdit(&dialog);
            inputPwd->setPlaceholderText(tr("请输入新密码"));
            inputPwd->setEchoMode(QLineEdit::Password);
            layout->addWidget(inputPwd);
            
            auto *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
            connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
            connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
            layout->addWidget(btnBox);
            
            if (dialog.exec() == QDialog::Accepted) {
                QString newPassword = inputPwd->text();
                if (newPassword.length() < 6) {
                    QMessageBox::warning(this, tr("提示"), tr("密码长度至少为6位"));
                    return;
                }
                if (DatabaseManager::resetUserPassword(user.userId, newPassword)) {
                    QMessageBox::information(this, tr("成功"), tr("密码已重置"));
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("重置失败，请重试"));
                }
            }
        });
        
        m_userTable->setCellWidget(row, 5, btnResetPwd);
    }
    
    m_userTable->resizeColumnsToContents();
}

QWidget* AdminMainWindow::createOrderManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏（复用）
    auto *sidebar = new QWidget(page);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("QWidget { background-color: #34495e; }");
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    m_navGearManage = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    m_navGearManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navGearManage, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    m_navUserManage = new QPushButton(tr("👤 用户管理"), sidebar);
    m_navUserManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navUserManage, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    m_navOrderManage = new QPushButton(tr("📋 订单/流水"), sidebar);
    m_navOrderManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navOrderManage, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    m_navSystemSettings = new QPushButton(tr("⚙️ 系统设置"), sidebar);
    m_navSystemSettings->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navSystemSettings, &QPushButton::clicked, this, [this] { switchPage(Page::SystemSettings); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(m_navGearManage);
    sidebarLayout->addWidget(m_navUserManage);
    sidebarLayout->addWidget(m_navOrderManage);
    sidebarLayout->addWidget(m_navSystemSettings);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("订单/流水"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    // 订单表格
    m_orderTable = new QTableWidget(contentArea);
    m_orderTable->setColumnCount(6);
    m_orderTable->setHorizontalHeaderLabels({
        tr("流水号"), tr("用户"), tr("雨具ID"), tr("借出时间"), tr("归还时间"), tr("费用")
    });
    m_orderTable->horizontalHeader()->setStretchLastSection(true);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addWidget(m_orderTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

void AdminMainWindow::refreshOrderManageData()
{
    if (!m_orderTable || !DatabaseManager::init()) return;
    
    m_orderTable->setRowCount(0);
    
    // 查询所有借还记录
    QSqlDatabase db = DBHelper::getThreadLocalConnection();
    if (!db.isOpen()) return;
    
    QSqlQuery query(db);
    query.prepare("SELECT record_id, user_id, gear_id, borrow_time, return_time, cost "
                  "FROM record ORDER BY borrow_time DESC LIMIT 100");
    
    if (!query.exec()) {
        qWarning() << "[Admin] 查询订单失败:" << query.lastError().text();
        return;
    }
    
    while (query.next()) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        
        m_orderTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        m_orderTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        m_orderTable->setItem(row, 4, new QTableWidgetItem(
            query.value(4).isNull() ? tr("未归还") : query.value(4).toString()));
        m_orderTable->setItem(row, 5, new QTableWidgetItem(
            QString::number(query.value(5).toDouble(), 'f', 2) + tr(" 元")));
    }
    
    m_orderTable->resizeColumnsToContents();
}

QWidget* AdminMainWindow::createSystemSettingsPage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航栏（复用）
    auto *sidebar = new QWidget(page);
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("QWidget { background-color: #34495e; }");
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);
    
    auto *btnDashboard = new QPushButton(tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    
    m_navGearManage = new QPushButton(tr("☂️ 雨具管理"), sidebar);
    m_navGearManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navGearManage, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    
    m_navUserManage = new QPushButton(tr("👤 用户管理"), sidebar);
    m_navUserManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navUserManage, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    
    m_navOrderManage = new QPushButton(tr("📋 订单/流水"), sidebar);
    m_navOrderManage->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: transparent; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navOrderManage, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });
    
    m_navSystemSettings = new QPushButton(tr("⚙️ 系统设置"), sidebar);
    m_navSystemSettings->setStyleSheet("QPushButton { text-align: left; padding: 15px 20px; font-size: 14px; color: white; border: none; background-color: #2c3e50; } QPushButton:hover { background-color: #2c3e50; }");
    connect(m_navSystemSettings, &QPushButton::clicked, this, [this] { switchPage(Page::SystemSettings); });
    
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(m_navGearManage);
    sidebarLayout->addWidget(m_navUserManage);
    sidebarLayout->addWidget(m_navOrderManage);
    sidebarLayout->addWidget(m_navSystemSettings);
    sidebarLayout->addStretch();

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("系统设置"), contentArea);
    title->setStyleSheet("font-size:20px; font-weight:700; color: #2c3e50;");

    auto *infoLabel = new QLabel(tr("系统设置功能待完善"), contentArea);
    infoLabel->setStyleSheet("font-size:14px; color: #7f8c8d;");
    contentLayout->addWidget(title);
    contentLayout->addWidget(infoLabel);
    contentLayout->addStretch();

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

