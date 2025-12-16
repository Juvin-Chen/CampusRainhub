#include "MainWindow.h"
#include "SlotItem.h"
#include "Model/RainGearFactory.h"
#include "Model/RainGear_subclasses.hpp"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    switchPage(Page::Welcome);
    setWindowTitle("RainHub Client");
    resize(900, 700);
}

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(createWelcomePage());     // 0 Welcome
    m_stack->addWidget(createCardReadPage());    // 1 CardRead
    m_stack->addWidget(createUserInputPage());   // 2 UserInput
    m_stack->addWidget(createFirstLoginPage());  // 3 FirstLogin
    m_stack->addWidget(createLoginPage());       // 4 Login
    m_stack->addWidget(createResetPwdPage());    // 5 ResetPwd
    m_stack->addWidget(createDashboardPage());   // 6 Dashboard
    m_stack->addWidget(createBorrowPage());      // 7 Borrow
    m_stack->addWidget(createMapPage());         // 8 Map
    m_stack->addWidget(createProfilePage());     // 9 Profile
    m_stack->addWidget(createInstructionPage()); // 10 Instruction

    layout->addWidget(m_stack);
    setCentralWidget(central);
}

QWidget* MainWindow::createWelcomePage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel(tr("NUIST智能自助雨具系统"), page);
    title->setStyleSheet("font-size:28px; font-weight:700;");
    auto *subtitle = new QLabel(tr("欢迎您的使用"), page);
    subtitle->setStyleSheet("font-size:18px; color:#555;");

    auto *btnStart = new QPushButton(tr("开始使用"), page);
    btnStart->setFixedWidth(180);
    btnStart->setStyleSheet("padding:12px 18px; font-size:16px;");
    connect(btnStart, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(btnStart, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createCardReadPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    auto *iconLabel = new QLabel(page);
    iconLabel->setText("📱");
    iconLabel->setStyleSheet("font-size:80px;");
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *tip = new QLabel(tr("请将您的一卡通放置在刷卡处"), page);
    tip->setStyleSheet("font-size:24px; font-weight:600; color:#2c3e50;");
    tip->setAlignment(Qt::AlignCenter);

    auto *subtip = new QLabel(tr("系统将自动识别您的学号和姓名"), page);
    subtip->setStyleSheet("font-size:16px; color:#7f8c8d;");
    subtip->setAlignment(Qt::AlignCenter);

    auto *btnConfirm = new QPushButton(tr("确定"), page);
    btnConfirm->setFixedSize(200, 50);
    btnConfirm->setStyleSheet("font-size:18px; padding:10px;");
    connect(btnConfirm, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Welcome);
    });

    layout->addStretch();
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    layout->addWidget(tip, 0, Qt::AlignCenter);
    layout->addWidget(subtip, 0, Qt::AlignCenter);
    layout->addSpacing(30);
    layout->addWidget(btnConfirm, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    layout->addStretch();
    return page;
}

