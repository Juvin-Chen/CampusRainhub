/*
    管理员后台主窗口实现
*/
#include "MainWindow.h"
#include "../client_ui/assets/Styles.h"
#include "../control/Admin_AuthService.h"
#include "../control/Admin_StationService.h"
#include "../control/Admin_GearService.h"
#include "../control/Admin_UserService.h"
#include "../control/Admin_OrderService.h"
#include "../Model/User.h"

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QDialog>
#include <QBrush>
#include <QColor>
#include <QAbstractItemView>

AdminMainWindow::AdminMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_authService(std::make_unique<Admin_AuthService>())
    , m_stationService(std::make_unique<Admin_StationService>())
    , m_gearService(std::make_unique<Admin_GearService>())
    , m_userService(std::make_unique<Admin_UserService>())
    , m_orderService(std::make_unique<Admin_OrderService>())
{
    qApp->setStyleSheet(Styles::globalStyle());
    
    setupUi();
    switchPage(Page::Login);
    setWindowTitle(tr("RainHub 管理员后台"));
    resize(1000, 700);
    
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &AdminMainWindow::onRefreshTimer);
}

AdminMainWindow::~AdminMainWindow() = default;

void AdminMainWindow::setupUi()
{
    auto *central = new QWidget(this);
    central->setObjectName("centralWidget");
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createLoginPage());
    m_stack->addWidget(createDashboardPage());
    m_stack->addWidget(createGearManagePage());
    m_stack->addWidget(createUserManagePage());
    m_stack->addWidget(createOrderManagePage());

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QString AdminMainWindow::getWeatherInfo() const
{
    QDateTime now = QDateTime::currentDateTime();
    int hour = now.time().hour();
    QString weather;
    if (hour >= 6 && hour < 12) {
        weather = tr("🌧️ 小雨 18°C");
    } else if (hour >= 12 && hour < 18) {
        weather = tr("⛅ 多云 22°C");
    } else {
        weather = tr("🌧️ 小雨 16°C");
    }
    return weather;
}

//登录页面
QWidget* AdminMainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    auto *card = new QWidget(page);
    card->setStyleSheet(Styles::pageContainer());
    card->setFixedSize(500, 450);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(50, 50, 50, 50);

    auto *iconLabel = new QLabel(QStringLiteral("🔐"), card);
    iconLabel->setStyleSheet(Styles::Labels::welcomeIcon());
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setMinimumHeight(80);

    auto *title = new QLabel(tr("管理员后台"), card);
    title->setStyleSheet(Styles::Labels::title());
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("请输入管理员账号和密码登录"), card);
    subtitle->setStyleSheet(Styles::Labels::subtitle());
    subtitle->setAlignment(Qt::AlignCenter);

    m_loginUserIdInput = new QLineEdit(card);
    m_loginPasswordInput = new QLineEdit(card);
    m_loginUserIdInput->setPlaceholderText(tr("请输入管理员账号"));
    m_loginPasswordInput->setPlaceholderText(tr("请输入密码"));
    m_loginPasswordInput->setEchoMode(QLineEdit::Password);
    m_loginUserIdInput->setFixedWidth(320);
    m_loginPasswordInput->setFixedWidth(320);

    auto *btnLogin = new QPushButton(tr("登 录"), card);
    btnLogin->setStyleSheet(Styles::Buttons::primary());
    btnLogin->setCursor(Qt::PointingHandCursor);
    
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        const QString userId = m_loginUserIdInput->text().trimmed();
        const QString password = m_loginPasswordInput->text();

        if (userId.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入管理员账号和密码"));
            return;
        }

        auto adminOpt = m_authService->adminLogin(userId, password);
        if (!adminOpt) {
            QMessageBox::warning(this, tr("登录失败"), tr("账号或密码错误，或该账号不是管理员"));
            return;
        }
        
        m_currentAdmin = std::make_shared<User>(
            adminOpt->get_id(),
            adminOpt->get_name(),
            adminOpt->get_password(),
            adminOpt->get_role(),
            adminOpt->get_credit(),
            adminOpt->get_is_active()
        );
        
        m_loginPasswordInput->clear();
        
        if (m_adminLabel) {
            m_adminLabel->setText(tr("👤 管理员：%1").arg(m_currentAdmin->get_name()));
        }
        
        QMessageBox::information(this, tr("登录成功"), tr("欢迎，%1").arg(m_currentAdmin->get_name()));
        switchPage(Page::Dashboard);
        m_refreshTimer->start(5000);
    });

    cardLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    cardLayout->addWidget(title, 0, Qt::AlignCenter);
    cardLayout->addWidget(subtitle, 0, Qt::AlignCenter);
    cardLayout->addSpacing(12);
    cardLayout->addWidget(m_loginUserIdInput, 0, Qt::AlignCenter);
    cardLayout->addWidget(m_loginPasswordInput, 0, Qt::AlignCenter);
    cardLayout->addSpacing(16);
    cardLayout->addWidget(btnLogin, 0, Qt::AlignCenter);

    layout->addWidget(card, 0, Qt::AlignCenter);
    return page;
}

