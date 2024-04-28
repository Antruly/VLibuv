#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>

#include <thread>
#include "VLogger.h"
#include "VMemory.h"
#include "VObject.h"
#include <queue>
class VMemoryCache;
struct VMemoryCacheBlock;
struct VMemoryCacheFrame;
struct VMemoryCacheUnit;

struct VMemoryCacheUnit {
  enum class VMEMORY_CACHE_FLAGS {
    CACHE_NONE = 0,
    CACHE_IDLE = 1,
    CACHE_USED = 2,
    CACHE_PENDING = 3,
    CACHE_WAIT_FREE = 4,
    CACHE_REPEAL = 0xFF
  };

  void* ptr = nullptr;
  size_t size = 0;
  std::atomic<VMEMORY_CACHE_FLAGS> flag = VMEMORY_CACHE_FLAGS::CACHE_NONE;
};

struct VMemoryCacheFrame {
  VMemoryCacheFrame();
  ~VMemoryCacheFrame();

  VMemoryCacheUnit unit;
  VMemoryCacheFrame* next_frame = nullptr;
  VMemoryCacheFrame* prev_frame = nullptr;

  void init();
  void free();
  void appandFrame(VMemoryCacheFrame* frame);
  void replaceFrame(VMemoryCacheFrame* frame);
  void insertFrame(VMemoryCacheFrame* frame);
  void removeFrame();
};

struct VMemoryCacheBlock {
  VMemoryCacheBlock();
  ~VMemoryCacheBlock();

  VMemoryCacheFrame* header_frame = nullptr;
  VMemoryCacheFrame* last_frame = nullptr;
  VMemoryCacheBlock* next_block = nullptr;
  VMemoryCacheBlock* prev_block = nullptr;
  // <type, num> eg: <65536, 2>
  std::unordered_map<size_t, size_t> frame_type_num;
  std::unordered_map<size_t, size_t> frame_idle_type_num;
  size_t memory_size = 0;
  size_t memory_idle_size = 0;
  size_t memory_used_size = 0;
  size_t frame_count = 0;
  size_t idle_frame_count = 0;
  size_t used_frame_count = 0;
  size_t wait_free_frame_count = 0;

  bool appandFrame(VMemoryCacheFrame* new_frame);
  bool appandNewFrame(VMemoryCacheFrame* new_frame, size_t size);
  VMemoryCacheFrame* getIdleFrame(size_t size);

  VMemoryCacheFrame* getIdleFrame(void* ptr);

  void updateInfo();

  void free();
  std::vector<VMemoryCacheFrame*> removeUnIdle();

  void appandBlock(VMemoryCacheBlock* block);
  void replaceBlock(VMemoryCacheBlock* block);
  void insertBlock(VMemoryCacheBlock* block);
  void removeBlock();
};

class VMemoryCache : public VObject {
 public:
  enum class VMEMORY_CACHE_MECHANISM {
    QUEUE_CYCLE,       // 队列循环
    COUNT_PRIORITY,    // 计数优先
    FULL_NOTIFICATION  // 存满告知
  };

 private:
  std::queue<VMemoryCacheFrame*> idle_frame_ptrs;
  std::unordered_map<std::thread::id, VMemoryCacheBlock*> header_block_caches;
  std::unordered_map<size_t, size_t> cache_count;
  std::unordered_map<void*, size_t> cache_ptr;
  size_t maxCacheSize;      // 最大缓存大小
  size_t currentCacheSize;  // 当前缓存大小
  VMEMORY_CACHE_MECHANISM cacheMechanism = VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE;

 protected:
  std::function<void(size_t)> cache_full_callback;

 public:
  VMemoryCache(size_t maxSize);
  ~VMemoryCache();
  void setCacheFullCb(std::function<void(size_t)> cache_full);

  // 申请内存
  void* malloc(size_t size);

  void* realloc(void* ptr, size_t size);

  // 释放内存
  void free(void* ptr);

  // 从缓存中释放空闲内存块
  void cleanCache(size_t size);

  VMemoryCacheBlock* getHeaderCacheBlock(std::thread::id id);
};