QWidget* MainWindow::createUserInputPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("请输入您的信息"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("（软件模拟：请手动输入学号/工号和姓名）"), page);
    subtitle->setStyleSheet("font-size:14px; color:#7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    // 使用成员变量保存输入框指针，以便退出登录时清空
    if (!m_inputUser) {
        m_inputUser = new QLineEdit(page);
    }
    if (!m_inputName) {
        m_inputName = new QLineEdit(page);
    }
    m_inputUser->setPlaceholderText(tr("请输入学号/工号"));
    m_inputName->setPlaceholderText(tr("请输入姓名"));
    m_inputUser->setFixedWidth(300);
    m_inputName->setFixedWidth(300);
    m_inputUser->clear(); // 每次显示页面时清空
    m_inputName->clear(); // 每次显示页面时清空
    
    form->addRow(tr("学号/工号："), m_inputUser);
    form->addRow(tr("姓名："), m_inputName);

    auto *btnSubmit = new QPushButton(tr("提交"), page);
    btnSubmit->setFixedWidth(160);
    btnSubmit->setStyleSheet("font-size:16px; padding:10px;");
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        // 直接从输入框获取值
        const QString userId = m_inputUser->text().trimmed();
        const QString realName = m_inputName->text().trimmed();

        if (userId.isEmpty() || realName.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入学号/工号和姓名"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
            return;
        }

        qDebug() << "[UserInput] 查询用户:" << userId << realName;

        // 查询用户是否存在
        auto record = DatabaseManager::fetchUserByIdAndName(userId, realName);
        if (!record) {
            QMessageBox::warning(this, tr("用户不存在"), tr("未找到该学号/工号和姓名对应的用户，请检查输入。"));
            return;
        }

        qDebug() << "[UserInput] 找到用户, is_active:" << record->isActive;

        // 保存临时用户信息
        m_tempUserId = userId;
        m_tempUserName = realName;
        m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;

        // 根据is_active判断是否首次登录
        if (!record->isActive) {
            // 首次登录，跳转到设置密码页面
            qDebug() << "[UserInput] 首次登录，跳转到设置密码页面";
            switchPage(Page::FirstLogin);
        } else {
            // 非首次登录，跳转到密码登录页面
            qDebug() << "[UserInput] 非首次登录，跳转到密码登录页面";
            switchPage(Page::Login);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::CardRead);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createFirstLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    auto *title = new QLabel(tr("首次登录 - 设置密码"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("欢迎！请设置您的登录密码"), page);
    subtitle->setStyleSheet("font-size:14px; color:#7f8c8d;");
    subtitle->setAlignment(Qt::AlignCenter);

    // 显示用户信息（只读）
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setAlignment(Qt::AlignHCenter);
    infoLayout->setSpacing(8);
    
    auto *userInfoLabel = new QLabel(page);
    userInfoLabel->setStyleSheet("font-size:16px; color:#34495e;");
    userInfoLabel->setAlignment(Qt::AlignCenter);
    // 使用lambda更新显示的用户信息
    auto updateUserInfo = [userInfoLabel, this]() {
        QString infoText = tr("学号/工号：%1\n姓名：%2")
            .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
            .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
        userInfoLabel->setText(infoText);
    };
    updateUserInfo();
    infoLayout->addWidget(userInfoLabel);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);
    
    m_inputNewPass = new QLineEdit(page);
    m_inputConfirmPass = new QLineEdit(page);
    m_inputNewPass->setPlaceholderText(tr("请输入新密码"));
    m_inputConfirmPass->setPlaceholderText(tr("请再次输入新密码"));
    m_inputNewPass->setEchoMode(QLineEdit::Password);
    m_inputConfirmPass->setEchoMode(QLineEdit::Password);
    m_inputNewPass->setFixedWidth(300);
    m_inputConfirmPass->setFixedWidth(300);
    
    form->addRow(tr("新密码："), m_inputNewPass);
    form->addRow(tr("确认密码："), m_inputConfirmPass);

    auto *btnSubmit = new QPushButton(tr("完成注册"), page);
    btnSubmit->setFixedWidth(160);
    btnSubmit->setStyleSheet("font-size:16px; padding:10px;");
    connect(btnSubmit, &QPushButton::clicked, this, [this] {
        if (performFirstLogin()) {
            switchPage(Page::Dashboard);
        }
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addWidget(subtitle, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addLayout(infoLayout);
    layout->addSpacing(10);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createLoginPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    m_loginRoleLabel = new QLabel(tr("请输入密码"), page);
    m_loginRoleLabel->setStyleSheet("font-size:18px; font-weight:600;");
    m_loginRoleLabel->setAlignment(Qt::AlignCenter);

    // 显示用户信息（只读）
    auto *infoLayout = new QVBoxLayout();
    infoLayout->setAlignment(Qt::AlignHCenter);
    infoLayout->setSpacing(8);
    
    auto *userInfoLabel = new QLabel(page);
    userInfoLabel->setStyleSheet("font-size:16px; color:#34495e;");
    userInfoLabel->setAlignment(Qt::AlignCenter);
    // 直接显示前面输入的学号和姓名（在switchPage时更新）
    QString infoText = tr("学号/工号：%1\n姓名：%2")
        .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
        .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
    userInfoLabel->setText(infoText);
    infoLayout->addWidget(userInfoLabel);
    
    // 保存标签指针，以便在switchPage时更新
    m_loginUserInfoLabel = userInfoLabel;

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(12);
    
    // 非首次登录只需要密码输入框
    m_inputPass = new QLineEdit(page);
    m_inputPass->setEchoMode(QLineEdit::Password);
    m_inputPass->setPlaceholderText(tr("请输入密码"));
    m_inputPass->setFixedWidth(300);
    form->addRow(tr("密码："), m_inputPass);

    auto *btnLogin = new QPushButton(tr("登录"), page);
    btnLogin->setFixedWidth(160);
    connect(btnLogin, &QPushButton::clicked, this, [this] {
        // 使用临时保存的用户信息进行登录
        if (m_tempUserId.isEmpty() || m_tempUserName.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("用户信息丢失，请重新输入。"));
            switchPage(Page::UserInput);
            return;
        }

        const QString password = m_inputPass ? m_inputPass->text() : QString();
        if (password.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请输入密码"));
            return;
        }

        if (!DatabaseManager::init()) {
            QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到本地 MySQL，请检查服务是否已启动。"));
            return;
        }

        // 调用密码校验逻辑
        auto record = DatabaseManager::fetchUserByIdAndNameAndPassword(m_tempUserId, m_tempUserName, password);
        if (!record) {
            QMessageBox::warning(this, tr("登录失败"), tr("密码错误，请检查输入。"));
            return;
        }

        m_currentRole = (record->role == 1) ? Role::Staff : Role::Student;
        m_currentUser = std::make_unique<User>(record->userId, record->realName, record->credit, record->role);

        updateRoleLabel();
        updateProfileFromUser();
        QMessageBox::information(this, tr("登录成功"), tr("已从数据库加载用户信息。"));
        switchPage(Page::Dashboard);
    });

    auto *btnReset = new QPushButton(tr("修改密码"), page);
    btnReset->setFlat(true);
    connect(btnReset, &QPushButton::clicked, this, [this] {
        switchPage(Page::ResetPwd);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::UserInput);
    });

    layout->addWidget(m_loginRoleLabel, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addLayout(infoLayout);
    layout->addSpacing(10);
    layout->addLayout(form);
    layout->addWidget(btnLogin, 0, Qt::AlignCenter);
    layout->addWidget(btnReset, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createResetPwdPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    auto *title = new QLabel(tr("重置密码"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");
    title->setAlignment(Qt::AlignCenter);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setFormAlignment(Qt::AlignHCenter);
    form->setVerticalSpacing(16);

    auto *inputOld = new QLineEdit(page);
    inputOld->setPlaceholderText(tr("请输入旧密码"));
    inputOld->setEchoMode(QLineEdit::Password);
    auto *inputNew = new QLineEdit(page);
    inputNew->setPlaceholderText(tr("请输入新密码"));
    inputNew->setEchoMode(QLineEdit::Password);
    auto *inputConfirm = new QLineEdit(page);
    inputConfirm->setPlaceholderText(tr("请再次输入新密码"));
    inputConfirm->setEchoMode(QLineEdit::Password);
    inputOld->setFixedWidth(300);
    inputNew->setFixedWidth(300);
    inputConfirm->setFixedWidth(300);

    form->addRow(tr("旧密码："), inputOld);
    form->addRow(tr("新密码："), inputNew);
    form->addRow(tr("确认密码："), inputConfirm);

    auto *btnSubmit = new QPushButton(tr("提交"), page);
    btnSubmit->setFixedWidth(140);
    connect(btnSubmit, &QPushButton::clicked, this, [this, inputOld, inputNew, inputConfirm] {
        const QString oldPassword = inputOld->text();
        const QString newPassword = inputNew->text();
        const QString confirmPassword = inputConfirm->text();

        if (oldPassword.isEmpty() || newPassword.isEmpty() || confirmPassword.isEmpty()) {
            QMessageBox::warning(this, tr("提示"), tr("请填写所有密码字段"));
            return;
        }

        if (newPassword != confirmPassword) {
            QMessageBox::warning(this, tr("密码不一致"), tr("两次输入的新密码不一致，请重新输入。"));
            return;
        }

        if (newPassword.length() < 6) {
            QMessageBox::warning(this, tr("密码太短"), tr("密码长度至少为6位，请重新输入。"));
            return;
        }

        // 修改密码时采用当前待登录的账号（来自学号/姓名输入页面）
        const QString userId = m_tempUserId;
        if (userId.isEmpty()) {
            QMessageBox::warning(this, tr("错误"), tr("用户信息丢失，请返回上一页重新输入学号和姓名。"));
            switchPage(Page::UserInput);
            return;
        }

        if (!DatabaseManager::changeUserPassword(userId, oldPassword, newPassword)) {
            QMessageBox::warning(this, tr("修改失败"), tr("旧密码错误或修改失败，请重试。"));
            return;
        }

        // 修改成功后要求用户使用新密码重新登录
        if (m_inputPass) {
            m_inputPass->clear();
        }
        QMessageBox::information(this, tr("修改成功"), tr("密码已成功修改，请使用新密码重新登录。"));
        switchPage(Page::Login);
    });

    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Login);
    });

    layout->addWidget(title, 0, Qt::AlignCenter);
    layout->addLayout(form);
    layout->addWidget(btnSubmit, 0, Qt::AlignCenter);
    layout->addWidget(btnBack, 0, Qt::AlignCenter);
    return page;
}

QWidget* MainWindow::createDashboardPage()
{
    auto *page = new QWidget(this);
    auto *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(16, 16, 16, 16);
    outerLayout->setSpacing(0);

    // 顶部栏：标题和退出登录按钮
    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(0, 0, 0, 0);
    
    auto *title = new QLabel(tr("NUIST智能雨具系统"), page);
    title->setStyleSheet("font-size:22px; font-weight:700;");
    
    auto *btnLogout = new QPushButton(tr("退出登录"), page);
    btnLogout->setFixedWidth(100);
    btnLogout->setStyleSheet("font-size:14px; padding:6px 12px;");
    
    // 站点选择区域
    auto *stationLayout = new QHBoxLayout();
    stationLayout->setAlignment(Qt::AlignHCenter);
    stationLayout->setSpacing(10);
    
    auto *stationLabel = new QLabel(tr("请选择您所在的站点："), page);
    stationLabel->setStyleSheet("font-size:14px;");
    
    m_stationComboBox = new QComboBox(page);
    m_stationComboBox->setFixedWidth(200);
    m_stationComboBox->setStyleSheet("font-size:14px; padding:4px;");
    loadStations(); // 加载站点列表
    
    connect(m_stationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &MainWindow::onStationChanged);
    
    stationLayout->addWidget(stationLabel);
    stationLayout->addWidget(m_stationComboBox);
    stationLayout->addStretch();
    connect(btnLogout, &QPushButton::clicked, this, [this] {
        // 清除用户信息
        m_currentUser.reset();
        m_tempUserId.clear();
        m_tempUserName.clear();
        m_currentRole = Role::Unknown;
        m_currentStationId = 0;
        
        // 清空登录页面的输入框
        if (m_inputPass) {
            m_inputPass->clear();
        }
        if (m_inputNewPass) {
            m_inputNewPass->clear();
        }
        if (m_inputConfirmPass) {
            m_inputConfirmPass->clear();
        }
        
        // 清空站点选择
        if (m_stationComboBox) {
            m_stationComboBox->setCurrentIndex(0);
        }
        
        // 清空用户输入页面的输入框
        if (m_inputUser) {
            m_inputUser->clear();
        }
        if (m_inputName) {
            m_inputName->clear();
        }
        
        // 返回欢迎页面
        switchPage(Page::Welcome);
    });
    
    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(btnLogout);
    
    // 站点选择区域（独立一行）
    auto *stationRow = new QHBoxLayout();
    stationRow->setContentsMargins(0, 10, 0, 10);
    stationRow->addStretch();
    stationRow->addLayout(stationLayout);
    stationRow->addStretch();
    
    // 主内容区域
    auto *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(18);

    auto *btnBorrow = new QPushButton(tr("我要借伞"), page);
    auto *btnReturn = new QPushButton(tr("我要还伞"), page);
    btnBorrow->setFixedSize(200, 80);
    btnReturn->setFixedSize(200, 80);
    btnBorrow->setStyleSheet("font-size:18px;");
    btnReturn->setStyleSheet("font-size:18px;");

    connect(btnBorrow, &QPushButton::clicked, this, [this] {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        m_borrowMode = true;
        switchPage(Page::Borrow);
    });
    connect(btnReturn, &QPushButton::clicked, this, [this] {
        if (m_currentStationId == 0) {
            QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
            return;
        }
        m_borrowMode = false;
        switchPage(Page::Borrow);
    });

    auto *btnInstruction = new QPushButton(tr("使用说明"), page);
    btnInstruction->setFlat(true);
    connect(btnInstruction, &QPushButton::clicked, this, [this] {
        switchPage(Page::Instruction);
    });

    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(20, 0, 20, 0);
    bottom->setSpacing(20);
    auto *btnProfile = new QPushButton(tr("👤 个人中心"), page);
    btnProfile->setFixedWidth(150);
    auto *btnMap = new QPushButton(tr("🗺️ 查看地图"), page);
    btnMap->setFixedWidth(150);
    connect(btnProfile, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
        switchPage(Page::Profile);
    });
    connect(btnMap, &QPushButton::clicked, this, [this] {
        switchPage(Page::Map);
    });
    bottom->addWidget(btnProfile, 0, Qt::AlignLeft);
    bottom->addStretch();
    bottom->addWidget(btnMap, 0, Qt::AlignRight);

    layout->addWidget(btnBorrow, 0, Qt::AlignCenter);
    layout->addWidget(btnReturn, 0, Qt::AlignCenter);
    layout->addWidget(btnInstruction, 0, Qt::AlignCenter);
    layout->addLayout(bottom);
    
    // 组装页面布局
    outerLayout->addLayout(topBar);
    outerLayout->addLayout(stationRow);
    outerLayout->addStretch();
    outerLayout->addLayout(layout);
    outerLayout->addStretch();
    
    return page;
}

QWidget* MainWindow::createBorrowPage()
{
    auto *page = new QWidget(this);
    auto *outer = new QVBoxLayout(page);
    outer->setContentsMargins(16, 16, 16, 16);
    outer->setSpacing(12);

    auto *topBar = new QHBoxLayout();
    m_slotTitle = new QLabel(tr("借伞模式"), page);
    m_slotTitle->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    topBar->addWidget(m_slotTitle);
    topBar->addStretch();
    topBar->addWidget(btnBack);
    outer->addLayout(topBar);

    auto *grid = new QGridLayout();
    grid->setSpacing(10);

    // Create 12 slot widgets (3x4)
    for (int i = 0; i < 12; ++i) {
        auto *slot = new SlotItem(i, page);
        // 初始状态设为Empty（灰色），等待从数据库加载
        slot->setState(SlotItem::State::Empty);
        slot->setIcon(QPixmap(), QStringLiteral("#%1").arg(i + 1));
        
        connect(slot, &SlotItem::clicked, this, [this, i](int index, SlotItem::State state) {
            if (m_currentStationId == 0) {
                QMessageBox::warning(this, tr("错误"), tr("请先选择站点"));
                return;
            }
            
            if (!m_currentUser) {
                QMessageBox::warning(this, tr("错误"), tr("请先登录"));
                switchPage(Page::Dashboard);
                return;
            }
            
            int slotId = index + 1; // 槽位ID从1开始
            
            if (m_borrowMode) {
                // 借伞模式
                handleBorrowGear(slotId, index);
            } else {
                // 还伞模式
                handleReturnGear(slotId, index);
            }
        });
        m_slots.push_back(slot);
        grid->addWidget(slot, i / 4, i % 4);
    }

    outer->addLayout(grid);
    return page;
}

QWidget* MainWindow::createMapPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto *title = new QLabel(tr("地图占位（后续接入真实散点）"), page);
    title->setStyleSheet("font-size:18px; font-weight:700;");
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title, 0, Qt::AlignLeft);
    layout->addStretch();
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

