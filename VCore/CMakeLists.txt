cmake_minimum_required(VERSION 3.0)
project(VCore)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Debug版本后缀加d
set(CMAKE_DEBUG_POSTFIX "d")

# 设置SDL为否
set(SDL_FOUND FALSE)

# 设置 VCore 路径
set(VCORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 设置默认的 libuv 路径
set(DEFAULT_LIBUV_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/include")
set(DEFAULT_LIBUV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/src")
set(LIBUV_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/include" CACHE PATH "Path to libuv include directory")
set(LIBUV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/src" CACHE PATH "Path to libuv source directory")
# 如果未提供路径，则使用默认路径
if(NOT LIBUV_INCLUDE_DIR)
    set(LIBUV_INCLUDE_DIR ${DEFAULT_LIBUV_INCLUDE_DIR})
endif()

if(NOT LIBUV_SOURCE_DIR)
    set(LIBUV_SOURCE_DIR ${DEFAULT_LIBUV_SOURCE_DIR})
endif()


# Core
set(CORE_FILES
    ${VCORE_SRC_DIR}/VBuf.h
    ${VCORE_SRC_DIR}/VBuf.cpp
    ${VCORE_SRC_DIR}/VCore.h
    ${VCORE_SRC_DIR}/VCore.cpp
    ${VCORE_SRC_DIR}/VDir.h
    ${VCORE_SRC_DIR}/VDir.cpp
    ${VCORE_SRC_DIR}/VIntPtr.h
    ${VCORE_SRC_DIR}/VIntPtr.cpp
    ${VCORE_SRC_DIR}/VObject.h
    ${VCORE_SRC_DIR}/VObject.cpp
    ${VCORE_SRC_DIR}/VString.h
)
source_group(Core FILES ${CORE_FILES})

# Hanlde
set(HANDLE_FILES
    ${VCORE_SRC_DIR}/VAsync.h
    ${VCORE_SRC_DIR}/VAsync.cpp
    ${VCORE_SRC_DIR}/VCheck.h
    ${VCORE_SRC_DIR}/VCheck.cpp
    ${VCORE_SRC_DIR}/VFsEvent.h
    ${VCORE_SRC_DIR}/VFsEvent.cpp
    ${VCORE_SRC_DIR}/VFsPoll.h
    ${VCORE_SRC_DIR}/VFsPoll.cpp
    ${VCORE_SRC_DIR}/VHandle.h
    ${VCORE_SRC_DIR}/VHandle.cpp
    ${VCORE_SRC_DIR}/VIdle.h
    ${VCORE_SRC_DIR}/VIdle.cpp
    ${VCORE_SRC_DIR}/VLoop.h
    ${VCORE_SRC_DIR}/VLoop.cpp
    ${VCORE_SRC_DIR}/VPipe.h
    ${VCORE_SRC_DIR}/VPipe.cpp
    ${VCORE_SRC_DIR}/VPoll.h
    ${VCORE_SRC_DIR}/VPoll.cpp
    ${VCORE_SRC_DIR}/VPrepare.h
    ${VCORE_SRC_DIR}/VPrepare.cpp
    ${VCORE_SRC_DIR}/VProcess.h
    ${VCORE_SRC_DIR}/VProcess.cpp
    ${VCORE_SRC_DIR}/VSignal.h
    ${VCORE_SRC_DIR}/VSignal.cpp
    ${VCORE_SRC_DIR}/VStream.h
    ${VCORE_SRC_DIR}/VStream.cpp
    ${VCORE_SRC_DIR}/VTcp.h
    ${VCORE_SRC_DIR}/VTcp.cpp
    ${VCORE_SRC_DIR}/VTimer.h
    ${VCORE_SRC_DIR}/VTimer.cpp
    ${VCORE_SRC_DIR}/VTty.h
    ${VCORE_SRC_DIR}/VTty.cpp
    ${VCORE_SRC_DIR}/VUdp.h
    ${VCORE_SRC_DIR}/VUdp.cpp
)
source_group(Hanlde FILES ${HANDLE_FILES})

# NetWork
set(NETWORK_FILES
    ${VCORE_SRC_DIR}/VTcpService.h
    ${VCORE_SRC_DIR}/VTcpService.cpp
)
source_group(NetWork FILES ${NETWORK_FILES})

# Other
set(OTHER_FILES
    ${VCORE_SRC_DIR}/VBarrier.h
    ${VCORE_SRC_DIR}/VBarrier.cpp
    ${VCORE_SRC_DIR}/VCpuInfo.h
    ${VCORE_SRC_DIR}/VCpuInfo.cpp
    ${VCORE_SRC_DIR}/VDirent.h
    ${VCORE_SRC_DIR}/VDirent.cpp
    ${VCORE_SRC_DIR}/VEnvItem.h
    ${VCORE_SRC_DIR}/VEnvItem.cpp
    ${VCORE_SRC_DIR}/VGroup.h
    ${VCORE_SRC_DIR}/VGroup.cpp
    ${VCORE_SRC_DIR}/VInterfaceAddress.h
    ${VCORE_SRC_DIR}/VInterfaceAddress.cpp
    ${VCORE_SRC_DIR}/VMetrics.h
    ${VCORE_SRC_DIR}/VMetrics.cpp
    ${VCORE_SRC_DIR}/VPasswd.h
    ${VCORE_SRC_DIR}/VPasswd.cpp
    ${VCORE_SRC_DIR}/VRwlock.h
    ${VCORE_SRC_DIR}/VRwlock.cpp
    ${VCORE_SRC_DIR}/VThread.h
    ${VCORE_SRC_DIR}/VThread.cpp
    ${VCORE_SRC_DIR}/VUtsname.h
    ${VCORE_SRC_DIR}/VUtsname.cpp
    ${VCORE_SRC_DIR}/VStatfs.h
    ${VCORE_SRC_DIR}/VStatfs.cpp
)
source_group(Other FILES ${OTHER_FILES})

# Req
set(REQ_FILES
    ${VCORE_SRC_DIR}/VReq.h
    ${VCORE_SRC_DIR}/VReq.cpp
    ${VCORE_SRC_DIR}/VWrite.h
    ${VCORE_SRC_DIR}/VWrite.cpp
    ${VCORE_SRC_DIR}/VConnect.h
    ${VCORE_SRC_DIR}/VConnect.cpp
    ${VCORE_SRC_DIR}/VFs.h
    ${VCORE_SRC_DIR}/VFs.cpp
    ${VCORE_SRC_DIR}/VGetaddrinfo.h
    ${VCORE_SRC_DIR}/VGetaddrinfo.cpp
    ${VCORE_SRC_DIR}/VGetnameinfo.h
    ${VCORE_SRC_DIR}/VGetnameinfo.cpp
    ${VCORE_SRC_DIR}/VRandom.h
    ${VCORE_SRC_DIR}/VRandom.cpp
    ${VCORE_SRC_DIR}/VShutdown.h
    ${VCORE_SRC_DIR}/VShutdown.cpp
    ${VCORE_SRC_DIR}/VUdpSend.h
    ${VCORE_SRC_DIR}/VUdpSend.cpp
    ${VCORE_SRC_DIR}/VWork.h
    ${VCORE_SRC_DIR}/VWork.cpp
)
source_group(Req FILES ${REQ_FILES})

# Uv
set(UV
    ${VCORE_SRC_DIR}/VLibuv.h
#    ${VCORE_SRC_DIR}/main.cpp
)

# libuv
file(GLOB LIBUV_FILES
    "${LIBUV_SOURCE_DIR}/*.h"
    "${LIBUV_SOURCE_DIR}/*.c"
)
source_group(libuv FILES ${LIBUV_FILES})

# libuv/win
file(GLOB LIBUV_WIN_FILES
    "${LIBUV_SOURCE_DIR}/win/*.h"
    "${LIBUV_SOURCE_DIR}/win/*.c"
)
source_group(libuv/win FILES ${LIBUV_WIN_FILES})

# libuv/unix
file(GLOB LIBUV_UNIX_FILES
    "${LIBUV_SOURCE_DIR}/unix/*.h"
    "${LIBUV_SOURCE_DIR}/unix/*.c"
)
source_group(libuv/unix FILES ${LIBUV_UNIX_FILES})

# # # # # # # # # # # # install# # # # # # # # # # # # 
# install include vcore files
file(GLOB INSTALL_INCLUDE_VCORE_FILES
    "${VCORE_SRC_DIR}/*.h"
)
# install include libuv files
file(GLOB INSTALL_INCLUDE_LIBUV_FILES
    "${LIBUV_SOURCE_DIR}/*.h"
)
# install include libuv/unix files
file(GLOB INSTALL_INCLUDE_LIBUV_UNIX_FILES
    "${LIBUV_SOURCE_DIR}/unix/*.h"
)

# install include libuv/win files
file(GLOB INSTALL_INCLUDE_LIBUV_WIN_FILES
    "${LIBUV_SOURCE_DIR}/win/*.h"
)
# # # # # # # # # # # # install# # # # # # # # # # # # 

# 设置包含文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBUV_INCLUDE_DIR}
	${LIBUV_SOURCE_DIR}
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
add_library(VCore STATIC ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_UNIX_FILES} ${NETWORK_FILES} ${OTHER_FILES} ${REQ_FILES})
#add_executable(VCore ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_UNIX_FILES} ${NETWORK_FILES} ${OTHER_FILES} ${REQ_FILES})
target_link_libraries(VCore
    pthread   # 线程库
    dl        # 动态链接库
)

install(FILES 
        ${INSTALL_INCLUDE_LIBUV_UNIX_FILES}
        # 列出其他头文件
        DESTINATION include/libuv/unix)

elseif(WIN32)
add_library(VCore STATIC ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_WIN_FILES} ${NETWORK_FILES} ${OTHER_FILES} ${REQ_FILES})
#add_executable(VCore ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_WIN_FILES} ${NETWORK_FILES} ${OTHER_FILES} ${REQ_FILES})
target_link_libraries(VCore
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
)


install(FILES 
        ${INSTALL_INCLUDE_LIBUV_WIN_FILES}
        # 列出其他头文件
        DESTINATION include/libuv/win)

endif()



# 将可执行文件安装到 bin 目录
#install(TARGETS VCore DESTINATION bin)

# 将库文件安装到 lib 目录
install(TARGETS VCore DESTINATION lib)

# 将头文件安装到 include 目录
install(FILES 
        ${INSTALL_INCLUDE_VCORE_FILES}
        # 列出其他头文件
        DESTINATION include)
install(FILES 
        ${INSTALL_INCLUDE_VCORE_FILES}
        # 列出其他头文件
        DESTINATION include/libuv)