//创建侧边栏的辅助函数 
static QWidget* createSidebar(QWidget* parent, AdminMainWindow* window, int activePage)
{
    auto *sidebar = new QWidget(parent);
    sidebar->setMinimumWidth(200);
    sidebar->setMaximumWidth(200);
    sidebar->setStyleSheet(Styles::adminSidebar());
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(16, 24, 16, 24);
    sidebarLayout->setSpacing(8);
    
    auto *logoLabel = new QLabel(QObject::tr("🌧️ RainHub"), sidebar);
    logoLabel->setStyleSheet(Styles::Labels::sidebarTitle());
    logoLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(logoLabel);
    sidebarLayout->addSpacing(24);
    
    auto *btnDashboard = new QPushButton(QObject::tr("📊 首页概览"), sidebar);
    btnDashboard->setStyleSheet(activePage == 1 ? Styles::Buttons::sideNavActive() : Styles::Buttons::sideNav());
    btnDashboard->setCursor(Qt::PointingHandCursor);
    
    auto *btnGear = new QPushButton(QObject::tr("☂️ 雨具管理"), sidebar);
    btnGear->setStyleSheet(activePage == 2 ? Styles::Buttons::sideNavActive() : Styles::Buttons::sideNav());
    btnGear->setCursor(Qt::PointingHandCursor);
    
    auto *btnUser = new QPushButton(QObject::tr("👤 用户管理"), sidebar);
    btnUser->setStyleSheet(activePage == 3 ? Styles::Buttons::sideNavActive() : Styles::Buttons::sideNav());
    btnUser->setCursor(Qt::PointingHandCursor);
    
    auto *btnOrder = new QPushButton(QObject::tr("📋 订单流水"), sidebar);
    btnOrder->setStyleSheet(activePage == 4 ? Styles::Buttons::sideNavActive() : Styles::Buttons::sideNav());
    btnOrder->setCursor(Qt::PointingHandCursor);
    
    // 连接信号需要在外部完成，这里只返回sidebar
    sidebarLayout->addWidget(btnDashboard);
    sidebarLayout->addWidget(btnGear);
    sidebarLayout->addWidget(btnUser);
    sidebarLayout->addWidget(btnOrder);
    sidebarLayout->addStretch();
    
    // 使用属性存储按钮指针
    sidebar->setProperty("btnDashboard", QVariant::fromValue(static_cast<void*>(btnDashboard)));
    sidebar->setProperty("btnGear", QVariant::fromValue(static_cast<void*>(btnGear)));
    sidebar->setProperty("btnUser", QVariant::fromValue(static_cast<void*>(btnUser)));
    sidebar->setProperty("btnOrder", QVariant::fromValue(static_cast<void*>(btnOrder)));
    
    return sidebar;
}