QWidget* MainWindow::createProfilePage()
{
    auto *page = new QWidget(this);
    auto *top = new QVBoxLayout(page);
    top->setContentsMargins(16, 16, 16, 16);
    top->setSpacing(12);

    // 内容区居中
    auto *center = new QWidget(page);
    auto *vbox = new QVBoxLayout(center);
    vbox->setAlignment(Qt::AlignCenter); // 整体垂直水平居中
    vbox->setSpacing(12);
    m_profileTitle = new QLabel(tr("个人信息"), center);
    m_profileTitle->setStyleSheet("font-size:22px; font-weight:700;");
    m_profileName = new QLabel(center);
    m_profileId = new QLabel(center);
    m_profileBalance = new QLabel(center);
    m_profileName->setStyleSheet("font-size:18px;");
    m_profileId->setStyleSheet("font-size:16px; color:#444;");
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600;");
    vbox->addWidget(m_profileTitle, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileName, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileId, 0, Qt::AlignHCenter);
    vbox->addWidget(m_profileBalance, 0, Qt::AlignHCenter);
    vbox->addSpacing(12);
    center->setLayout(vbox);
    // 将内容区整体加到顶层layout并占据较多空间以实现视觉居中
    top->addStretch(2);
    top->addWidget(center, 0, Qt::AlignCenter);
    top->addStretch(3);

    // 底部刷新和返回按钮
    auto *bottom = new QHBoxLayout();
    bottom->setContentsMargins(0,0,0,0);
    bottom->setSpacing(0);
    auto *btnRefresh = new QPushButton(tr("刷新余额"), page);
    btnRefresh->setFixedWidth(140);
    connect(btnRefresh, &QPushButton::clicked, this, [this] {
        updateProfileFromUser();
    });
    auto *btnBack = new QPushButton(tr("返回"), page);
    btnBack->setFixedWidth(120);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });
    bottom->addWidget(btnRefresh,0,Qt::AlignLeft);
    bottom->addStretch();
    bottom->addWidget(btnBack,0,Qt::AlignRight);
    top->addLayout(bottom);
    updateProfileFromUser();
    return page;
}

