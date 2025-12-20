cmake_minimum_required(VERSION 3.16)
project(RainHub VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC特定设置：需要/Zc:__cplusplus和/permissive-选项
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

# 设置CMake路径（如果未设置）
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "E:/Qt/6.9.3/mingw_64" CACHE PATH "Qt安装路径")
endif()

# 查找Qt6（如果找不到则尝试Qt5）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network)

# 设置Qt的MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/Control
    ${CMAKE_CURRENT_SOURCE_DIR}/client
)

# 客户端源文件
set(CLIENT_SOURCES
    client/main.cpp
    client/MainWindow.cpp
    client/SlotItem.cpp
    Model/User.cpp
    Model/Stationlocal.cpp
    Model/StationUtils.cpp
    Control/SystemManger.cpp
    Control/DatabaseManager.cpp
)

# 管理员后台源文件
set(ADMIN_SOURCES
    admin/main.cpp
    admin/AdminMainWindow.cpp
    Model/User.cpp
    Model/Stationlocal.cpp
    Model/StationUtils.cpp
    Control/DatabaseManager.cpp
)

# 客户端头文件
set(CLIENT_HEADERS
    client/MainWindow.h
    client/SlotItem.h
    Model/User.h
    Model/RainGear.hpp
    Model/RainGear_subclasses.hpp
    Model/RainGearFactory.h
    Model/GlobalEnum.hpp
    Model/Stationlocal.h
    Model/StationUtils.h
    Control/DBHelper.h
    Control/RainGearDAO.hpp
    Control/DatabaseManager.h
)

# 管理员后台头文件
set(ADMIN_HEADERS
    admin/AdminMainWindow.h
    Model/User.h
    Model/RainGear.hpp
    Model/RainGear_subclasses.hpp
    Model/RainGearFactory.h
    Model/GlobalEnum.hpp
    Model/Stationlocal.h
    Model/StationUtils.h
    Control/DBHelper.h
    Control/RainGearDAO.hpp
    Control/DatabaseManager.h
)

# 资源文件
set(RESOURCES
    rgear_icons/icons.qrc
    map_resources/map.qrc
)

# 创建客户端可执行文件
add_executable(${PROJECT_NAME} ${CLIENT_SOURCES} ${CLIENT_HEADERS} ${RESOURCES})

# 创建管理员后台可执行文件
add_executable(${PROJECT_NAME}_Admin ${ADMIN_SOURCES} ${ADMIN_HEADERS} ${RESOURCES})

# 链接Qt库（客户端）
target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# 链接Qt库（管理员后台）
target_link_libraries(${PROJECT_NAME}_Admin
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
)