// Dashboard页面
QWidget* AdminMainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 侧边栏
    auto *sidebar = createSidebar(page, this, 1);
    auto *btnDashboard = static_cast<QPushButton*>(sidebar->property("btnDashboard").value<void*>());
    auto *btnGear = static_cast<QPushButton*>(sidebar->property("btnGear").value<void*>());
    auto *btnUser = static_cast<QPushButton*>(sidebar->property("btnUser").value<void*>());
    auto *btnOrder = static_cast<QPushButton*>(sidebar->property("btnOrder").value<void*>());
    
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });

    // 右侧内容区
    auto *contentArea = new QWidget(page);
    contentArea->setStyleSheet("background-color: #f0f2f5;");
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    contentLayout->setSpacing(16);

    // 顶部栏
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("📊 管理员后台"), contentArea);
    title->setStyleSheet(Styles::Labels::pageTitle());
    
    m_weatherLabel = new QLabel(getWeatherInfo(), contentArea);
    m_weatherLabel->setStyleSheet("font-size:13px; color: #4a4a68; padding: 8px 16px; background-color: rgba(102, 126, 234, 0.1); border-radius: 8px;");
    
    m_adminLabel = new QLabel(tr("👤 管理员："), contentArea);
    m_adminLabel->setStyleSheet("font-size:14px; color: #667eea; font-weight:600;");
    
    auto *btnLogout = new QPushButton(tr("退出登录"), contentArea);
    btnLogout->setStyleSheet(Styles::Buttons::logout());
    btnLogout->setCursor(Qt::PointingHandCursor);
    connect(btnLogout, &QPushButton::clicked, this, &AdminMainWindow::handleLogout);
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(m_weatherLabel);
    topBar->addSpacing(12);
    topBar->addWidget(m_adminLabel);
    topBar->addSpacing(12);
    topBar->addWidget(btnLogout);
    contentLayout->addLayout(topBar);

    // 统计卡片区
    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);
    
    auto *statsCard1 = new QWidget(contentArea);
    statsCard1->setStyleSheet(Styles::statCard());
    auto *card1Layout = new QVBoxLayout(statsCard1);
    card1Layout->setContentsMargins(20, 16, 20, 16);
    m_onlineDevicesLabel = new QLabel(tr("100%"), statsCard1);
    m_onlineDevicesLabel->setStyleSheet(Styles::Labels::statNumber());
    auto *card1Title = new QLabel(tr("设备在线率"), statsCard1);
    card1Title->setStyleSheet(Styles::Labels::statLabel());
    card1Layout->addWidget(m_onlineDevicesLabel);
    card1Layout->addWidget(card1Title);
    
    auto *statsCard2 = new QWidget(contentArea);
    statsCard2->setStyleSheet(Styles::statCard());
    auto *card2Layout = new QVBoxLayout(statsCard2);
    card2Layout->setContentsMargins(20, 16, 20, 16);
    m_borrowedGearsLabel = new QLabel(tr("0 把"), statsCard2);
    m_borrowedGearsLabel->setStyleSheet(Styles::Labels::statNumber());
    auto *card2Title = new QLabel(tr("雨具借出"), statsCard2);
    card2Title->setStyleSheet(Styles::Labels::statLabel());
    card2Layout->addWidget(m_borrowedGearsLabel);
    card2Layout->addWidget(card2Title);
    
    auto *statsCard3 = new QWidget(contentArea);
    statsCard3->setStyleSheet(Styles::statCard());
    auto *card3Layout = new QVBoxLayout(statsCard3);
    card3Layout->setContentsMargins(20, 16, 20, 16);
    m_faultCountLabel = new QLabel(tr("0"), statsCard3);
    m_faultCountLabel->setStyleSheet("font-size: 28px; font-weight: 700; color: #ff3d71; background: transparent;");
    auto *card3Title = new QLabel(tr("待处理故障"), statsCard3);
    card3Title->setStyleSheet(Styles::Labels::statLabel());
    card3Layout->addWidget(m_faultCountLabel);
    card3Layout->addWidget(card3Title);
    
    statsLayout->addWidget(statsCard1);
    statsLayout->addWidget(statsCard2);
    statsLayout->addWidget(statsCard3);
    statsLayout->addStretch();
    contentLayout->addLayout(statsLayout);

    // 站点概览表格
    auto *tableTitle = new QLabel(tr("📍 站点雨具概览"), contentArea);
    tableTitle->setStyleSheet("font-size:16px; font-weight:600; color: #1a1a2e;");
    contentLayout->addWidget(tableTitle);
    
    m_stationTable = new QTableWidget(contentArea);
    m_stationTable->setColumnCount(5);
    m_stationTable->setHorizontalHeaderLabels({
        tr("站点名称"), tr("总雨具数"), tr("可借数量"), tr("已借出"), tr("故障数")
    });
    m_stationTable->horizontalHeader()->setStretchLastSection(true);
    m_stationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_stationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_stationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    contentLayout->addWidget(m_stationTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);
    
    return page;
}