QWidget* MainWindow::createInstructionPage()
{
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);

    auto *title = new QLabel(tr("使用说明 / 服务协议"), page);
    title->setStyleSheet("font-size:20px; font-weight:700;");

    m_instructionViewer = new QTextBrowser(page);
    m_instructionViewer->setOpenExternalLinks(false);
    m_instructionViewer->setHtml(
        "<h2 align=\"center\">NUIST 智能雨具系统服务协议</h2>"
        "<hr>"
        "<h3>一、 服务对象</h3>"
        "<p>本系统仅面向 NUIST 在校教职工与学生开放，登录需验证校园一卡通账户。</p>"
        "<h3>二、 借还规则</h3>"
        "<p><b>1. 借出：</b>账户余额需大于 <b>20.00元</b> 方可使用。借出时系统将冻结相应金额作为押金。</p>"
        "<p><b>2. 归还：</b>请将雨具插入任意站点的空闲卡槽，听到“咔哒”上锁声并看到屏幕提示“归还成功”后方可离开。</p>"
        "<h3>三、 资费标准 (自动扣款)</h3>"
        "<ul>"
        "<li><b>普通雨伞/一次性雨衣：</b> 押金 20元。</li>"
        "<li><b>高级抗风伞/加厚雨衣：</b> 押金 50元。</li>"
        "<li><b>免费时长：</b> 借出后 <b>24小时内</b> 归还免费。</li>"
        "<li><b>超时费用：</b> 超过24小时，按 <b>1元/12小时</b> 从余额扣除，直至扣完押金。</li>"
        "</ul>"
        "<h3>四、 遗失与损坏</h3>"
        "<p>若雨具遗失或严重损坏导致无法归还，系统将<b>扣除全额押金</b>用于赔偿。</p>"
        "<h3>五、 联系我们</h3>"
        "<p>如遇设备故障或扣费异常，请点击主页左侧的<b>【联系客服】</b>按钮，或致电校园服务中心：5873-6110。</p>"
    );

    auto *btnBack = new QPushButton(tr("我已阅读"), page);
    btnBack->setFixedWidth(140);
    connect(btnBack, &QPushButton::clicked, this, [this] {
        switchPage(Page::Dashboard);
    });

    layout->addWidget(title);
    layout->addWidget(m_instructionViewer, 1);
    layout->addWidget(btnBack, 0, Qt::AlignRight);
    return page;
}

