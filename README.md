# ☔ Campus RainHub

[English](README.md) | [中文](README.zh-CN.md)

> **"No more being trapped in buildings on rainy days."**

### Project Introduction

**Campus RainHub** aims to **reshape campus micro-mobility**. It is a shared rain gear service platform integrating intelligent user terminal interaction, real-time backend monitoring, and data visualization for decision-making.

Developed using **C++17** and **Qt 6.9.3**, **built with CMake**, and relying on **MySQL 8.0** for data storage, it is an IoT solution tailored specifically for campus scenarios.

The project consists of two independent applications:

1.  **User Self-Service Terminal (Client)**: Deployed at building entrances, supporting concurrent logins on multiple terminals for students to borrow and return gear independently.
2.  **Comprehensive Management Dashboard (Admin)**: Used by operations staff for rain gear scheduling, station monitoring, and data statistics.

The project aims to solve the pain point of sudden rain on campus by adopting a "shared economy" model similar to power bank rental stations, providing **24-hour unattended** rain gear services.

---

### Features & Functionality

The core logic involves placing intelligent terminal stations in high-traffic campus areas. Students interact with the system via RFID cards (simulating campus ID cards). Each station hardware is designed with **12 smart slots**, using algorithms to automatically assign return positions.

#### User Terminal

- **Multi-type Rain Gear Support**: The system manages various types of rain gear through the Factory Pattern, including ordinary plastic umbrellas, windproof umbrellas, dedicated sunshades, and raincoats.
- **Visualized Map**: Real-time rendering of the campus map, dynamically displaying the location of each station and current remaining inventory (available/empty slots).
- **Built-in User Guide**: Explains borrowing/returning rules and fee standards, helping new users quickly get started with using the self-service terminal.

#### Management Dashboard

- **Global Monitoring**: Real-time monitoring of the online status and inventory levels of all stations, assisting in scheduling decisions.
- **Station and Rain Gear Lifecycle Management**: Supports status changes for stations and rain gear.
- **User Account and Security Management**
  - **Status Audit**: Supports administrators in viewing the **activation status** of user accounts (activated/pending).
  - **Emergency Security Operations**: Provides an administrator-level **password reset interface**. When a user forgets their password or the account shows anomalies, administrators can manually modify the user password in the backend, ensuring account controllability and security.
- **Data Statistics**: Visual display of order transaction history.

---

### Architecture & Technical Implementation

The project adopts a classic **Layered Architecture**, optimized deeply for performance and concurrency:

* **UI Layer**: Built with **Qt Widgets**, featuring a modern dark-themed interface implemented via **QSS**.
* **Service Layer**: Encapsulates core business logic (e.g., `AuthService`, `BorrowService`) and handles workflow control.
* **DAO Layer**: Separates SQL operations from business logic using the **DAO Pattern**, ensuring the independence of data persistence.
* **Utils Layer**:
  * **Thread-Safe Database Management**: Implemented a `ConnectionPool` based on the **Thread-Local Storage (TLS)** mechanism. The system dynamically generates independent connection instances based on Thread IDs. The `getThreadLocalConnection` method ensures each thread possesses its own database context, perfectly resolving concurrency conflicts within the Qt SQL module in multi-threaded environments.
  * **Hybrid Data Loading Strategy**: Designed a `MapConfigLoader`. Adopts a **"Static Configuration + Dynamic Data"** hybrid mode—static data like station coordinates and names are pre-loaded from local JSON resources (`:/map/map_config.json`) in milliseconds, while dynamic data (inventory levels) is fetched from the database in real-time. This strategy guarantees real-time accuracy while significantly reducing database I/O overhead and improving map rendering performance.

---

### Project Structure

```text
Rainhub/
├── sql/                    # Database Scripts
│   ├── init_db.sql         # Schema creation script
│   └── data_insert.sql     # Initial test data (stations, gear, test users)
├── src/
│   ├── admin_ui/           # Admin UI Logic (Interactions & Slots)
│   ├── client_ui/          # Client UI Logic (Custom Widgets & Animations)
│   ├── control/            # Service Layer (Business Logic, Auth, Algo)
│   ├── dao/                # Data Access Object Layer (SQL Encapsulation)
│   ├── model/              # Data Entities (POJOs: User, RainGear, etc.)
│   └── utils/              # Utilities (Thread-Local DB Pool, Map Loader)
├── assets/                 # Static Resources (Icons, Map JSON)
├── third_party/            # Dependencies (MySQL Connector/C++ DLLs)
└── CMakeLists.txt          # CMake Build Script
```



### Run Guide

#### 1. Prerequisites

- **Compiler**: MinGW 11.2+ or MSVC 2019+ (Must support **C++17**)
- **Framework**: Qt 6.x (Tested on **6.9.3**)
- **Database**: MySQL 8.0+
- **Build Tool**: CMake 3.16+

#### 2. Database Configuration

1. Navigate to the `sql` directory and run the scripts using a DB tool (e.g., Navicat or MySQL Workbench):

   - First, run `init_db.sql` to create the `rainhub_db` database and tables.
   - Then, run `data_insert.sql` to import default stations and test data.

2. Open `src/utils/ConnectionPool.cpp` and update the connection details (lines 18-19):

   C++

   ```
   db.setUserName("your_username"); // TODO: Replace with your MySQL username
   db.setPassword("your_password"); // TODO: Replace with your MySQL password
   ```

#### 3. Build & Compile

This project uses **CMake** for cross-platform building. Ensure CMake is installed and the Qt environment variables are set.

Bash

```
# 1. Clone or download the project, then enter the root directory
cd Rainhub

# 2. Create and enter the build directory (Out-of-source build recommended)
mkdir build && cd build

# 3. Generate build files (Makefiles or VS Solution)
cmake ..

# 4. Compile (Release mode)
cmake --build . --config Release
```

#### 4. Run

After compilation, the executables will be generated in `build/bin` (or `build/Release`):

- **Client App**: Run `RainHub.exe`
- **Admin App**: Run `RainHub_Admin.exe`

------

> If you find this project interesting, please **Star** ⭐
