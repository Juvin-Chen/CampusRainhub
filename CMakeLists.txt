cmake_minimum_required(VERSION 3.16)
project(RainHub VERSION 1.0.0 LANGUAGES CXX)

# 基础设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC特定设置
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /utf-8)
endif()

# MinGW UTF-8支持
if(MINGW)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Qt 配置 - 可通过命令行参数覆盖
if(NOT DEFINED CMAKE_PREFIX_PATH)
    if(EXISTS "E:/Qt/6.9.3/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/Qt/6.9.3/mingw_64" CACHE PATH "Qt安装路径")
    elseif(EXISTS "E:/Qt/6.9.0/mingw_64")
        set(CMAKE_PREFIX_PATH "E:/Qt/6.9.0/mingw_64" CACHE PATH "Qt安装路径")
    elseif(EXISTS "C:/Qt/6.9.0/mingw_64")
        set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64" CACHE PATH "Qt安装路径")
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network)

message(STATUS "Found Qt${QT_VERSION_MAJOR} at: ${Qt${QT_VERSION_MAJOR}_DIR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# MySQL 配置
if(NOT DEFINED MYSQL_ROOT)
    if(EXISTS "C:/Program Files/MySQL/MySQL Server 8.0")
        set(MYSQL_ROOT "C:/Program Files/MySQL/MySQL Server 8.0" CACHE PATH "MySQL安装路径")
    elseif(EXISTS "C:/Program Files/MySQL/MySQL Server 8.4")
        set(MYSQL_ROOT "C:/Program Files/MySQL/MySQL Server 8.4" CACHE PATH "MySQL安装路径")
    endif()
endif()

set(MYSQL_LIB_DIR "${MYSQL_ROOT}/lib")
set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(MYSQL_DLL_DIR "${THIRD_PARTY_DIR}/mysql/lib")
set(MYSQL_SQLDRIVERS_DIR "${THIRD_PARTY_DIR}/mysql/sqldrivers")

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/client_ui
    ${CMAKE_SOURCE_DIR}/src/admin_ui
    ${CMAKE_SOURCE_DIR}/src/Model
    ${CMAKE_SOURCE_DIR}/src/control
    ${CMAKE_SOURCE_DIR}/src/dao
    ${CMAKE_SOURCE_DIR}/src/utils
)

# 源文件定义

# 共享的 Model 层
set(MODEL_SOURCES
    src/Model/User.cpp
    src/Model/Stationlocal.cpp
    src/Model/StationUtils.cpp
)

# 共享的 DAO 层
set(DAO_SOURCES
    src/dao/UserDao.cpp
    src/dao/GearDao.cpp
    src/dao/StationDao.cpp
    src/dao/RecordDao.cpp
)

# 共享的 Control/Service 层（客户端）
set(CLIENT_SERVICE_SOURCES
    src/control/AuthService.cpp
    src/control/BorrowService.cpp
    src/control/StationService.cpp
)

# 管理员后台 Service 层
set(ADMIN_SERVICE_SOURCES
    src/control/Admin_AuthService.cpp
    src/control/Admin_StationService.cpp
    src/control/Admin_GearService.cpp
    src/control/Admin_UserService.cpp
    src/control/Admin_OrderService.cpp
)

# 共享的 Utils 层
set(UTILS_SOURCES
    src/utils/ConnectionPool.cpp
)

# 客户端 UI 层
set(CLIENT_UI_SOURCES
    src/client_ui/main.cpp
    src/client_ui/MainWindow.cpp
    src/client_ui/components/SlotItem.cpp
    src/client_ui/pages/WelcomePage.cpp
    src/client_ui/pages/AuthPages.cpp
    src/client_ui/pages/DashboardPage.cpp
    src/client_ui/pages/BorrowPage.cpp
    src/client_ui/pages/MapPage.cpp
    src/client_ui/pages/ProfilePage.cpp
    src/client_ui/pages/InstructionPage.cpp
)

# 客户端头文件
set(CLIENT_HEADERS
    src/client_ui/MainWindow.h
    src/client_ui/assets/Styles.h
    src/client_ui/components/SlotItem.h
    src/client_ui/pages/WelcomePage.h
    src/client_ui/pages/AuthPages.h
    src/client_ui/pages/DashboardPage.h
    src/client_ui/pages/BorrowPage.h
    src/client_ui/pages/MapPage.h
    src/client_ui/pages/ProfilePage.h
    src/client_ui/pages/InstructionPage.h
)

# 管理员后台 UI 层
set(ADMIN_UI_SOURCES
    src/admin_ui/main.cpp
    src/admin_ui/MainWindow.cpp
)

# 管理员后台头文件
set(ADMIN_HEADERS
    src/admin_ui/MainWindow.h
)

# 资源文件（统一放在 assets 目录）
set(RESOURCES
    assets/resources.qrc
)

