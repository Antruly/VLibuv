cmake_minimum_required(VERSION 3.0)
project(VNetWork)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Debug版本后缀加d
set(CMAKE_DEBUG_POSTFIX "d")

# 设置SDL为否
set(SDL_FOUND FALSE)

# 设置 VCore 路径
set(VCORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../VCore/src")

# 设置 VNetWork 路径
set(VNETWORK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# NetWork
set(NETWORK_FILES
    ${VNETWORK_SRC_DIR}/VTcpService.h
    ${VNETWORK_SRC_DIR}/VTcpService.cpp
)
source_group(NetWork FILES ${NETWORK_FILES})

# # # # # # # # # # # # install# # # # # # # # # # # # 
# install include vnetwork files
file(GLOB INSTALL_INCLUDE_VNETWORK_FILES
    "${VNETWORK_SRC_DIR}/*.h"
)
# # # # # # # # # # # # install# # # # # # # # # # # # 

# 设置包含文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBUV_DIR}/include
    ${LIBUV_DIR}/src
    ${VCORE_SRC_DIR}
)
if (MSVC)
    # 禁用特定的警告
    add_compile_options(/wd4100 /wd4127 /wd4201 /wd4206 /wd4210 /wd4232 /wd4456 /wd4457 /wd4459 /wd4706 /wd4996)
elseif (UNIX)
    # 对于 GNU 或 Clang 编译器
    add_compile_options(-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)
endif()



# 创建可执行文件并添加源文件以及相应的头文件
if (UNIX)
add_library(VNetWork STATIC ${NETWORK_FILES})
#add_executable(VNetWork ${NETWORK_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 在 Debug 模式下链接 debug 版本的库
    target_link_libraries(VNetWork
    pthread   # 线程库
    dl        # 动态链接库
	VCored
)
else()
    # 在 Release 模式下链接 release 版本的库
    target_link_libraries(VNetWork
    pthread   # 线程库
    dl        # 动态链接库
	VCore
)
endif()



elseif(WIN32)
add_library(VNetWork STATIC ${NETWORK_FILES})
#add_executable(VNetWork ${NETWORK_FILES})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 在 Debug 模式下链接 debug 版本的库
     target_link_libraries(VNetWork
    psapi.lib
    user32.lib
    advapi32.lib
    iphlpapi.lib
    userenv.lib
    ws2_32.lib
    kernel32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
	dbghelp.lib
	VCored.lib
)
else()
    # 在 Release 模式下链接 release 版本的库
    target_link_libraries(VNetWork
    psapi.lib
    user32.lib
    advapi32.lib
    iphlpapi.lib
    userenv.lib
    ws2_32.lib
    kernel32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
	dbghelp.lib
	VCore.lib
)
endif()

endif()


# 将可执行文件安装到 bin 目录
#install(TARGETS VNetWork DESTINATION bin)

# 将库文件安装到 lib 目录
install(TARGETS VNetWork DESTINATION lib)

# 将头文件安装到 include 目录
install(FILES 
        ${INSTALL_INCLUDE_VNETWORK_FILES}
        # include
        DESTINATION include/VNetWork)
