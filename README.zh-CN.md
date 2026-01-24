# ☔ Campus RainHub

[English](README.md) | [中文](README.zh-CN.md)

> **"No more being trapped in buildings on rainy days."**
>
> **"下雨天，再也不用被困在教学楼了。"**

### 项目简介

**Campus RainHub (校园智能雨具共享系统)** 是一套基于 **C++17** 和 **Qt 6.9.3** 开发的校园物联网解决方案，底层数据存储采用 **MySQL 8.0**。

本项目包含两个独立的应用端：

1.  **用户自助终端 (Client)**：部署在教学楼门口，支持多终端并发登录，供学生自助借还。
2.  **综合管理后台 (Admin)**：供运营人员进行雨具调度、站点监控和数据统计。

项目旨在解决高校师生突遇降雨时的出行痛点，通过类似“共享充电宝”的模式，实现校园雨具的 **24小时无人值守** 借还服务。

---

### 系统功能与特性

系统核心逻辑是在校园高频活动区设置智能终端站点，学生通过刷卡（模拟校园一卡通）进行交互。每个站点硬件设计包含 **12个智能槽位**，通过算法自动分配归还位置。

#### 用户端/User Terminal

* **多类型雨具支持**：系统通过工厂模式管理多种雨具，包括普通塑料伞、抗风伞、专用遮阳伞及雨衣。
* **可视化地图**：实时渲染校园地图，动态显示各站点位置及当前剩余库存（可用/空槽）。

#### 管理端/Management Dashboard

* **全局监控**：实时监控所有站点的在线状态与库存水位，辅助调度决策。
* **雨具生命周期管理**：支持雨具的状态变更。
* **数据统计**：可视化展示订单流水与用户活跃度，生成运营报表。

---

### 架构设计与技术实现

本项目采用经典的 **分层架构**，并针对性能与并发进行了深度的优化设计：

- **表现层 (UI Layer)**：使用 Qt Widgets 构建，通过 QSS 实现深色系现代化界面。
- **业务逻辑层 (Service Layer)**：封装核心业务（如 `AuthService`, `BorrowService`），负责业务流程控制。
- **数据访问层 (DAO Layer)**：通过 **DAO 模式** 将 SQL 操作与业务逻辑分离，确保数据持久化的独立性。
- **通用设施层 (Utils Layer)**：
  - **线程安全数据库管理**：实现了基于 **Thread-Local (线程局部存储)** 机制的 `ConnectionPool`。系统根据线程 ID 动态生成独立的连接实例，并通过 `getThreadLocalConnection` 确保每个线程拥有独立的数据库上下文，完美解决了 Qt 数据库模块在多线程环境下的并发冲突问题。
  - **混合数据加载策略**：设计了 `MapConfigLoader` 配置加载器。采用 **"静态配置 + 动态数据"** 的混合加载模式——站点坐标、名称等静态数据从本地 JSON 资源 (`:/map/map_config.json`) 毫秒级预加载，而库存水位等动态数据从数据库实时拉取。这种策略在保证数据实时性的同时，极大地降低了数据库 I/O 压力，提升了地图渲染性能。

---

### 项目结构

```text
Rainhub/
├── sql/                    # 数据库脚本
│   ├── init_db.sql         # 包含建库、建表语句
│   └── data_insert.sql     # 初始化测试数据（站点、雨具、测试用户）
├── src/
│   ├── admin_ui/           # 管理端界面逻辑（UI交互与槽函数）
│   ├── client_ui/          # 用户端界面逻辑（含自定义控件与动画）
│   ├── control/            # 业务逻辑层（Service），处理借还算法与鉴权
│   ├── dao/                # 数据访问层（DAO），封装所有 SQL 操作
│   ├── model/              # 数据实体类（User, RainGear等）
│   └── utils/              # 工具类（线程级数据库连接池/静态站点数据加载类）
├── assets/                 # 静态资源（图标、地图JSON配置、QSS样式表）
├── third_party/            # 第三方依赖（MySQL Connector/C++ 动态库）
└── CMakeLists.txt          # CMake 构建脚本
```



### 运行指南

#### 1. 环境准备

- **编译器**：MinGW 11.2+ 或 MSVC 2019+ (支持 C++17)
- **框架**：Qt 6.x (已测试 6.9.3)
- **数据库**：MySQL 8.0+
- **构建工具**：CMake 3.16+

#### 2. 数据库配置

1. 进入 `sql` 目录，通过数据库管理工具（如 Navicat 或 MySQL Workbench）运行脚本：

   - 先运行 `init_db.sql`：会自动创建 `rainhub_db` 数据库及所有表结构。
   - 再运行 `data_insert.sql`：导入默认的站点和测试数据。

2. 打开 `src/utils/ConnectionPool.cpp`，修改连接配置（第18-19行）：

   ```c++
   db.setUserName("your_username"); // 替换为你的 MySQL 用户名
   db.setPassword("your_password"); // 替换为你的 MySQL 密码
   ```

#### 3. 编译与构建

本项目使用 **CMake** 进行跨平台构建。请确保系统已安装 CMake 且已配置好 Qt 环境变量。

```
# 1. 克隆或下载项目后，进入项目根目录
cd Rainhub

# 2. 创建并进入构建目录（推荐 Out-of-source build，保持源码整洁）
mkdir build && cd build

# 3. 生成构建文件 (Makefiles 或 VS Solution)
cmake ..

# 4. 开始编译 (Release模式)
cmake --build . --config Release
```

#### 4. 运行

编译完成后，可执行文件将生成在 `build/bin` (或 `build/Release`) 目录下：

- **用户端**：运行 `RainHub.exe`
- **管理端**：运行 `RainHub_Admin.exe`

------

如果觉得这个项目有意思，欢迎 Star ⭐

