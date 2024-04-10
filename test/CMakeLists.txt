cmake_minimum_required(VERSION 3.0)
project(test-Vlibuv)

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

# 设置 VCore 路径
set(VCORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 设置默认的 libuv 路径
set(DEFAULT_LIBUV_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/include")
set(DEFAULT_LIBUV_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libuv/lib")
set(LIBUV_DIR "" CACHE PATH "Path to libuv directory")
set(LIBUV_INCLUDE_DIR "${LIBUV_DIR}/include")
set(LIBUV_LIB_DIR "${LIBUV_DIR}/lib")
# 如果未提供路径，则使用默认路径
if(NOT LIBUV_DIR)
    set(LIBUV_INCLUDE_DIR ${DEFAULT_LIBUV_INCLUDE_DIR})
    set(LIBUV_LIB_DIR ${DEFAULT_LIBUV_LIB_DIR})
endif()

# 设置默认的 openssl 路径
set(DEFAULT_OPENSSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/openssl")
set(DEFAULT_OPENSSL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/openssl")
set(OPENSSL_DIR "" CACHE PATH "Path to openssl directory")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_DIR}/include")
set(OPENSSL_LIB_DIR "${OPENSSL_DIR}/lib")

# 如果未提供路径，则使用默认路径
if(NOT OPENSSL_DIR)
    set(OPENSSL_INCLUDE_DIR ${DEFAULT_OPENSSL_INCLUDE_DIR})
    set(OPENSSL_LIB_DIR ${DEFAULT_OPENSSL_LIB_DIR})
endif()

    message("OPENSSL_INCLUDE_DIR path: ${OPENSSL_INCLUDE_DIR}")
    message("OPENSSL_LIB_DIR path: ${OPENSSL_LIB_DIR}")

if (OPENSSL_INCLUDE_DIR)
    file(READ "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h" OPENSSL_VERSION_FILE)
    message("OpenSSL version file content:")
    #message("${OPENSSL_VERSION_FILE}")

    string(REGEX MATCH "define OPENSSL_VERSION_MAJOR[ \t]+([0-9a-fA-F]+)" _ ${OPENSSL_VERSION_FILE})
    if (NOT "${CMAKE_MATCH_1}" STREQUAL "")
    set(OPENSSL_VERSION_MAJOR ${CMAKE_MATCH_1})
    endif ()
    string(REGEX MATCH "define OPENSSL_VERSION_MINOR[ \t]+([0-9a-fA-F]+)" _ ${OPENSSL_VERSION_FILE})
    if (NOT "${CMAKE_MATCH_1}" STREQUAL "")
    set(OPENSSL_VERSION_MINOR ${CMAKE_MATCH_1})
    endif ()
    string(REGEX MATCH "define OPENSSL_VERSION_PATCH[ \t]+([0-9a-fA-F]+)" _ ${OPENSSL_VERSION_FILE})
    if (NOT "${CMAKE_MATCH_1}" STREQUAL "")
    set(OPENSSL_VERSION_PATCH ${CMAKE_MATCH_1})
    endif ()
    

if(NOT OPENSSL_VERSION_MAJOR AND NOT OPENSSL_VERSION_MINOR AND NOT OPENSSL_VERSION_PATCH)
    string(REGEX MATCH "OPENSSL_VERSION_NUMBER[ \t]+0x([0-9a-fA-F]+)" _ ${OPENSSL_VERSION_FILE})
    if (NOT "${CMAKE_MATCH_1}" STREQUAL "")
    set(OPENSSL_VERSION_HEX ${CMAKE_MATCH_1})
    message("OpenSSL version: 0x${OPENSSL_VERSION_HEX}")
    # 将版本号转换为字符串
    string(SUBSTRING "${OPENSSL_VERSION_HEX}" 0 1 OPENSSL_VERSION_MAJOR)
    string(SUBSTRING "${OPENSSL_VERSION_HEX}" 2 1 OPENSSL_VERSION_MINOR)
    string(SUBSTRING "${OPENSSL_VERSION_HEX}" 4 1 OPENSSL_VERSION_PATCH)
    else()
    message(FATAL_ERROR "Failed to extract OpenSSL version number")
    endif ()
    endif()

    message("OpenSSL version: ${OPENSSL_VERSION_MAJOR}.${OPENSSL_VERSION_MINOR}.${OPENSSL_VERSION_PATCH}")
endif ()



# 设置包含文件路径
include_directories(
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/include/VCore
    ${CMAKE_INSTALL_PREFIX}/include/VNetWork
    ${CMAKE_INSTALL_PREFIX}/include/VWeb
    ${CMAKE_INSTALL_PREFIX}/include/libuv
    ${CMAKE_INSTALL_PREFIX}/include/http_parser
    ${CMAKE_INSTALL_PREFIX}/include/websocket_parser
    ${CMAKE_INSTALL_PREFIX}/include/zlib
    ${CMAKE_INSTALL_PREFIX}/include/openssl
)
# 设置库目录
link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
)

if (MSVC)
    # 禁用特定的警告
    add_compile_options(/wd4100 /wd4127 /wd4201 /wd4206 /wd4210 /wd4221 /wd4232 /wd4456 /wd4457 /wd4459 /wd4706 /wd4996)
elseif (UNIX)
    # 对于 GNU 或 Clang 编译器
    add_compile_options(-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)
endif()

SET(BUILD_UNIX_SYSTEM_LIBS
    pthread
    dl
)
SET(BUILD_WIN32_SYSTEM_LIBS
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
    crypt32.lib
)

SET(BUILD_UNIX_VLIBUV_DEBUG_LIBS
    libVCored.a
    libVNetWorkd.a
    libVWebd.a
    libuv.a
)

SET(BUILD_UNIX_VLIBUV_RELEASE_LIBS 
    libVCored.a
    libVNetWorkd.a
    libVWebd.a
    libuv.a
)

SET(BUILD_WIN32_VLIBUV_DEBUG_LIBS 
    VCored.lib
    VNetWorkd.lib
    VWebd.lib
)

SET(BUILD_WIN32_VLIBUV_RELEASE_LIBS 
    VCore.lib
    VNetWork.lib
    VWeb.lib
)

 message("OpenSSL version ${OPENSSL_VERSION_HEX}")
# 根据版本号判断版本
if ("${OPENSSL_VERSION_MAJOR}" STREQUAL "1" AND "${OPENSSL_VERSION_MINOR}" STREQUAL "0") # OpenSSL 1.0.x or older
        message("OpenSSL version 1.0.x or older detected")
SET(BUILD_UNIX_OPENSSL_LIBS
    libeay.a
    ssleay.a
)

SET(BUILD_WIN32_x64_OPENSSL_LIBS
    libeay.lib
    ssleay.lib
)
SET(BUILD_WIN32_x86_OPENSSL_LIBS
    libeay32.lib
    ssleay32.lib
)
elseif ("${OPENSSL_VERSION_MAJOR}" STREQUAL "1" AND "${OPENSSL_VERSION_MINOR}" STREQUAL "1") # OpenSSL 1.1.x
        message("OpenSSL version 1.1.x detected")
SET(BUILD_UNIX_OPENSSL_LIBS
    libssl.a
    libcrypto.a
)

SET(BUILD_WIN32_x64_OPENSSL_LIBS
    libssl.lib
    libcrypto.lib
)
SET(BUILD_WIN32_x86_OPENSSL_LIBS
    libssl.lib
    libcrypto.lib
)
elseif ("${OPENSSL_VERSION_MAJOR}" STREQUAL "3") # OpenSSL 3.x
        message("OpenSSL version 3.x detected")
SET(BUILD_UNIX_OPENSSL_LIBS
    libssl.a
    libcrypto.a
)

SET(BUILD_WIN32_x64_OPENSSL_LIBS
    libssl.lib
    libcrypto.lib
)
SET(BUILD_WIN32_x86_OPENSSL_LIBS
    libssl.lib
    libcrypto.lib
)
    else ()
        message("Unknown OpenSSL version")
    endif ()

# 创建可执行文件并添加源文件以及相应的头文件
add_executable(test-VTcpClient test-VTcpClient.cpp)
add_executable(test-VTcpServer test-VTcpServer.cpp)
add_executable(test-VHttpClient test-VHttpClient.cpp)
add_executable(test-VHttpServer test-VHttpServer.cpp)
add_executable(test-VOpenSsl test-VOpenSsl.cpp)
add_executable(test-VHttpParser test-VHttpParser.cpp)
add_executable(test-VWebSocketParser test-VWebSocketParser.cpp)
add_executable(test-VWebSocketClient test-VWebSocketClient.cpp)


if (UNIX)
# test-VTcpClient
target_link_libraries(test-VTcpClient 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VTcpServer
target_link_libraries(test-VTcpServer 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VHttpClient
target_link_libraries(test-VHttpClient 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VHttpServer
target_link_libraries(test-VHttpServer 
   debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VOpenSsl
target_link_libraries(test-VOpenSsl 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VHttpParser
target_link_libraries(test-VHttpParser 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VWebSocketParser
target_link_libraries(test-VWebSocketParser 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

# test-VWebSocketClient
target_link_libraries(test-VWebSocketClient 
    debug ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_DEBUG_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
    optimized ${LIBUV_LIB_DIR} ${BUILD_UNIX_SYSTEM_LIBS} ${BUILD_UNIX_VLIBUV_RELEASE_LIBS} ${BUILD_UNIX_OPENSSL_LIBS}
)

elseif(WIN32)
if(CMAKE_GENERATOR_PLATFORM MATCHES "x64")
  # 针对64位平台的配置
message(STATUS "Configuring for x64")
# test-VTcpClient
target_link_libraries(test-VTcpClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VTcpServer
target_link_libraries(test-VTcpServer PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)


 # test-VHttpClient
target_link_libraries(test-VHttpClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VHttpServer
target_link_libraries(test-VHttpServer PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VOpenSsl
target_link_libraries(test-VOpenSsl PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VHttpParser
target_link_libraries(test-VHttpParser PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VWebSocketParser
target_link_libraries(test-VWebSocketParser PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

# test-VWebSocketClient
target_link_libraries(test-VWebSocketClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x64_OPENSSL_LIBS}>
)

elseif(CMAKE_GENERATOR_PLATFORM MATCHES "Win32")
# 针对32位平台的配置
message(STATUS "Configuring for Win32")
# test-VTcpClient
target_link_libraries(test-VTcpClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VTcpServer
target_link_libraries(test-VTcpServer PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VHttpClient
target_link_libraries(test-VHttpClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VHttpServer
target_link_libraries(test-VHttpServer PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VOpenSsl
target_link_libraries(test-VOpenSsl PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VHttpParser
target_link_libraries(test-VHttpParser PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VWebSocketParser
target_link_libraries(test-VWebSocketParser PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)

# test-VWebSocketClient
target_link_libraries(test-VWebSocketClient PRIVATE
    $<$<CONFIG:Debug>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_DEBUG_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
    $<$<CONFIG:Release>:${BUILD_WIN32_SYSTEM_LIBS} ${BUILD_WIN32_VLIBUV_RELEASE_LIBS} ${BUILD_WIN32_x86_OPENSSL_LIBS}>
)
endif()


endif()