# Windows特定配置
if(WIN32)
    # MySQL DLL路径（使用桌面dll文件夹的配套文件）
    set(DLL_DIR "C:/Users/hdcqW/Desktop/dll")
    set(MYSQL_LIB_DIR "C:/Program Files/MySQL/MySQL Server 8.0/lib")
    set(MYSQL_DLL "${DLL_DIR}/libmysql.dll")
    set(MYSQL_SSL_DLL "${DLL_DIR}/libssl-3-x64.dll")
    set(MYSQL_CRYPTO_DLL "${DLL_DIR}/libcrypto-3-x64.dll")
    set(MYSQL_DRIVER_DLL "${DLL_DIR}/sqldrivers/qsqlmysql.dll")
    
    # 链接MySQL库（客户端）
    target_link_directories(${PROJECT_NAME} PRIVATE ${MYSQL_LIB_DIR})
    target_link_libraries(${PROJECT_NAME} libmysql)
    
    # 链接MySQL库（管理员后台）
    target_link_directories(${PROJECT_NAME}_Admin PRIVATE ${MYSQL_LIB_DIR})
    target_link_libraries(${PROJECT_NAME}_Admin libmysql)
    
    # 复制MySQL DLL到输出目录（客户端）
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "复制MySQL DLL到客户端输出目录"
    )
    # 复制MySQL的OpenSSL库到输出目录（客户端）
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_SSL_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "复制MySQL的libssl到客户端输出目录"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_CRYPTO_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "复制MySQL的libcrypto到客户端输出目录"
    )
    # 复制MySQL DLL到输出目录（管理员后台）
    add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>
        COMMENT "复制MySQL DLL到管理员后台输出目录"
    )
    # 复制MySQL的OpenSSL库到输出目录（管理员后台）
    add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_SSL_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>
        COMMENT "复制MySQL的libssl到管理员后台输出目录"
    )
    add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_CRYPTO_DLL}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>
        COMMENT "复制MySQL的libcrypto到管理员后台输出目录"
    )
    
    # 从桌面dll文件夹复制所需的DLL
    set(DLL_DIR "C:/Users/hdcqW/Desktop/dll")
    if(EXISTS "${DLL_DIR}")
        file(GLOB DLL_FILES "${DLL_DIR}/*.dll")
        foreach(DLL_FILE ${DLL_FILES})
            get_filename_component(DLL_NAME ${DLL_FILE} NAME)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${DLL_FILE}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMENT "复制 ${DLL_NAME} 到输出目录"
            )
        endforeach()
    endif()
    
    # 复制MinGW运行时库（如果使用MinGW编译器）
    if(MINGW)
        # 尝试从编译器路径找到运行时库
        if(CMAKE_CXX_COMPILER)
            get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
            # MinGW运行时库通常在这些位置
            set(MINGW_RUNTIME_LIBS
                "${COMPILER_DIR}/libgcc_s_seh-1.dll"
                "${COMPILER_DIR}/libstdc++-6.dll"
                "${COMPILER_DIR}/libwinpthread-1.dll"
            )
            foreach(RUNTIME_LIB ${MINGW_RUNTIME_LIBS})
                if(EXISTS "${RUNTIME_LIB}")
                    get_filename_component(LIB_NAME ${RUNTIME_LIB} NAME)
                    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${RUNTIME_LIB}"
                        $<TARGET_FILE_DIR:${PROJECT_NAME}>
                        COMMENT "复制MinGW运行时库 ${LIB_NAME} 到客户端输出目录"
                    )
                    add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${RUNTIME_LIB}"
                        $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>
                        COMMENT "复制MinGW运行时库 ${LIB_NAME} 到管理员后台输出目录"
                    )
                endif()
            endforeach()
        endif()
    endif()
    
    # 复制MySQL驱动插件DLL（使用桌面dll文件夹的配套版本）
    get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake LOCATION)
    if(QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
        get_filename_component(QT_PREFIX_DIR ${QT_BIN_DIR} DIRECTORY)
        
        if(EXISTS "${MYSQL_DRIVER_DLL}")
            # 客户端
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "创建客户端sqldrivers目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_DRIVER_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "复制MySQL驱动插件到客户端sqldrivers目录"
            )
            # 复制libmysql.dll到sqldrivers目录（qsqlmysql.dll的依赖）
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "复制libmysql.dll到客户端sqldrivers目录"
            )
            # 复制OpenSSL库到sqldrivers目录（libmysql.dll的依赖）
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_SSL_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "复制libssl到客户端sqldrivers目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_CRYPTO_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
                COMMENT "复制libcrypto到客户端sqldrivers目录"
            )
            # 管理员后台
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/sqldrivers
                COMMENT "创建管理员后台sqldrivers目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_DRIVER_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/sqldrivers
                COMMENT "复制MySQL驱动插件到管理员后台sqldrivers目录"
            )
            # 复制libmysql.dll到sqldrivers目录（qsqlmysql.dll的依赖）
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/sqldrivers
                COMMENT "复制libmysql.dll到管理员后台sqldrivers目录"
            )
            # 复制OpenSSL库到sqldrivers目录（libmysql.dll的依赖）
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_SSL_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/sqldrivers
                COMMENT "复制libssl到管理员后台sqldrivers目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MYSQL_CRYPTO_DLL}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/sqldrivers
                COMMENT "复制libcrypto到管理员后台sqldrivers目录"
            )
        endif()
    endif()
    
    # 复制Qt运行时DLL（包括所有必需的DLL）
    if(QT_QMAKE_EXECUTABLE)
        get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
        # Qt Widgets应用需要这些核心DLL
        set(QT_DLLS
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Core.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Gui.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Widgets.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Sql.dll"
            "${QT_BIN_DIR}/Qt${QT_VERSION_MAJOR}Network.dll"
        )
        foreach(QT_DLL ${QT_DLLS})
            if(EXISTS "${QT_DLL}")
                get_filename_component(QT_DLL_NAME ${QT_DLL} NAME)
                # 客户端
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_DLL}"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                    COMMENT "复制 ${QT_DLL_NAME} 到客户端输出目录"
                )
                # 管理员后台
                add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${QT_DLL}"
                    $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>
                    COMMENT "复制 ${QT_DLL_NAME} 到管理员后台输出目录"
                )
            else()
                message(WARNING "Qt DLL not found: ${QT_DLL}")
            endif()
        endforeach()
        
        # 复制Qt平台插件（Windows需要platforms/qwindows.dll）
        get_filename_component(QT_PREFIX_DIR ${QT_BIN_DIR} DIRECTORY)
        set(QT_PLATFORMS_DIR "${QT_PREFIX_DIR}/plugins/platforms")
        set(QT_PLATFORM_PLUGIN "${QT_PLATFORMS_DIR}/qwindows.dll")
        if(EXISTS "${QT_PLATFORM_PLUGIN}")
            # 客户端
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
                COMMENT "创建客户端platforms目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_PLATFORM_PLUGIN}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
                COMMENT "复制Qt平台插件到客户端platforms目录"
            )
            # 管理员后台
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/platforms
                COMMENT "创建管理员后台platforms目录"
            )
            add_custom_command(TARGET ${PROJECT_NAME}_Admin POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_PLATFORM_PLUGIN}"
                $<TARGET_FILE_DIR:${PROJECT_NAME}_Admin>/platforms
                COMMENT "复制Qt平台插件到管理员后台platforms目录"
            )
        endif()
    endif()
endif()

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(${PROJECT_NAME}_Admin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Debug/Release配置
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "d"
)

