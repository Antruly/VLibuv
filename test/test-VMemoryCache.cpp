#pragma once
#include "VLibuv.h"
#include "VLogger.h"
#include "VMemory.h"
#include "VMemoryCache.h"

VMemoryCache caceh(1024 * 1024 * 1024);

// 不使用缓存的情况
double withoutCache() {
    clock_t start_time = clock();
    for (int i = 0; i < 10000; ++i) {
        int var = 65536; // 随机定义变量
        void* ptr = VMemory::malloc(var);
        VMemory::free(ptr);
    }

    clock_t end_time = clock();
    return double(end_time - start_time);
}

// 使用内存缓存的情况
double withCache() {
 
    clock_t start_time = clock();
    for (int i = 0; i < 10000; ++i) {
        int var = 65536; // 随机定义变量
        void* ptr = caceh.malloc(var);
        caceh.free(ptr);
    }
    clock_t end_time = clock();
    return double(end_time - start_time);
}

int main() {
    srand(time(0)); // 设置随机种子

    double timeWithoutCache = withoutCache();
    double timeWithCache = withCache();

    VLogger::Log->logInfo("不使用缓存的耗时：%lf ms", timeWithoutCache);
    VLogger::Log->logInfo("使用缓存的耗时：%lf ms", timeWithCache);

    return 0;
}
