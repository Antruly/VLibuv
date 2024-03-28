cmake_minimum_required(VERSION 3.0)
project(VWeb)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VWeb/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VWeb/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/VWeb/lib)

# install include libuv/win files
file(GLOB PDB_FILE
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug/*.pdb"
    "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release/*.pdb"
)

message(STATUS "Project Binary Directory: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

#get_target_property(VWEB_TARGET_LOCATION VWeb LOCATION)
#set(PDB_FILE "${VWEB_TARGET_LOCATION}.pdb")
#message(STATUS "Executable Location: ${VWEB_TARGET_LOCATION}")

if(MSVC)
    add_compile_options("/Zi")
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(STATUS "Configuring for Debug build")
        target_compile_options(VWeb PRIVATE "/Zi")
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        message(STATUS "Configuring for Release build")
        target_compile_options(VWeb PRIVATE "/Zi")
    else()
    endif()
endif()

# 设置 VCore 路径
set(VCORE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../VCore/src")

# 设置 VNetWork 路径
set(VNETWORK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../VNetWork/src")

# 设置 VWeb 路径
set(VWEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 设置默认的 http_parser 路径
set(DEFAULT_HTTP_PARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/http_parser")
set(DEFAULT_HTTP_PARSER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/http_parser")
set(HTTP_PARSER_DIR "" CACHE PATH "Path to http_parser directory")
set(HTTP_PARSER_INCLUDE_DIR "${HTTP_PARSER_DIR}")
set(HTTP_PARSER_SOURCE_DIR "${HTTP_PARSER_DIR}")

# 如果未提供路径，则使用默认路径
if(NOT HTTP_PARSER_DIR)
    set(HTTP_PARSER_INCLUDE_DIR ${DEFAULT_HTTP_PARSER_INCLUDE_DIR})
    set(HTTP_PARSER_SOURCE_DIR ${DEFAULT_HTTP_PARSER_SOURCE_DIR})
endif()


# 设置默认的 websocket_parser 路径
set(DEFAULT_WEBSOCKET_PARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/websocket_parser")
set(DEFAULT_WEBSOCKET_PARSER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/websocket_parser")
set(WEBSOCKET_PARSER_DIR "" CACHE PATH "Path to websocket_parser directory")
set(WEBSOCKET_PARSER_INCLUDE_DIR "${WEBSOCKET_PARSER_DIR}")
set(WEBSOCKET_PARSER_SOURCE_DIR "${WEBSOCKET_PARSER_DIR}")
# 如果未提供路径，则使用默认路径
if(NOT WEBSOCKET_PARSER_DIR)
    set(WEBSOCKET_PARSER_INCLUDE_DIR ${DEFAULT_WEBSOCKET_PARSER_INCLUDE_DIR})
    set(WEBSOCKET_PARSER_SOURCE_DIR ${DEFAULT_WEBSOCKET_PARSER_SOURCE_DIR})
endif()


# 设置默认的 zlib 路径
set(DEFAULT_ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
set(DEFAULT_ZLIB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/zlib")
set(ZLIB_DIR "" CACHE PATH "Path to zlib directory")
set(ZLIB_INCLUDE_DIR "${ZLIB_DIR}")
set(ZLIB_SOURCE_DIR "${ZLIB_DIR}")

# 如果未提供路径，则使用默认路径
if(NOT ZLIB_DIR)
    set(ZLIB_INCLUDE_DIR ${DEFAULT_ZLIB_INCLUDE_DIR})
    set(ZLIB_SOURCE_DIR ${DEFAULT_ZLIB_SOURCE_DIR})
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



# VWeb
set(VWEB_FILES
    ${VWEB_SRC_DIR}/VHttpClient.cpp
    ${VWEB_SRC_DIR}/VHttpClient.h
    ${VWEB_SRC_DIR}/VHttpParser.cpp
    ${VWEB_SRC_DIR}/VHttpParser.h
    ${VWEB_SRC_DIR}/VHttpRequest.cpp
    ${VWEB_SRC_DIR}/VHttpRequest.h
    ${VWEB_SRC_DIR}/VHttpResponse.cpp
    ${VWEB_SRC_DIR}/VHttpResponse.h
    ${VWEB_SRC_DIR}/VHttpServer.cpp
    ${VWEB_SRC_DIR}/VHttpServer.h
    ${VWEB_SRC_DIR}/VWeb.cpp
    ${VWEB_SRC_DIR}/VWeb.h
    ${VWEB_SRC_DIR}/VWebDefine.h
    ${VWEB_SRC_DIR}/VWebSocketParser.cpp
    ${VWEB_SRC_DIR}/VWebSocketParser.h
    ${VWEB_SRC_DIR}/VZlib.cpp
    ${VWEB_SRC_DIR}/VZlib.h
    ${VWEB_SRC_DIR}/VOpenSsl.cpp
    ${VWEB_SRC_DIR}/VOpenSsl.h
)
source_group(VWeb FILES ${VWEB_FILES})

# http_parser
set(HTTP_PARSER_FILES
    ${HTTP_PARSER_SOURCE_DIR}/http_parser.c
    ${HTTP_PARSER_INCLUDE_DIR}/http_parser.h
)
source_group(http_parser FILES ${HTTP_PARSER_FILES})

# websocket_parser
set(WEBSOCKET_PARSER_FILES
    ${WEBSOCKET_PARSER_SOURCE_DIR}/websocket_parser.c
    ${WEBSOCKET_PARSER_INCLUDE_DIR}/websocket_parser.h
)
source_group(websocket_parser FILES ${WEBSOCKET_PARSER_FILES})

# zlib
file(GLOB ZLIB_FILES
    "${ZLIB_SOURCE_DIR}/*.h"
    "${ZLIB_SOURCE_DIR}/*.c"
)
source_group(zlib FILES ${ZLIB_FILES})

# # # # # # # # # # # # install# # # # # # # # # # # # 
# install include vcore files
file(GLOB INSTALL_INCLUDE_VWEB_FILES
    "${VWEB_SRC_DIR}/*.h"
)
# install include http_parser files
file(GLOB INSTALL_INCLUDE_HTTP_PARSER_FILES
    "${HTTP_PARSER_SOURCE_DIR}/*.h"
    "${HTTP_PARSER_INCLUDE_DIR}/*.h"
)

# install include websocket_parser files
file(GLOB INSTALL_INCLUDE_WEBSOCKET_PARSER_FILES
    "${WEBSOCKET_PARSER_SOURCE_DIR}/*.h"
    "${WEBSOCKET_PARSER_INCLUDE_DIR}/*.h"
)

# install include zlib files
file(GLOB INSTALL_INCLUDE_ZLIB_FILES
    "${ZLIB_SOURCE_DIR}/*.h"
    "${ZLIB_INCLUDE_DIR}/*.h"
)

# install include openssl files
file(GLOB INSTALL_INCLUDE_OPENSSL_FILES
    "${OPENSSL_INCLUDE_DIR}/openssl/*.h"
)

file(GLOB INSTALL_LIB_OPENSSL_FILES
    "${OPENSSL_LIB_DIR}/*.lib"
    "${OPENSSL_LIB_DIR}/*.a"
    "${OPENSSL_LIB_DIR}/*.pdb"
)

# # # # # # # # # # # # install# # # # # # # # # # # # 

# 设置包含文件路径
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VCORE_SRC_DIR}
    ${VNETWORK_SRC_DIR}
    ${LIBUV_DIR}/include
    ${LIBUV_DIR}/src
    ${HTTP_PARSER_INCLUDE_DIR}
    ${WEBSOCKET_PARSER_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}

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
add_library(VWeb STATIC ${VWEB_FILES} ${HTTP_PARSER_FILES} ${WEBSOCKET_PARSER_FILES} ${ZLIB_FILES})

target_link_libraries(VWeb
    pthread   # 线程库
    dl        # 动态链接库
)


elseif(WIN32)
add_library(VWeb STATIC ${VWEB_FILES} ${HTTP_PARSER_FILES} ${WEBSOCKET_PARSER_FILES} ${ZLIB_FILES})

target_link_libraries(VWeb
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

endif()



# 将可执行文件安装到 bin 目录
#install(TARGETS VWeb DESTINATION bin)

# 将库文件安装到 lib 目录
install(TARGETS VWeb DESTINATION lib)

# 在调试模式下安装 .pdb 文件
install(FILES ${PDB_FILE} DESTINATION lib OPTIONAL) 

# 将头文件安装到 include 目录
install(FILES 
        ${INSTALL_INCLUDE_VWEB_FILES}
        # include
        DESTINATION include/VWeb)
install(FILES 
        ${INSTALL_INCLUDE_HTTP_PARSER_FILES}
        # include/http_parser
        DESTINATION include/http_parser)
install(FILES 
        ${INSTALL_INCLUDE_WEBSOCKET_PARSER_FILES}
        # include/websocket_parser
        DESTINATION include/websocket_parser)
install(FILES 
        ${INSTALL_INCLUDE_ZLIB_FILES}
        # include/zlib
        DESTINATION include/zlib)
install(FILES 
        ${INSTALL_INCLUDE_OPENSSL_FILES}
        # include/openssl/openssl
        DESTINATION include/openssl/openssl)
install(FILES 
        ${INSTALL_LIB_OPENSSL_FILES}
        # include/lib
        DESTINATION lib)