# 创建可执行文件

# 客户端
add_executable(${PROJECT_NAME}
    ${CLIENT_UI_SOURCES}
    ${MODEL_SOURCES}
    ${DAO_SOURCES}
    ${CLIENT_SERVICE_SOURCES}
    ${UTILS_SOURCES}
    ${CLIENT_HEADERS}
    ${RESOURCES}
)

# 管理员后台
add_executable(${PROJECT_NAME}_Admin
    ${ADMIN_UI_SOURCES}
    ${MODEL_SOURCES}
    ${DAO_SOURCES}
    ${ADMIN_SERVICE_SOURCES}
    ${UTILS_SOURCES}
    ${ADMIN_HEADERS}
    ${RESOURCES}
)

# 链接库

# 客户端链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# 管理员后台链接Qt库
target_link_libraries(${PROJECT_NAME}_Admin
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# Windows特定配置
if(WIN32)
    if(EXISTS "${MYSQL_LIB_DIR}")
        target_link_directories(${PROJECT_NAME} PRIVATE ${MYSQL_LIB_DIR})
        target_link_directories(${PROJECT_NAME}_Admin PRIVATE ${MYSQL_LIB_DIR})
        target_link_libraries(${PROJECT_NAME} libmysql)
        target_link_libraries(${PROJECT_NAME}_Admin libmysql)
    endif()
endif()

# 输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(${PROJECT_NAME}_Admin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# DLL 复制（Windows）
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake LOCATION)
    if(QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
        get_filename_component(QT_PREFIX_DIR ${QT_BIN_DIR} DIRECTORY)
    endif()

    set(MYSQL_DLLS
        "${MYSQL_DLL_DIR}/libmysql.dll"
        "${MYSQL_DLL_DIR}/libssl-3-x64.dll"
        "${MYSQL_DLL_DIR}/libcrypto-3-x64.dll"
    )
    
    set(QT_DLLS
        "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Core.dll"
        "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Gui.dll"
        "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Widgets.dll"
        "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Sql.dll"
        "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Network.dll"
    )

    # 定义一个宏来复制DLL到目标
    macro(copy_dlls_to_target TARGET_NAME)
        # 复制MySQL DLL
        foreach(DLL ${MYSQL_DLLS})
            if(EXISTS "${DLL}")
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" $<TARGET_FILE_DIR:${TARGET_NAME}>
                )
            endif()
        endforeach()

        # 复制Qt DLL
        foreach(DLL ${QT_DLLS})
            if(EXISTS "${DLL}")
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" $<TARGET_FILE_DIR:${TARGET_NAME}>
                )
            endif()
        endforeach()

        # 复制Qt平台插件
        set(QT_PLATFORM_PLUGIN "${QT_PREFIX_DIR}/plugins/platforms/qwindows.dll")
        if(EXISTS "${QT_PLATFORM_PLUGIN}")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/platforms
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT_PLATFORM_PLUGIN}" $<TARGET_FILE_DIR:${TARGET_NAME}>/platforms
            )
        endif()

        # 复制MySQL驱动插件
        set(MYSQL_DRIVER_DLL "${MYSQL_SQLDRIVERS_DIR}/qsqlmysql.dll")
        if(EXISTS "${MYSQL_DRIVER_DLL}")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MYSQL_DRIVER_DLL}" $<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers
            )
            foreach(DLL ${MYSQL_DLLS})
                if(EXISTS "${DLL}")
                    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL}" $<TARGET_FILE_DIR:${TARGET_NAME}>/sqldrivers
                    )
                endif()
            endforeach()
        endif()

        # MinGW运行时库
        if(MINGW AND CMAKE_CXX_COMPILER)
            get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
            set(MINGW_RUNTIME_LIBS
                "${COMPILER_DIR}/libgcc_s_seh-1.dll"
                "${COMPILER_DIR}/libstdc++-6.dll"
                "${COMPILER_DIR}/libwinpthread-1.dll"
            )
            foreach(LIB ${MINGW_RUNTIME_LIBS})
                if(EXISTS "${LIB}")
                    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIB}" $<TARGET_FILE_DIR:${TARGET_NAME}>
                    )
                endif()
            endforeach()
        endif()
    endmacro()

    # 为两个目标复制DLL
    copy_dlls_to_target(${PROJECT_NAME})
    copy_dlls_to_target(${PROJECT_NAME}_Admin)
endif()

# 输出配置信息
message(STATUS "RainHub Build Configuration")
message(STATUS "Qt Version: ${QT_VERSION_MAJOR}")
message(STATUS "Qt Path: ${CMAKE_PREFIX_PATH}")
message(STATUS "MySQL Root: ${MYSQL_ROOT}")
message(STATUS "Third Party: ${THIRD_PARTY_DIR}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Targets: ${PROJECT_NAME}, ${PROJECT_NAME}_Admin")
