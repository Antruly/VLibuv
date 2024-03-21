cmake_minimum_required(VERSION 3.0)

project(VLibuv)
# 设置 VLibuv 路径
set(VLIBUV_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# 设置安装目录根路径
# set(CMAKE_INSTALL_PREFIX install)


# VLibuv
set(VLIBUV_FILES
    ${VLIBUV_DIR}/VLibuv.h
    ${VLIBUV_DIR}/VVersion.h
)


# 添加子目录（src和library）
add_subdirectory(VCore)
add_subdirectory(VNetWork)
add_subdirectory(VWeb)
install(FILES 
        ${VLIBUV_FILES}
        # include
        DESTINATION include)
