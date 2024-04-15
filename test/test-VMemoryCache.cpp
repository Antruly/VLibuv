#pragma once
#include "VLibuv.h"
#include "VLogger.h"
#include "VMemory.h"
#include "VMemoryCache.h"

double withoutCache() {
  clock_t start_time = clock();
  for (int i = 0; i < 100000; ++i) {
    int var = 6553 + rand() % 1000 - rand() % 1000;  // 随机定义变量
    void* ptr = std::malloc(var);

    int var2 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr2 = std::malloc(var2);

    int var3 = 655 + rand() % 100 - rand() % 100;  // 随机定义变量
    void* ptr3 = std::malloc(var3);

    std::free(ptr);

    int var4 = 6553600 + rand() % 1000000 - rand() % 1000000;  // 随机定义变量
    void* ptr4 = std::malloc(var4);

    int var5 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr5 = std::malloc(var5);

    std::free(ptr2);
    std::free(ptr3);
    std::free(ptr4);
    std::free(ptr5);
  }

  clock_t end_time = clock();
  return double(end_time - start_time);
}

double withPool() {
  clock_t start_time = clock();
  for (int i = 0; i < 100000; ++i) {
    int var = 6553 + rand() % 1000 - rand() % 1000;  // 随机定义变量
    void* ptr = VMemory::malloc(var);

    int var2 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr2 = VMemory::malloc(var2);

    int var3 = 655 + rand() % 100 - rand() % 100;  // 随机定义变量
    void* ptr3 = VMemory::malloc(var3);

    VMemory::free(ptr);

    int var4 = 6553600 + rand() % 1000000 - rand() % 1000000;  // 随机定义变量
    void* ptr4 = VMemory::malloc(var4);

    int var5 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr5 = VMemory::malloc(var5);

    VMemory::free(ptr2);
    VMemory::free(ptr3);
    VMemory::free(ptr4);
    VMemory::free(ptr5);
  }

  clock_t end_time = clock();
  return double(end_time - start_time);
}

double withCachePool(VMemoryCache& caceh) {
  clock_t start_time = clock();
  for (int i = 0; i < 100000; ++i) {
    int var = 6553 + rand() % 1000 - rand() % 1000;  // 随机定义变量
    void* ptr = caceh.malloc(var);

    int var2 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr2 = caceh.malloc(var2);

    int var3 = 655 + rand() % 100 - rand() % 100;  // 随机定义变量
    void* ptr3 = caceh.malloc(var3);

    caceh.free(ptr);

    int var4 = 6553600 + rand() % 1000000 - rand() % 1000000;  // 随机定义变量
    void* ptr4 = caceh.malloc(var4);

    int var5 = 65536 + rand() % 10000 - rand() % 10000;  // 随机定义变量
    void* ptr5 = caceh.malloc(var5);

    caceh.free(ptr2);
    caceh.free(ptr3);
    caceh.free(ptr4);
    caceh.free(ptr5);
  }
  clock_t end_time = clock();
  return double(end_time - start_time);
}

int main() {
  {
    srand(time(0));  // 设置随机种子
    VMemoryCache caceh(1024 * 1024 * 1024);

    double timeWithoutCache = withoutCache();
    double timeWithPool = withPool();
    double timeWithCachePool = withCachePool(caceh);

    VLogger::Log->logInfo("系统原生耗时：%lf ms", timeWithoutCache);
    VLogger::Log->logInfo("使用内存池耗时：%lf ms", timeWithPool);
    VLogger::Log->logInfo("使用缓存加内存池耗时：%lf ms", timeWithCachePool);

    VMemoryCacheBlock* block =
        caceh.getHeaderCacheBlock(std::this_thread::get_id());
    block->updateInfo();
  }

  return 0;
}