// 雨具管理页面
QWidget* AdminMainWindow::createGearManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *sidebar = createSidebar(page, this, 2);
    auto *btnDashboard = static_cast<QPushButton*>(sidebar->property("btnDashboard").value<void*>());
    auto *btnGear = static_cast<QPushButton*>(sidebar->property("btnGear").value<void*>());
    auto *btnUser = static_cast<QPushButton*>(sidebar->property("btnUser").value<void*>());
    auto *btnOrder = static_cast<QPushButton*>(sidebar->property("btnOrder").value<void*>());
    
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });

    auto *contentArea = new QWidget(page);
    contentArea->setStyleSheet("background-color: #f0f2f5;");
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("☂️ 雨具管理"), contentArea);
    title->setStyleSheet(Styles::Labels::pageTitle());

    // 筛选区域
    auto *filterCard = new QWidget(contentArea);
    filterCard->setStyleSheet(Styles::statCard());
    auto *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(20, 16, 20, 16);
    
    auto *stationLabel = new QLabel(tr("站点："), filterCard);
    stationLabel->setStyleSheet(Styles::Labels::formLabel());
    m_gearStationCombo = new QComboBox(filterCard);
    m_gearStationCombo->addItem(tr("全部站点"), 0);
    m_gearStationCombo->setFixedWidth(180);
    
    auto *slotLabel = new QLabel(tr("槽位："), filterCard);
    slotLabel->setStyleSheet(Styles::Labels::formLabel());
    m_gearSlotCombo = new QComboBox(filterCard);
    m_gearSlotCombo->addItem(tr("全部槽位"), 0);
    for (int i = 1; i <= 12; ++i) {
        m_gearSlotCombo->addItem(QStringLiteral("#%1").arg(i), i);
    }
    m_gearSlotCombo->setFixedWidth(120);
    
    auto *btnRefresh = new QPushButton(tr("刷新"), filterCard);
    btnRefresh->setStyleSheet(Styles::Buttons::secondary());
    btnRefresh->setCursor(Qt::PointingHandCursor);
    connect(btnRefresh, &QPushButton::clicked, this, &AdminMainWindow::refreshGearManageData);
    connect(m_gearStationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &AdminMainWindow::refreshGearManageData);
    
    filterLayout->addWidget(stationLabel);
    filterLayout->addWidget(m_gearStationCombo);
    filterLayout->addSpacing(16);
    filterLayout->addWidget(slotLabel);
    filterLayout->addWidget(m_gearSlotCombo);
    filterLayout->addSpacing(16);
    filterLayout->addWidget(btnRefresh);
    filterLayout->addStretch();

    // 雨具表格
    m_gearTable = new QTableWidget(contentArea);
    m_gearTable->setColumnCount(6);
    m_gearTable->setHorizontalHeaderLabels({
        tr("雨具ID"), tr("类型"), tr("站点"), tr("槽位"), tr("状态"), tr("操作")
    });
    m_gearTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_gearTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_gearTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_gearTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addWidget(filterCard);
    contentLayout->addWidget(m_gearTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

// 用户管理页面
QWidget* AdminMainWindow::createUserManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *sidebar = createSidebar(page, this, 3);
    auto *btnDashboard = static_cast<QPushButton*>(sidebar->property("btnDashboard").value<void*>());
    auto *btnGear = static_cast<QPushButton*>(sidebar->property("btnGear").value<void*>());
    auto *btnUser = static_cast<QPushButton*>(sidebar->property("btnUser").value<void*>());
    auto *btnOrder = static_cast<QPushButton*>(sidebar->property("btnOrder").value<void*>());
    
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });

    auto *contentArea = new QWidget(page);
    contentArea->setStyleSheet("background-color: #f0f2f5;");
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("👤 用户管理"), contentArea);
    title->setStyleSheet(Styles::Labels::pageTitle());

    // 搜索区域
    auto *searchCard = new QWidget(contentArea);
    searchCard->setStyleSheet(Styles::statCard());
    auto *searchLayout = new QHBoxLayout(searchCard);
    searchLayout->setContentsMargins(20, 16, 20, 16);
    
    m_userSearchInput = new QLineEdit(searchCard);
    m_userSearchInput->setPlaceholderText(tr("输入学号/工号或姓名搜索"));
    m_userSearchInput->setFixedWidth(280);
    
    auto *btnSearch = new QPushButton(tr("搜索"), searchCard);
    btnSearch->setStyleSheet(Styles::Buttons::secondary());
    btnSearch->setCursor(Qt::PointingHandCursor);
    connect(btnSearch, &QPushButton::clicked, this, &AdminMainWindow::refreshUserManageData);
    connect(m_userSearchInput, &QLineEdit::returnPressed, this, &AdminMainWindow::refreshUserManageData);
    
    searchLayout->addWidget(m_userSearchInput);
    searchLayout->addWidget(btnSearch);
    searchLayout->addStretch();

    // 用户表格
    m_userTable = new QTableWidget(contentArea);
    m_userTable->setColumnCount(6);
    m_userTable->setHorizontalHeaderLabels({
        tr("学号/工号"), tr("姓名"), tr("角色"), tr("余额"), tr("状态"), tr("操作")
    });
    m_userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_userTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addWidget(searchCard);
    contentLayout->addWidget(m_userTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

// 订单管理页面
QWidget* AdminMainWindow::createOrderManagePage()
{
    auto *page = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto *sidebar = createSidebar(page, this, 4);
    auto *btnDashboard = static_cast<QPushButton*>(sidebar->property("btnDashboard").value<void*>());
    auto *btnGear = static_cast<QPushButton*>(sidebar->property("btnGear").value<void*>());
    auto *btnUser = static_cast<QPushButton*>(sidebar->property("btnUser").value<void*>());
    auto *btnOrder = static_cast<QPushButton*>(sidebar->property("btnOrder").value<void*>());
    
    connect(btnDashboard, &QPushButton::clicked, this, [this] { switchPage(Page::Dashboard); });
    connect(btnGear, &QPushButton::clicked, this, [this] { switchPage(Page::GearManage); });
    connect(btnUser, &QPushButton::clicked, this, [this] { switchPage(Page::UserManage); });
    connect(btnOrder, &QPushButton::clicked, this, [this] { switchPage(Page::OrderManage); });

    auto *contentArea = new QWidget(page);
    contentArea->setStyleSheet("background-color: #f0f2f5;");
    auto *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(24, 24, 24, 24);
    contentLayout->setSpacing(16);

    auto *title = new QLabel(tr("📋 订单流水"), contentArea);
    title->setStyleSheet(Styles::Labels::pageTitle());

    m_orderTable = new QTableWidget(contentArea);
    m_orderTable->setColumnCount(6);
    m_orderTable->setHorizontalHeaderLabels({
        tr("流水号"), tr("用户"), tr("雨具ID"), tr("借出时间"), tr("归还时间"), tr("费用")
    });
    m_orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_orderTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_orderTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_orderTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_orderTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    contentLayout->addWidget(title);
    contentLayout->addWidget(m_orderTable, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    return page;
}

// 页面切换和数据刷新
void AdminMainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    
    switch (page) {
        case Page::Dashboard:
            refreshDashboardData();
            break;
        case Page::GearManage:
            refreshGearManageData();
            break;
        case Page::UserManage:
            refreshUserManageData();
            break;
        case Page::OrderManage:
            refreshOrderManageData();
            break;
        default:
            break;
    }
}

void AdminMainWindow::handleLogout()
{
    m_refreshTimer->stop();
    m_currentAdmin.reset();
    
    if (m_loginUserIdInput) m_loginUserIdInput->clear();
    if (m_loginPasswordInput) m_loginPasswordInput->clear();
    
    switchPage(Page::Login);
}

void AdminMainWindow::onRefreshTimer()
{
    int currentPage = m_stack->currentIndex();
    switch (static_cast<Page>(currentPage)) {
        case Page::Dashboard:
            refreshDashboardData();
            break;
        case Page::GearManage:
            refreshGearManageData();
            break;
        case Page::UserManage:
            refreshUserManageData();
            break;
        case Page::OrderManage:
            refreshOrderManageData();
            break;
        default:
            break;
    }
}

void AdminMainWindow::refreshDashboardData()
{
    if (m_weatherLabel) {
        m_weatherLabel->setText(getWeatherInfo());
    }
    
    // 获取统计数据
    double onlineRate = m_stationService->getOnlineRate();
    int borrowedCount = m_gearService->getTotalBorrowedCount();
    int brokenCount = m_gearService->getTotalBrokenCount();
    
    if (m_onlineDevicesLabel) {
        m_onlineDevicesLabel->setText(QString("%1%").arg(QString::number(onlineRate, 'f', 0)));
    }
    if (m_borrowedGearsLabel) {
        m_borrowedGearsLabel->setText(QString("%1 把").arg(borrowedCount));
    }
    if (m_faultCountLabel) {
        m_faultCountLabel->setText(QString::number(brokenCount));
    }
    
    // 刷新站点表格
    if (m_stationTable) {
        m_stationTable->setRowCount(0);
        
        auto stationStats = m_stationService->getStationStats();
        for (const auto& stats : stationStats) {
            int row = m_stationTable->rowCount();
            m_stationTable->insertRow(row);
            
            m_stationTable->setItem(row, 0, new QTableWidgetItem(stats.name));
            m_stationTable->setItem(row, 1, new QTableWidgetItem(QString::number(stats.totalGears)));
            
            auto *availableItem = new QTableWidgetItem(QString::number(stats.availableCount));
            availableItem->setForeground(QBrush(QColor("#00d68f")));
            m_stationTable->setItem(row, 2, availableItem);
            
            auto *borrowedItem = new QTableWidgetItem(QString::number(stats.borrowedCount));
            borrowedItem->setForeground(QBrush(QColor("#667eea")));
            m_stationTable->setItem(row, 3, borrowedItem);
            
            auto *brokenItem = new QTableWidgetItem(QString::number(stats.brokenCount));
            if (stats.brokenCount > 0) {
                brokenItem->setForeground(QBrush(QColor("#ff3d71")));
            }
            m_stationTable->setItem(row, 4, brokenItem);
        }
    }
}

void AdminMainWindow::refreshGearManageData()
{
    if (!m_gearTable) return;
    
    m_gearTable->setRowCount(0);
    
    int selectedStationId = m_gearStationCombo ? m_gearStationCombo->currentData().toInt() : 0;
    int selectedSlotId = m_gearSlotCombo ? m_gearSlotCombo->currentData().toInt() : 0;
    
    // 填充站点下拉框（只在第一次）
    if (m_gearStationCombo && m_gearStationCombo->count() == 1) {
        auto stationStats = m_stationService->getStationStats();
        for (const auto& stats : stationStats) {
            m_gearStationCombo->addItem(stats.name, stats.stationId);
        }
    }
    
    auto gears = m_gearService->getAllGears(selectedStationId, selectedSlotId);
    
    QStringList typeNames = {tr("未知"), tr("普通塑料伞"), tr("高质量抗风伞"), tr("专用遮阳伞"), tr("雨衣")};
    QStringList statusNames = {tr("未知"), tr("可借"), tr("已借出"), tr("故障")};
    
    // 获取站点名称映射
    auto stationStats = m_stationService->getStationStats();
    QMap<int, QString> stationNames;
    for (const auto& stats : stationStats) {
        stationNames[stats.stationId] = stats.name;
    }
    
    for (const auto& gear : gears) {
        int row = m_gearTable->rowCount();
        m_gearTable->insertRow(row);
        
        m_gearTable->setItem(row, 0, new QTableWidgetItem(gear.gearId));
        m_gearTable->setItem(row, 1, new QTableWidgetItem(
            gear.typeId >= 1 && gear.typeId <= 4 ? typeNames[gear.typeId] : tr("未知")));
        
        QString stationDisplay = gear.status == 2 ? tr("已借出") : stationNames.value(gear.stationId, tr("未知"));
        m_gearTable->setItem(row, 2, new QTableWidgetItem(stationDisplay));
        
        QString slotDisplay = gear.status == 2 ? tr("-") : (gear.slotId > 0 ? QStringLiteral("#%1").arg(gear.slotId) : tr("无"));
        m_gearTable->setItem(row, 3, new QTableWidgetItem(slotDisplay));
        
        auto *statusItem = new QTableWidgetItem(
            gear.status >= 1 && gear.status <= 3 ? statusNames[gear.status] : tr("未知"));
        if (gear.status == 1) {
            statusItem->setForeground(QBrush(QColor("#00d68f")));
        } else if (gear.status == 2) {
            statusItem->setForeground(QBrush(QColor("#8f8fa3")));
        } else if (gear.status == 3) {
            statusItem->setForeground(QBrush(QColor("#ff3d71")));
        }
        m_gearTable->setItem(row, 4, statusItem);
        
        // 操作按钮
        auto *btnModify = new QPushButton(tr("修改状态"));
        btnModify->setStyleSheet(Styles::Buttons::secondary());
        btnModify->setCursor(Qt::PointingHandCursor);
        
        connect(btnModify, &QPushButton::clicked, this, [this, gear = gear]() {
            QDialog dialog(this);
            dialog.setWindowTitle(tr("修改雨具状态"));
            dialog.setStyleSheet("QDialog { background-color: #ffffff; }");
            auto *layout = new QVBoxLayout(&dialog);
            layout->setSpacing(16);
            layout->setContentsMargins(24, 24, 24, 24);
            
            auto *label = new QLabel(tr("雨具ID: %1\n当前状态: %2")
                .arg(gear.gearId)
                .arg(gear.status == 1 ? tr("可借") : (gear.status == 2 ? tr("已借出") : tr("故障"))));
            label->setStyleSheet(Styles::Labels::info());
            layout->addWidget(label);
            
            auto *combo = new QComboBox(&dialog);
            combo->addItem(tr("可借"), 1);
            combo->addItem(tr("已借出"), 2);
            combo->addItem(tr("故障"), 3);
            combo->setCurrentIndex(gear.status - 1);
            layout->addWidget(combo);
            
            auto *btnLayout = new QHBoxLayout();
            auto *btnOk = new QPushButton(tr("确定"), &dialog);
            btnOk->setStyleSheet(Styles::Buttons::primary());
            auto *btnCancel = new QPushButton(tr("取消"), &dialog);
            btnCancel->setStyleSheet(Styles::Buttons::back());
            connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
            connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
            btnLayout->addWidget(btnOk);
            btnLayout->addWidget(btnCancel);
            layout->addLayout(btnLayout);
            
            if (dialog.exec() == QDialog::Accepted) {
                int newStatus = combo->currentData().toInt();
                if (m_gearService->updateGearStatus(gear.gearId, newStatus)) {
                    QMessageBox::information(this, tr("成功"), tr("雨具状态已更新"));
                    refreshGearManageData();
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("更新失败，请重试"));
                }
            }
        });
        
        m_gearTable->setCellWidget(row, 5, btnModify);
    }
}