void MainWindow::switchPage(Page page)
{
    m_stack->setCurrentIndex(static_cast<int>(page));
    if (page == Page::Borrow && m_slotTitle) {
        m_slotTitle->setText(m_borrowMode ? tr("借伞模式") : tr("还伞模式"));
        // 切换到借还页面时，如果已选择站点，刷新槽位状态
        if (m_currentStationId > 0) {
            refreshSlotsFromDatabase();
            populateSlots(m_borrowMode);
        } else {
            // 如果没有选择站点，先显示空槽位
            for (auto *slot : m_slots) {
                slot->setState(SlotItem::State::Empty);
                slot->setEnabled(false);
            }
        }
    } else if (page == Page::Login && m_loginUserInfoLabel) {
        // 切换到密码登录页面时，更新显示的用户信息
        QString infoText = tr("学号/工号：%1\n姓名：%2")
            .arg(m_tempUserId.isEmpty() ? tr("未知") : m_tempUserId)
            .arg(m_tempUserName.isEmpty() ? tr("未知") : m_tempUserName);
        m_loginUserInfoLabel->setText(infoText);
    } else if (page == Page::UserInput) {
        // 切换到用户输入页面时，清空输入框
        if (m_inputUser) {
            m_inputUser->clear();
        }
        if (m_inputName) {
            m_inputName->clear();
        }
    }
}

