/*
    地图页面实现
    
    优化说明：
    - 静态数据（站点名称、坐标、描述）从 JSON 读取 → 极快
    - 动态数据（库存数量）从数据库读取 → 保证实时性
    - 不再加载完整的雨具对象，只统计数量 → 更高效
*/
#include "MapPage.h"
#include "../assets/Styles.h"
#include "../../control/StationService.h"
#include "../../utils/MapConfigLoader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox> 
#include <QTimer>

MapPage::MapPage(StationService *stationService, QWidget *parent)
    : QWidget(parent)
    , m_stationService(stationService)
{
    setupUi();
}

void MapPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    // 玻璃卡片容器
    auto *card = new QWidget(this);
    card->setStyleSheet(Styles::pageContainer());
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 20, 24, 20);
    cardLayout->setSpacing(16);

    // 顶部标题栏
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(tr("🗺️ 校园雨具站点分布图"), card);
    title->setStyleSheet(Styles::Labels::pageTitle());
    
    auto *btnBack = new QPushButton(tr("返回主页"), card);
    btnBack->setStyleSheet(Styles::Buttons::back());
    btnBack->setCursor(Qt::PointingHandCursor);
    connect(btnBack, &QPushButton::clicked, this, &MapPage::backRequested);
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnBack);

    // 图例说明
    auto *legendLabel = new QLabel(tr("🟢 库存充足(≥5)  🟡 库存紧张(2-4)  🔴 库存不足(<2)  ⚫ 站点离线"), card);
    legendLabel->setStyleSheet(Styles::Labels::hint());
    legendLabel->setAlignment(Qt::AlignCenter);

    // 地图容器
    m_mapContainer = new QWidget(card);
    m_mapContainer->setMinimumSize(750, 500);
    m_mapContainer->setStyleSheet(Styles::mapContainer());

    cardLayout->addLayout(topBar);
    cardLayout->addWidget(legendLabel);
    cardLayout->addWidget(m_mapContainer, 1);
    
    layout->addWidget(card);
}

void MapPage::refreshMap()
{
    loadMapStations();
}

void MapPage::loadMapStations()
{
    if (!m_mapContainer) return;
    
    // 清除旧的站点按钮和标签
    QList<QWidget*> children = m_mapContainer->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (auto *child : children) {
        delete child;
    }
    
    // 优化：分离静态数据和动态数据 
    
    // 1. 从 JSON 读取静态配置（站点名称、坐标、描述）- 极快
    QMap<int, StationConfig> stationConfigs = MapConfigLoader::loadStationConfigs();
    
    // 2. 从数据库读取动态数据（库存数量和在线状态）- 一次查询获取所有信息
    QMap<int, StationMapInfo> stationMapInfo = m_stationService->getStationMapInfo();
    
    // 计算容器尺寸
    int containerWidth = m_mapContainer->width();
    int containerHeight = m_mapContainer->height();
    if (containerWidth < 100) containerWidth = 750;
    if (containerHeight < 100) containerHeight = 500;
    
    // 遍历 JSON 配置的站点绘制
    for (auto it = stationConfigs.constBegin(); it != stationConfigs.constEnd(); ++it) {
        const StationConfig &cfg = it.value();
        int stationId = cfg.stationId;
        QString name = cfg.name;
        double posX = cfg.posX;
        double posY = cfg.posY;
        QString description = cfg.description;
        
        // 从数据库结果获取库存数量和在线状态
        StationMapInfo info = stationMapInfo.value(stationId, {0, true});  // 默认：库存0，在线
        int availableCount = info.availableCount;
        bool isOnline = info.isOnline;
        
        // 创建站点按钮
        auto *stationBtn = new QPushButton(m_mapContainer);
        stationBtn->setFixedSize(24, 24);
        stationBtn->setCursor(Qt::PointingHandCursor);
        
        // 根据在线状态和库存数量设置颜色
        QString color;
        if (!isOnline) {
            color = "#95a5a6";  // 灰色 - 站点离线（优先级最高）
        } else if (availableCount >= 5) {
            color = "#2ecc71";  // 绿色 - 库存充足
        } else if (availableCount >= 2) {
            color = "#f1c40f";  // 黄色 - 库存紧张
        } else {
            color = "#e74c3c";  // 红色 - 库存不足
        }
        
        stationBtn->setStyleSheet(QString(
            "QPushButton {"
            "  background-color: %1;"
            "  border: 2px solid white;"
            "  border-radius: 12px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %1;"
            "  border: 3px solid #3498db;"
            "  border-radius: 12px;"
            "}"
        ).arg(color));
        
        // 设置工具提示
        QString statusText = isOnline ? tr("在线") : tr("离线");
        stationBtn->setToolTip(QString("%1\n状态：%2\n可借雨具：%3 把\n%4")
            .arg(name).arg(statusText).arg(availableCount).arg(description));
        
        // 点击显示详细信息
        connect(stationBtn, &QPushButton::clicked, this, [this, name, availableCount, description, isOnline]() {
            QString statusText = isOnline ? tr("🟢 在线") : tr("🔴 离线");
            QString msg = QString("<h3>%1</h3>"
                "<p><b>在线状态：</b>%2</p>"
                "<p><b>可借雨具数量：</b>%3 把</p>"
                "<p><b>站点说明：</b>%4</p>")
                .arg(name).arg(statusText).arg(availableCount).arg(description);
            QMessageBox::information(this, tr("站点信息"), msg);
        });
        
        // 添加站点名称标签
        auto *nameLabel = new QLabel(name, m_mapContainer);
        nameLabel->setStyleSheet("font-size: 11px; font-weight: 600; color: #2c3e50; "
                                "background-color: rgba(255, 255, 255, 200); "
                                "padding: 2px 6px; border-radius: 3px;");
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->adjustSize();
        
        // 设置位置
        int x = static_cast<int>(containerWidth * posX) - 12;
        int y = static_cast<int>(containerHeight * posY) - 12;
        stationBtn->move(x, y);
        nameLabel->move(x - nameLabel->width() / 2 + 12, y + 28);
        
        stationBtn->show();
        nameLabel->show();
        
        // 延迟再次设置位置（确保容器已完成布局）
        QTimer::singleShot(100, this, [this, stationBtn, nameLabel, posX, posY]() {
            if (!m_mapContainer || !stationBtn || !nameLabel) return;
            int w = m_mapContainer->width();
            int h = m_mapContainer->height();
            if (w < 100 || h < 100) return;
            
            int x = static_cast<int>(w * posX) - 12;
            int y = static_cast<int>(h * posY) - 12;
            stationBtn->move(x, y);
            nameLabel->move(x - nameLabel->width() / 2 + 12, y + 28);
        });
    }
}