void AdminMainWindow::refreshUserManageData()
{
    if (!m_userTable) return;
    
    m_userTable->setRowCount(0);
    
    QString searchText = m_userSearchInput ? m_userSearchInput->text().trimmed() : QString();
    auto users = m_userService->getAllUsers(searchText);
    
    QStringList roleNames = {tr("学生"), tr("教职工"), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr(""), tr("管理员")};
    
    for (const auto& user : users) {
        int row = m_userTable->rowCount();
        m_userTable->insertRow(row);
        
        m_userTable->setItem(row, 0, new QTableWidgetItem(user.get_id()));
        m_userTable->setItem(row, 1, new QTableWidgetItem(user.get_name()));
        m_userTable->setItem(row, 2, new QTableWidgetItem(
            user.get_role() >= 0 && user.get_role() < roleNames.size() ? roleNames[user.get_role()] : tr("未知")));
        
        auto *creditItem = new QTableWidgetItem(QString("￥%1").arg(QString::number(user.get_credit(), 'f', 2)));
        creditItem->setForeground(QBrush(QColor("#00d68f")));
        m_userTable->setItem(row, 3, creditItem);
        
        auto *statusItem = new QTableWidgetItem(user.get_is_active() ? tr("已激活") : tr("未激活"));
        statusItem->setForeground(QBrush(user.get_is_active() ? QColor("#00d68f") : QColor("#8f8fa3")));
        m_userTable->setItem(row, 4, statusItem);
        
        // 重置密码按钮
        auto *btnResetPwd = new QPushButton(tr("重置密码"));
        btnResetPwd->setStyleSheet(Styles::Buttons::secondary());
        btnResetPwd->setCursor(Qt::PointingHandCursor);
        
        connect(btnResetPwd, &QPushButton::clicked, this, [this, user = user]() {
            QDialog dialog(this);
            dialog.setWindowTitle(tr("重置密码"));
            dialog.setStyleSheet("QDialog { background-color: #ffffff; }");
            auto *layout = new QVBoxLayout(&dialog);
            layout->setSpacing(16);
            layout->setContentsMargins(24, 24, 24, 24);
            
            auto *label = new QLabel(tr("用户: %1 (%2)").arg(user.get_id()).arg(user.get_name()));
            label->setStyleSheet(Styles::Labels::info());
            layout->addWidget(label);
            
            auto *inputPwd = new QLineEdit(&dialog);
            inputPwd->setPlaceholderText(tr("请输入新密码"));
            inputPwd->setEchoMode(QLineEdit::Password);
            layout->addWidget(inputPwd);
            
            auto *btnLayout = new QHBoxLayout();
            auto *btnOk = new QPushButton(tr("确定"), &dialog);
            btnOk->setStyleSheet(Styles::Buttons::primary());
            auto *btnCancel = new QPushButton(tr("取消"), &dialog);
            btnCancel->setStyleSheet(Styles::Buttons::back());
            connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
            connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
            btnLayout->addWidget(btnOk);
            btnLayout->addWidget(btnCancel);
            layout->addLayout(btnLayout);
            
            if (dialog.exec() == QDialog::Accepted) {
                QString newPassword = inputPwd->text();
                if (newPassword.length() < 6) {
                    QMessageBox::warning(this, tr("提示"), tr("密码长度至少为6位"));
                    return;
                }
                if (m_userService->resetUserPassword(user.get_id(), newPassword)) {
                    QMessageBox::information(this, tr("成功"), tr("密码已重置"));
                } else {
                    QMessageBox::critical(this, tr("失败"), tr("重置失败，请重试"));
                }
            }
        });
        
        m_userTable->setCellWidget(row, 5, btnResetPwd);
    }
}

void AdminMainWindow::refreshOrderManageData()
{
    if (!m_orderTable) return;
    
    m_orderTable->setRowCount(0);
    
    auto orders = m_orderService->getRecentOrders(100);
    
    for (const auto& order : orders) {
        int row = m_orderTable->rowCount();
        m_orderTable->insertRow(row);
        
        m_orderTable->setItem(row, 0, new QTableWidgetItem(QString::number(order.recordId)));
        m_orderTable->setItem(row, 1, new QTableWidgetItem(order.userId));
        m_orderTable->setItem(row, 2, new QTableWidgetItem(order.gearId));
        m_orderTable->setItem(row, 3, new QTableWidgetItem(order.borrowTime));
        
        auto *returnItem = new QTableWidgetItem(order.returnTime.isEmpty() ? tr("未归还") : order.returnTime);
        if (order.returnTime.isEmpty()) {
            returnItem->setForeground(QBrush(QColor("#ffaa00")));
        }
        m_orderTable->setItem(row, 4, returnItem);
        
        auto *costItem = new QTableWidgetItem(QString("￥%1").arg(QString::number(order.cost, 'f', 2)));
        m_orderTable->setItem(row, 5, costItem);
    }
}