void MainWindow::populateSlots(bool borrowMode)
{
    // 如果未选择站点，提示用户
    if (m_currentStationId == 0) {
        QMessageBox::warning(this, tr("提示"), tr("请先在主页面选择站点"));
        switchPage(Page::Dashboard);
        return;
    }
    
    // 从数据库刷新槽位状态（颜色和类型完全由 refreshSlotsFromDatabase 控制）
    refreshSlotsFromDatabase();
    // 不再通过 setEnabled(true/false) 灰掉控件，否则图标会显得"糊"；
    // 借伞/还伞模式的可点击逻辑在 handleBorrowGear / handleReturnGear 中根据状态再做检查。
    
    // 确保UI立即更新
    QApplication::processEvents();
}

void MainWindow::updateRoleLabel()
{
    if (!m_loginRoleLabel) return;
    QString roleText = tr("请选择身份");
    if (m_currentRole == Role::Student) roleText = tr("学生登录");
    else if (m_currentRole == Role::Staff) roleText = tr("教职人员登录");
    m_loginRoleLabel->setText(roleText);
}

void MainWindow::updateProfileFromUser()
{
    if (!m_profileName || !m_profileId || !m_profileBalance || !m_profileTitle) return;

    if (!m_currentUser) {
        m_profileTitle->setText(tr("个人信息"));
        m_profileName->setText(tr("姓名：-"));
        m_profileId->setText(tr("账号：-"));
        m_profileBalance->setText(tr("账户余额：￥0.00"));
        m_profileBalance->setStyleSheet("font-size:18px; font-weight:600; color:#7f8c8d;");
        return;
    }

    const bool isStaff = m_currentUser->get_role() == 1;
    m_profileTitle->setText(tr("个人信息"));
    m_profileName->setText(tr("姓名：%1").arg(m_currentUser->get_name()));
    m_profileId->setText(isStaff ? tr("工号：%1").arg(m_currentUser->get_id())
                                 : tr("学号：%1").arg(m_currentUser->get_id()));
    m_profileBalance->setText(tr("账户余额：￥%1").arg(QString::number(m_currentUser->get_balance(), 'f', 2)));
    m_profileBalance->setStyleSheet("font-size:18px; font-weight:600; color:#2ecc71;");
}

bool MainWindow::performUserInput()
{
    // 这个函数现在不再使用，逻辑已移到createUserInputPage的lambda中
    // 保留此函数以防其他地方调用
    QMessageBox::warning(this, tr("错误"), tr("此函数已废弃，请使用页面内的提交逻辑"));
    return false;
}

bool MainWindow::performFirstLogin()
{
    const QString newPassword = m_inputNewPass ? m_inputNewPass->text() : QString();
    const QString confirmPassword = m_inputConfirmPass ? m_inputConfirmPass->text() : QString();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入新密码和确认密码"));
        return false;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, tr("密码不一致"), tr("两次输入的密码不一致，请重新输入。"));
        return false;
    }

    if (newPassword.length() < 6) {
        QMessageBox::warning(this, tr("密码太短"), tr("密码长度至少为6位，请重新输入。"));
        return false;
    }

    if (m_tempUserId.isEmpty()) {
        QMessageBox::critical(this, tr("错误"), tr("用户信息丢失，请重新输入。"));
        return false;
    }

    // 更新密码并激活账户
    if (!DatabaseManager::updateUserPassword(m_tempUserId, newPassword)) {
        QMessageBox::critical(this, tr("设置失败"), tr("密码设置失败，请重试。"));
        return false;
    }

    // 重新查询用户信息
    auto record = DatabaseManager::fetchUserByIdAndName(m_tempUserId, m_tempUserName);
    if (!record) {
        QMessageBox::critical(this, tr("错误"), tr("无法加载用户信息。"));
        return false;
    }

    // 创建用户对象
    m_currentUser = std::make_unique<User>(record->userId, record->realName, record->credit, record->role);
    updateProfileFromUser();

    QMessageBox::information(this, tr("注册成功"), tr("密码设置成功，账户已激活！"));
    return true;
}

bool MainWindow::performLogin()
{
    // 这个函数现在不再使用，登录逻辑已移到createLoginPage的lambda中
    // 保留此函数以防其他地方调用
    QMessageBox::warning(this, tr("错误"), tr("此函数已废弃，请使用页面内的登录逻辑"));
    return false;
}

void MainWindow::loadStations()
{
    if (!m_stationComboBox) return;
    
    m_stationComboBox->clear();
    m_stationComboBox->addItem(tr("-- 请选择站点 --"), 0);
    
    if (!DatabaseManager::init()) {
        qWarning() << "[MainWindow] 数据库连接失败，无法加载站点";
        return;
    }
    
    auto stations = DatabaseManager::fetchAllStations();
    for (const auto &station : stations) {
        m_stationComboBox->addItem(station.name, station.stationId);
    }
    
    qDebug() << "[MainWindow] 已加载" << stations.size() << "个站点";
}

