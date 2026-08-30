cmake_minimum_required(VERSION 3.16)
project(Sensor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ========== 依赖查找 ==========
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Core)
find_package(Boost REQUIRED COMPONENTS system)
find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)

# ========== 源文件分组 ==========
set(COMMON_SOURCES
    src/common/Config.cpp
    src/common/Logger.cpp
)

set(PROTOCOL_SOURCES
    src/protocol/FrameCodec.cpp
)

set(NET_SOURCES
    src/net/TcpClient.cpp
    src/net/MockServer.cpp
)

set(BUSINESS_SOURCES
    src/business/DataProcessor.cpp
    src/business/AcquisitionManager.cpp
)

set(STORAGE_SOURCES
    src/storage/SqliteStorage.cpp
)

set(UI_SOURCES
    src/ui/MainWindow.cpp
)

set(ALL_SOURCES
    src/main.cpp
    ${COMMON_SOURCES}
    ${PROTOCOL_SOURCES}
    ${NET_SOURCES}
    ${BUSINESS_SOURCES}
    ${STORAGE_SOURCES}
    ${UI_SOURCES}
)

# ========== 可执行目标 ==========
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Core
    Boost::system
    SQLite::SQLite3
    spdlog::spdlog
    pthread
)

# ========== 编译选项 ==========
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)
endif()

# ========== 安装配置文件到构建目录 ==========
configure_file(${CMAKE_SOURCE_DIR}/config.ini
    ${CMAKE_BINARY_DIR}/config.ini COPYONLY)
