#!/bin/bash
# ============================================================
# 远程传感器数据采集及可视化程序 - Linux 一键构建脚本
# 适用系统: Ubuntu 20.04 / 22.04 / Debian 11+
# ============================================================
set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

echo "=========================================="
echo " 传感器采集可视化系统 - Linux 构建"
echo " 项目目录: ${PROJECT_DIR}"
echo "=========================================="

# ---------- 1. 依赖检查 ----------
echo ""
echo "[1/4] 检查编译依赖..."

check_dep() {
    if ! command -v "$1" &> /dev/null; then
        echo "  [缺失] $1"
        return 1
    else
        echo "  [OK]   $1 ($($1 --version 2>/dev/null | head -1))"
        return 0
    fi
}

MISSING=0
check_dep cmake || MISSING=1
check_dep g++   || MISSING=1
check_dep make  || MISSING=1

if [ "$MISSING" -eq 1 ]; then
    echo ""
    echo "缺少编译工具，请先安装:"
    echo "  sudo apt update && sudo apt install -y build-essential cmake"
    exit 1
fi

# ---------- 2. 库依赖检查 ----------
echo ""
echo "[2/4] 检查第三方库..."

# 检查 Boost
if [ -d /usr/include/boost ] || dpkg -l libboost-all-dev &>/dev/null; then
    echo "  [OK]   Boost.Asio"
else
    echo "  [缺失] Boost (libboost-all-dev)"
    NEED_BOOST=1
fi

# 检查 Qt6
if pkg-config --exists Qt6Widgets 2>/dev/null || [ -d /usr/include/x86_64-linux-gnu/qt6 ]; then
    echo "  [OK]   Qt6 (Widgets + Charts)"
else
    echo "  [缺失] Qt6 (qt6-base-dev libqt6charts6-dev)"
    NEED_QT6=1
fi

# 检查 SQLite3
if pkg-config --exists sqlite3 2>/dev/null || [ -f /usr/include/sqlite3.h ]; then
    echo "  [OK]   SQLite3"
else
    echo "  [缺失] SQLite3 (libsqlite3-dev)"
    NEED_SQLITE=1
fi

# 检查 spdlog
if [ -f /usr/include/spdlog/spdlog.h ] || pkg-config --exists spdlog 2>/dev/null; then
    echo "  [OK]   spdlog"
else
    echo "  [缺失] spdlog (libspdlog-dev)"
    NEED_SPDLOG=1
fi

if [ -n "$NEED_BOOST" ] || [ -n "$NEED_QT6" ] || [ -n "$NEED_SQLITE" ] || [ -n "$NEED_SPDLOG" ]; then
    echo ""
    echo "缺少第三方库，请先安装:"
    echo "  sudo apt update && sudo apt install -y \\"
    echo "      libboost-all-dev \\"
    echo "      qt6-base-dev libqt6charts6-dev \\"
    echo "      libsqlite3-dev \\"
    echo "      libspdlog-dev"
    exit 1
fi

# ---------- 3. CMake 配置 ----------
echo ""
echo "[3/4] CMake 配置..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# ---------- 4. 编译 ----------
echo ""
echo "[4/4] 编译 (并行 $(nproc) 线程)..."
make -j"$(nproc)"

echo ""
echo "=========================================="
echo " 构建成功!"
echo " 可执行文件: ${BUILD_DIR}/SensorViz"
echo " 配置文件:   ${BUILD_DIR}/config.ini"
echo ""
echo " 运行方式:"
echo "   cd ${BUILD_DIR}"
echo "   ./SensorViz"
echo "=========================================="