void MainWindow::onStationChanged(int index)
{
    if (!m_stationComboBox || index < 0) return;
    
    int stationId = m_stationComboBox->itemData(index).toInt();
    if (stationId == 0) {
        m_currentStationId = 0;
        return;
    }
    
    m_currentStationId = stationId;
    qDebug() << "[MainWindow] 站点已切换为:" << m_stationComboBox->itemText(index) << "ID:" << stationId;
    
    // 如果当前在借还页面，刷新槽位状态
    if (m_stack && m_stack->currentIndex() == static_cast<int>(Page::Borrow)) {
        refreshSlotsFromDatabase();
    }
}

void MainWindow::refreshSlotsFromDatabase()
{
    if (m_currentStationId == 0) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择站点"));
        return;
    }
    
    if (!DatabaseManager::init()) {
        QMessageBox::critical(this, tr("数据库错误"), tr("无法连接到数据库"));
        return;
    }
    
    // 从数据库查询该站点的所有雨具
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    
    // 创建槽位映射：slot_id -> gear
    QMap<int, DatabaseManager::GearRecord> slotMap;
    for (const auto &gear : gears) {
        if (gear.slotId >= 1 && gear.slotId <= 12) {
            slotMap[gear.slotId] = gear;
        }
    }
    
    // 固定雨具类型分配：1-4普通塑料伞，5-8高质量抗风伞，9-10专用遮阳伞，11-12雨衣
    static const QMap<int, std::pair<GearType, QString>> slotTypeMap = {
        {1, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {2, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {3, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {4, {GearType::StandardPlastic, tr("普通塑料伞")}},
        {5, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {6, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {7, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {8, {GearType::PremiumWindproof, tr("高质量抗风伞")}},
        {9, {GearType::Sunshade, tr("专用遮阳伞")}},
        {10, {GearType::Sunshade, tr("专用遮阳伞")}},
        {11, {GearType::Raincoat, tr("雨衣")}},
        {12, {GearType::Raincoat, tr("雨衣")}}
    };
    
    // 使用RainGearFactory创建雨具对象并设置到槽位
    for (int i = 0; i < m_slots.size() && i < 12; ++i) {
        int slotId = i + 1; // 槽位ID从1开始
        auto *slot = m_slots[i];
        
        // 获取该槽位的固定类型
        auto typeInfo = slotTypeMap.value(slotId);
        GearType expectedType = typeInfo.first;
        QString typeName = typeInfo.second;
        
        // 创建对应类型的雨具对象（用于获取图标）
        QString gearId = slotMap.contains(slotId) ? slotMap[slotId].gearId : QStringLiteral("PLACEHOLDER_%1").arg(slotId);
        auto rainGear = RainGearFactory::create_raingear(expectedType, gearId);
        
        if (rainGear) {
            // 设置图标（直接加载，不缩放）
            QString iconPath = rainGear->get_iconpath();
            QPixmap icon(iconPath);
            if (icon.isNull()) {
                qWarning() << "[MainWindow] 无法加载图标:" << iconPath;
            } else {
                // 确保图标清晰，不进行任何缩放处理
                qDebug() << "[MainWindow] 加载图标:" << iconPath << "原始尺寸:" << icon.size();
            }
            QString label = QStringLiteral("#%1").arg(slotId);
            slot->setIcon(icon, label);
            slot->setGearTypeName(typeName); // 设置雨具类型名称
            
            // 根据数据库状态设置颜色
            if (slotMap.contains(slotId)) {
                const auto &gear = slotMap[slotId];
                if (gear.status == 1) {
                    // 可借状态 - 绿色
                    slot->setState(SlotItem::State::Available);
                } else if (gear.status == 2) {
                    // 已借出 - 灰色（空槽）
                    slot->setState(SlotItem::State::Empty);
                } else if (gear.status == 3) {
                    // 损坏 - 红色
                    slot->setState(SlotItem::State::Maintenance);
                }
            } else {
                // 该槽位没有雨具 - 灰色（空槽）
                slot->setState(SlotItem::State::Empty);
            }
        } else {
            // 无法创建雨具对象
            qWarning() << "[MainWindow] 无法创建雨具对象，slotId:" << slotId;
            slot->setState(SlotItem::State::Empty);
            slot->setIcon(QPixmap(), QStringLiteral("#%1").arg(slotId));
            slot->setGearTypeName(typeName);
        }
        
        // 确保样式已应用并立即刷新
        slot->setEnabled(true);
        slot->repaint();
        slot->update();
    }
    
    qDebug() << "[MainWindow] 已刷新站点" << m_currentStationId << "的槽位状态，共" << gears.size() << "个雨具";
}

void MainWindow::handleBorrowGear(int slotId, int slotIndex)
{
    if (!m_currentUser || m_currentStationId == 0) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录并选择站点"));
        return;
    }
    
    // 查询该槽位的雨具
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    QString gearId;
    int typeId = 0;
    
    for (const auto &gear : gears) {
        if (gear.slotId == slotId && gear.status == 1) {
            gearId = gear.gearId;
            typeId = gear.typeId;
            break;
        }
    }
    
    if (gearId.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("该槽位没有可借的雨具"));
        return;
    }
    
    // 检查用户是否已有借出的雨具
    auto currentBorrow = DatabaseManager::fetchUserCurrentBorrow(m_currentUser->get_id());
    if (currentBorrow) {
        QMessageBox::warning(this, tr("提示"), tr("您还有未归还的雨具，请先归还后再借。"));
        return;
    }
    
    // 计算押金
    double deposit = 0.0;
    switch (typeId) {
        case 1: deposit = 10.0; break;  // 普通塑料伞
        case 2: deposit = 20.0; break;  // 高质量抗风伞
        case 3: deposit = 15.0; break; // 遮阳伞
        case 4: deposit = 25.0; break; // 雨衣
        default: deposit = 20.0; break;
    }
    
    // 检查余额
    if (m_currentUser->get_credit() < deposit) {
        QMessageBox::warning(this, tr("余额不足"), 
            tr("您的余额不足，需要押金 %1 元，当前余额 %2 元").arg(deposit).arg(m_currentUser->get_credit()));
        return;
    }
    
    // 确认对话框
    int ret = QMessageBox::question(this, tr("确认借伞"), 
        tr("确认借出该雨具？\n押金：%1 元\n当前余额：%2 元").arg(deposit).arg(m_currentUser->get_credit()),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // 执行借伞操作
    if (!DatabaseManager::borrowGear(m_currentUser->get_id(), gearId, m_currentStationId, slotId, deposit)) {
        QMessageBox::critical(this, tr("借伞失败"), tr("借伞操作失败，请重试或联系管理员。"));
        return;
    }
    
    // 更新用户余额显示
    m_currentUser->deduct(deposit);
    updateProfileFromUser();
    
    // 刷新槽位状态（借伞后，该槽位变为空，应该显示灰色）
    refreshSlotsFromDatabase();
    
    // 强制刷新UI，确保样式立即更新
    for (auto *slot : m_slots) {
        slot->repaint();
        slot->update();
    }
    QApplication::processEvents(); // 处理事件循环，确保UI立即更新
    
    QMessageBox::information(this, tr("借伞成功"), tr("雨具已成功借出！\n押金：%1 元已扣除").arg(deposit));
}

void MainWindow::handleReturnGear(int slotId, int slotIndex)
{
    if (!m_currentUser || m_currentStationId == 0) {
        QMessageBox::warning(this, tr("错误"), tr("请先登录并选择站点"));
        return;
    }
    
    // 检查用户是否有借出的雨具
    auto currentBorrow = DatabaseManager::fetchUserCurrentBorrow(m_currentUser->get_id());
    if (!currentBorrow) {
        QMessageBox::warning(this, tr("提示"), tr("您当前没有借出的雨具"));
        return;
    }
    
    QString gearId = currentBorrow->gearId;
    
    // 检查该槽位是否为空（槽位为空表示可以还伞）
    // 注意：fetchGearsByStation只返回该站点中存在的雨具，如果槽位是空的，不会出现在结果中
    auto gears = DatabaseManager::fetchGearsByStation(m_currentStationId);
    bool slotOccupied = false;
    for (const auto &gear : gears) {
        if (gear.slotId == slotId) {
            // 如果槽位有雨具，且状态为可借（status==1），说明槽位被占用
            if (gear.status == 1) {
                slotOccupied = true;
                break;
            }
            // 如果状态是其他（损坏等），也认为被占用
            if (gear.status == 3) {
                slotOccupied = true;
                break;
            }
        }
    }
    
    if (slotOccupied) {
        QMessageBox::warning(this, tr("提示"), tr("该槽位已被占用，请选择其他空槽位"));
        return;
    }
    
    // 确认对话框
    int ret = QMessageBox::question(this, tr("确认还伞"), 
        tr("确认将雨具归还到槽位 #%1？").arg(slotId),
        QMessageBox::Yes | QMessageBox::No);
    
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // 执行还伞操作
    auto [success, cost] = DatabaseManager::returnGear(m_currentUser->get_id(), gearId, m_currentStationId, slotId);
    
    if (!success) {
        QMessageBox::critical(this, tr("还伞失败"), tr("还伞操作失败，请重试或联系管理员。"));
        return;
    }
    
    // 更新用户余额显示（押金已退还，扣除费用）
    // 注意：returnGear已经处理了余额更新，这里需要重新从数据库加载用户信息
    auto userRecord = DatabaseManager::fetchUserByIdAndName(m_currentUser->get_id(), m_currentUser->get_name());
    if (userRecord) {
        m_currentUser->recharge(userRecord->credit - m_currentUser->get_credit());
    }
    updateProfileFromUser();
    
    // 刷新槽位状态（还伞后，该槽位变为有雨具，应该显示绿色）
    refreshSlotsFromDatabase();
    
    // 强制刷新UI，确保样式立即更新
    for (auto *slot : m_slots) {
        slot->repaint();
        slot->update();
    }
    QApplication::processEvents(); // 处理事件循环，确保UI立即更新
    
    QString msg = tr("雨具已成功归还！");
    if (cost > 0) {
        msg += tr("\n使用费用：%1 元").arg(cost);
    } else {
        msg += tr("\n24小时内免费，已全额退还押金");
    }
    QMessageBox::information(this, tr("还伞成功"), msg);
}

