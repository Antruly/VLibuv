#pragma once
extern "C" {
#include "..\\..\\libuv\\include\uv.h"
}
#define _VLIBUV_INCLUDE
#ifndef _VLIBUV_INCLUDE
#define _VLIBUV_INCLUDE

// windows
#ifdef WIN32 

// debug
#ifdef _DEBUG

// x64
#ifdef _WIN64
extern "C" {
#include "../../libuv/x64/include/uv.h"
#pragma comment(lib, "../../libuv/x64/lib/uv.lib")
#pragma comment(lib, "../../libuv/x64/lib/libuv.lib")
}
// x86
#else
extern "C" {
#include "../../libuv/x86/include/uv.h"
#pragma comment(lib, "../../libuv/x86/lib/uv.lib")
#pragma comment(lib, "../../libuv/x86/lib/libuv.lib")
}
#endif
// release
#else       
// x64
#ifdef _WIN64
extern "C" {
#include "../../libuv/x64/include/uv.h"
#pragma comment(lib, "../../libuv/x64/lib/uv.lib")
#pragma comment(lib, "../../libuv/x64/lib/libuv.lib")
}
// x86
#else         
extern "C" {
#include "../../libuv/x86/include/uv.h"
#pragma comment(lib, "../../libuv/x86/lib/uv.lib")
#pragma comment(lib, "../../libuv/x86/lib/libuv.lib")
}
#endif
#endif
// Not WIN32
#else

#endif

#endif // !_VLIBUV_INCLUDE


#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>