/*
    主页/仪表板页面实现
*/
#include "DashboardPage.h"
#include "../assets/Styles.h"
#include "../../control/StationService.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

DashboardPage::DashboardPage(StationService *stationService, QWidget *parent)
    : QWidget(parent)
    , m_stationService(stationService)
{
    setupUi();
}

void DashboardPage::setupUi()
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(24, 24, 24, 24);
    outerLayout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(this);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 24, 32, 24);
    cardLayout->setSpacing(20);

    // 顶部栏：标题和退出登录按钮
    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(0, 0, 0, 0);
    
    auto *title = new QLabel(tr("☂️ NUIST 智能雨具系统"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    
    auto *btnLogout = new QPushButton(tr("退出登录"), card);
    btnLogout->setStyleSheet(Styles::Buttons::logout());
    btnLogout->setCursor(Qt::PointingHandCursor);
    connect(btnLogout, &QPushButton::clicked, this, &DashboardPage::logoutClicked);
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnLogout);

    // 站点选择区域
    auto *stationContainer = new QWidget(card);
    stationContainer->setStyleSheet("background: transparent;");
    auto *stationLayout = new QHBoxLayout(stationContainer);
    stationLayout->setContentsMargins(0, 8, 0, 8);
    stationLayout->setSpacing(12);
    
    auto *stationLabel = new QLabel(tr("📍 当前站点："), stationContainer);
    stationLabel->setStyleSheet("font-size: 15px; font-weight: 600; color: #4a4a68; background: transparent;");
    
    m_stationComboBox = new QComboBox(stationContainer);
    m_stationComboBox->setFixedWidth(220);
    connect(m_stationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DashboardPage::onStationChanged);
    
    stationLayout->addStretch();
    stationLayout->addWidget(stationLabel);
    stationLayout->addWidget(m_stationComboBox);
    stationLayout->addStretch();

    // 主功能按钮区
    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(24);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    auto *btnBorrow = new QPushButton(tr("☔ 我要借伞"), card);
    auto *btnReturn = new QPushButton(tr("🔄 我要还伞"), card);
    btnBorrow->setStyleSheet(Styles::Buttons::feature());
    btnReturn->setStyleSheet(Styles::Buttons::feature());
    btnBorrow->setCursor(Qt::PointingHandCursor);
    btnReturn->setCursor(Qt::PointingHandCursor);

    connect(btnBorrow, &QPushButton::clicked, this, [this]() {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        emit borrowClicked(m_currentStationId);
    });
    
    connect(btnReturn, &QPushButton::clicked, this, [this]() {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        emit returnClicked(m_currentStationId);
    });

    buttonsLayout->addWidget(btnBorrow);
    buttonsLayout->addWidget(btnReturn);

    // 使用说明链接
    auto *btnInstruction = new QPushButton(tr("📖 查看使用说明与收费标准"), card);
    btnInstruction->setStyleSheet(Styles::Buttons::link());
    btnInstruction->setCursor(Qt::PointingHandCursor);
    connect(btnInstruction, &QPushButton::clicked, this, &DashboardPage::instructionClicked);

    // 底部导航
    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0, 16, 0, 0);
    bottom->setSpacing(16);
    
    auto *btnProfile = new QPushButton(tr("👤 个人中心"), card);
    btnProfile->setStyleSheet(Styles::Buttons::nav());
    btnProfile->setCursor(Qt::PointingHandCursor);
    connect(btnProfile, &QPushButton::clicked, this, &DashboardPage::profileClicked);
    
    auto *btnMap = new QPushButton(tr("🗺️ 站点地图"), card);
    btnMap->setStyleSheet(Styles::Buttons::nav());
    btnMap->setCursor(Qt::PointingHandCursor);
    connect(btnMap, &QPushButton::clicked, this, &DashboardPage::mapClicked);
    
    bottom->addWidget(btnProfile);
    bottom->addStretch();
    bottom->addWidget(btnMap);

    // 组装卡片布局
    cardLayout->addLayout(topBar);
    cardLayout->addWidget(stationContainer);
    cardLayout->addStretch();
    cardLayout->addLayout(buttonsLayout);
    cardLayout->addWidget(btnInstruction, 0, Qt::AlignCenter);
    cardLayout->addStretch();
    cardLayout->addLayout(bottom);

    outerLayout->addWidget(card);
    
    // 初始加载站点列表
    refreshStations();
}

void DashboardPage::setUser(std::shared_ptr<User> user)
{
    m_currentUser = user;
}

void DashboardPage::refreshStations()
{
    m_stationComboBox->clear();
    m_stationComboBox->addItem(tr("-- 请选择站点 --"), 0);
    
    auto stations = m_stationService->getAllStations();
    for (size_t i = 0; i < stations.size(); ++i) {
        const auto &station = stations[i];
        if (station) {
            const bool isOnline = station->get_online();
            QString displayName = station->get_name();
            if (!isOnline) {
                displayName += tr("（离线）");
            }
            m_stationComboBox->addItem(displayName, static_cast<int>(station->get_station()));
            const int itemIndex = m_stationComboBox->count() - 1;
            // 额外存一份在线状态：Qt::UserRole+1
            m_stationComboBox->setItemData(itemIndex, isOnline, Qt::UserRole + 1);
        }
    }
}

void DashboardPage::onStationChanged(int index)
{
    if (index < 0) return;
    m_currentStationId = m_stationComboBox->itemData(index).toInt();
    if (m_currentStationId == 0) return;

    const bool isOnline = m_stationComboBox->itemData(index, Qt::UserRole + 1).toBool();
    if (!isOnline) {
        QMessageBox::warning(this, tr("提示"), tr("该站点处于离线状态，无法提供服务"));
        // 自动取消选择，避免进入借还流程
        m_stationComboBox->setCurrentIndex(0);
        m_currentStationId = 0;
    }
}

