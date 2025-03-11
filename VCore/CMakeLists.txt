cmake_minimum_required(VERSION 3.0)
project(VCore)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Debug版本后缀加d
set(CMAKE_DEBUG_POSTFIX "d")

# 设置SDL为否
set(SDL_FOUND FALSE)

if(MSVC)
# 对于 C++ 项目
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

# 或者对于 C 项目
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VCore/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VCore/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VCore/lib)

# install include libuv/win files
file(GLOB PDB_FILE
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/*.pdb"
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/*.pdb"
)

message(STATUS "Project Binary Directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

#get_target_property(VCORE_TARGET_LOCATION VCore LOCATION)
#set(PDB_FILE "${VCORE_TARGET_LOCATION}.pdb")
#message(STATUS "Executable Location: ${VCORE_TARGET_LOCATION}")

set(ENABLE_VMEMORY_MEMORY_POOL OFF CACHE BOOL "using VMemory memory pool")
if(ENABLE_VMEMORY_MEMORY_POOL)
    add_definitions(-DVMEMORY_MEMORY_POOL)
endif()

set(ENABLE_VMEMORY_THREAD_SAFETY ON CACHE BOOL "using VMemory thread safety")
if(ENABLE_VMEMORY_THREAD_SAFETY)
    add_definitions(-DVMEMORY_THREAD_SAFETY)
endif()

set(ENABLE_CRT_NO_VA_START_VALIDATION ON CACHE BOOL "will define _CRT_NO_VA_START_VALIDATION")
if(ENABLE_CRT_NO_VA_START_VALIDATION)
    add_definitions(-_CRT_NO_VA_START_VALIDATION)
endif()



if(MSVC)
    add_compile_options("/Zi")
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Configuring for Debug build")
        target_compile_options(VCore PRIVATE "/Zi")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Configuring for Release build")
        target_compile_options(VCore PRIVATE "/Zi")
    else()
    endif()
endif()



# 设置 VCore 路径
set(VCORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 设置默认的 libuv 路径
set(DEFAULT_LIBUV_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/include")
set(DEFAULT_LIBUV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/src")
set(LIBUV_DIR "" CACHE PATH "Path to libuv directory")
set(LIBUV_INCLUDE_DIR "${LIBUV_DIR}/include")
set(LIBUV_SOURCE_DIR "${LIBUV_DIR}/src")
# 如果未提供路径，则使用默认路径
if(NOT LIBUV_DIR)
    set(LIBUV_INCLUDE_DIR ${DEFAULT_LIBUV_INCLUDE_DIR})
    set(LIBUV_SOURCE_DIR ${DEFAULT_LIBUV_SOURCE_DIR})
endif()

# Core
set(CORE_FILES
    ${VCORE_SRC_DIR}/VBuf.h
    ${VCORE_SRC_DIR}/VBuf.cpp
    ${VCORE_SRC_DIR}/VCore.h
    ${VCORE_SRC_DIR}/VCore.cpp
    ${VCORE_SRC_DIR}/VCoreDefine.h
    ${VCORE_SRC_DIR}/VLibuvInclude.h
    ${VCORE_SRC_DIR}/VDir.h
    ${VCORE_SRC_DIR}/VDir.cpp
    ${VCORE_SRC_DIR}/VIntPtr.h
    ${VCORE_SRC_DIR}/VIntPtr.cpp
    ${VCORE_SRC_DIR}/VMemory.h
    ${VCORE_SRC_DIR}/VMemory.cpp
    ${VCORE_SRC_DIR}/VMemoryPool.h
    ${VCORE_SRC_DIR}/VMemoryPool.cpp
    ${VCORE_SRC_DIR}/VMemoryCache.h
    ${VCORE_SRC_DIR}/VMemoryCache.cpp
    ${VCORE_SRC_DIR}/VThreadPool.h
    ${VCORE_SRC_DIR}/VThreadPool.cpp
    ${VCORE_SRC_DIR}/VObject.h
    ${VCORE_SRC_DIR}/VObject.cpp
    ${VCORE_SRC_DIR}/VString.h
    ${VCORE_SRC_DIR}/VString.cpp
    ${VCORE_SRC_DIR}/VBase64.h
    ${VCORE_SRC_DIR}/VBase64.cpp
    ${VCORE_SRC_DIR}/VLogger.h
    ${VCORE_SRC_DIR}/VLogger.cpp
    ${VCORE_SRC_DIR}/VSha256.h
    ${VCORE_SRC_DIR}/VSha256.cpp
    ${VCORE_SRC_DIR}/VMutex.h
    ${VCORE_SRC_DIR}/VMutex.cpp
    ${VCORE_SRC_DIR}/VFile.h
    ${VCORE_SRC_DIR}/VFile.cpp
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
#    "${LIBUV_SOURCE_DIR}/unix/*.h"
#    "${LIBUV_SOURCE_DIR}/unix/*.c"
    "${LIBUV_INCLUDE_DIR}/*.h"
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
	"${LIBUV_INCLUDE_DIR}/*.h"
)
# install include libuv/unix files
file(GLOB INSTALL_INCLUDE_LIBUV_UNIX_FILES
    "${LIBUV_SOURCE_DIR}/unix/*.h"
)

# install include libuv/win files
file(GLOB INSTALL_INCLUDE_LIBUV_WIN_FILES
    "${LIBUV_SOURCE_DIR}/win/*.h"
)

# install include libuv/uv files
file(GLOB INSTALL_INCLUDE_LIBUV_UV_FILES
    "${LIBUV_INCLUDE_DIR}/uv/*.h"
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
    add_compile_options(/wd4100 /wd4127 /wd4201 /wd4206 /wd4210 /wd4221 /wd4232 /wd4456 /wd4457 /wd4459 /wd4706 /wd4996)
elseif (UNIX)
    # 对于 GNU 或 Clang 编译器
    add_compile_options(-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)
endif()





# 创建可执行文件并添加源文件以及相应的头文件
if (UNIX)
add_library(VCore STATIC ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_UNIX_FILES} ${OTHER_FILES} ${REQ_FILES})
#add_executable(VCore ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_UNIX_FILES} ${OTHER_FILES} ${REQ_FILES})
target_link_libraries(VCore
    pthread   # 线程库
    dl        # 动态链接库
)

install(FILES 
        ${INSTALL_INCLUDE_LIBUV_UNIX_FILES}
        # include/libuv/unix
        DESTINATION include/libuv/unix)

elseif(WIN32)
add_library(VCore STATIC ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_WIN_FILES} ${OTHER_FILES} ${REQ_FILES})
#add_executable(VCore ${CORE_FILES} ${HANDLE_FILES} ${LIBUV_FILES} ${LIBUV_WIN_FILES} ${OTHER_FILES} ${REQ_FILES})
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
	dbghelp.lib
)


install(FILES 
        ${INSTALL_INCLUDE_LIBUV_WIN_FILES}
        # include/libuv/win
        DESTINATION include/libuv/win)

endif()



# 将可执行文件安装到 bin 目录
#install(TARGETS VCore DESTINATION bin)

# 将库文件安装到 lib 目录
install(TARGETS VCore DESTINATION lib)

# 在调试模式下安装 .pdb 文件
install(FILES ${PDB_FILE} DESTINATION lib OPTIONAL) 

# 将头文件安装到 include 目录
install(FILES 
        ${INSTALL_INCLUDE_VCORE_FILES}
        # include
        DESTINATION include/VCore)
install(FILES 
        ${INSTALL_INCLUDE_LIBUV_FILES}
        # include/libuv
        DESTINATION include/libuv)
install(FILES 
        ${INSTALL_INCLUDE_LIBUV_UV_FILES}
        # include/libuv/uv
        DESTINATION include/libuv/uv)
