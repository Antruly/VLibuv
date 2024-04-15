#include "VMemoryPool.h"
#include "VLogger.h"
#include "VMemoryCache.h"

VMemoryCache::VMemoryCache(size_t maxSize)
    : maxCacheSize(maxSize), currentCacheSize(0) {}

VMemoryCache::~VMemoryCache() {
  this->cleanCache(currentCacheSize);
  while (!idle_frame_ptrs.empty()) {
    delete idle_frame_ptrs.front();
    idle_frame_ptrs.pop();
  }
}

void VMemoryCache::setCacheFullCb(
    std::function<void(size_t)> cache_full) {
  cache_full_callback = cache_full;
}

// 申请内存
void* VMemoryCache::malloc(size_t size) {
  if (size > maxCacheSize) {
    return VMemory::malloc(size);
  }
  VMemoryCacheFrame* frame = nullptr;

  // 查询是否存在缓存内存块
  if (header_block_caches.find(std::this_thread::get_id()) !=
          header_block_caches.end() &&
      header_block_caches[std::this_thread::get_id()] != nullptr) {
  
     frame = header_block_caches[std::this_thread::get_id()]->getIdleFrame(size);
    if (frame != nullptr) {
      // 命中缓存帧
      frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_USED;
      return frame->unit.ptr;
    }
  } 
  // 创建缓存内存块
  else {
    header_block_caches[std::this_thread::get_id()] = new VMemoryCacheBlock();
    if (header_block_caches[std::this_thread::get_id()] == nullptr) {
      header_block_caches.erase(std::this_thread::get_id());
      return nullptr;
    }
  }

   // 未命中缓存帧
  if (currentCacheSize + size > maxCacheSize) {
    cleanCache(size - (maxCacheSize - currentCacheSize));
  }
  // 优先从空闲缓存内存队列拿现有的
  if (idle_frame_ptrs.size() == 0) {
    frame = new VMemoryCacheFrame();
    if (frame == nullptr) {
      return nullptr;
    }
  } else {
    frame = idle_frame_ptrs.front();
    idle_frame_ptrs.pop();
  }
  // 附加缓存帧至内存块（内部自动申请内存大小为size）
  if (header_block_caches[std::this_thread::get_id()]->appandNewFrame(frame,
                                                                      size)) {
    currentCacheSize += frame->unit.size;
    return frame->unit.ptr;
  } else {
    frame->init();
    idle_frame_ptrs.push(frame);
  }
  
  return nullptr;
}

void* VMemoryCache::realloc(void* ptr, size_t size) {
  if (size > maxCacheSize) {
    return VMemory::realloc(ptr, size);
  }
  if (header_block_caches.find(std::this_thread::get_id()) !=
          header_block_caches.end() &&
      header_block_caches[std::this_thread::get_id()] != nullptr) {
    auto frame_old =
        header_block_caches[std::this_thread::get_id()]->getIdleFrame(ptr);
    if (frame_old != nullptr) {
      if (frame_old->unit.size >= size) {
        frame_old->unit.ptr = VMemory::realloc(ptr, size);
        if (frame_old->unit.ptr != nullptr) {
          currentCacheSize -= frame_old->unit.size - size;
          frame_old->unit.size = size;
          return frame_old->unit.ptr;
        } else {
          frame_old->unit.ptr = ptr;
          return nullptr;
        }
      } else {
        if ((size - frame_old->unit.size) + currentCacheSize > maxCacheSize) {
          this->cleanCache((size - frame_old->unit.size) + currentCacheSize -
                           maxCacheSize);
        }
        frame_old->unit.ptr = VMemory::realloc(ptr, size);
        if (frame_old->unit.ptr != nullptr) {
          currentCacheSize += size - frame_old->unit.size;
          frame_old->unit.size = size;
          return frame_old->unit.ptr;
        } else {
          frame_old->unit.ptr = ptr;
          return nullptr;
        }
      }
    }
  }

  return VMemory::realloc(ptr, size);
}

// 释放内存

void VMemoryCache::free(void* ptr) {
  if (ptr == nullptr) {
    return;
  }

  if (header_block_caches.find(std::this_thread::get_id()) !=
          header_block_caches.end() &&
      header_block_caches[std::this_thread::get_id()] != nullptr) {
    auto frame =
        header_block_caches[std::this_thread::get_id()]->getIdleFrame(ptr);
    if (frame != nullptr) {
      frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE;
      return;
    }
  }
  VMemory::free(ptr);
}

// 从缓存中释放空闲内存块
void VMemoryCache::cleanCache(size_t size) {
  if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE) {
    for (auto it = header_block_caches.begin();
         it != header_block_caches.end() && size > 0;) {
      auto& memBlocks = it->second;
      VMemoryCacheFrame* next_frame = memBlocks->header_frame;
      VMemoryCacheFrame* this_frame = nullptr;
      while (next_frame != nullptr && size > 0) {
        this_frame = next_frame;
        next_frame = this_frame->next_frame;

        if (this_frame->unit.flag ==
            VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
          currentCacheSize -= this_frame->unit.size;
          if (size < this_frame->unit.size) {
            size = 0;
          } else {
            size -= this_frame->unit.size;
          }
          this_frame->free();
          if (this_frame == memBlocks->header_frame) {
            memBlocks->header_frame = memBlocks->header_frame->next_frame;
          }
          if (this_frame == memBlocks->last_frame) {
            memBlocks->last_frame = memBlocks->last_frame->prev_frame;
          }
          this_frame->removeFrame();

          if (idle_frame_ptrs.size() > 10) {
            delete this_frame;
            this_frame = nullptr;
          } else {
            this_frame->init();
            idle_frame_ptrs.push(this_frame);
          }
          continue;
        }
      }
      ++it;
    }
  } else if (cacheMechanism ==
             VMemoryCache::VMEMORY_CACHE_MECHANISM::COUNT_PRIORITY) {
    if (cache_count.find(size) != cache_count.end()) {
      ++cache_count[size];
    } else {
      cache_count[size] = 0;
    }

    for (auto it = header_block_caches.begin();
         it != header_block_caches.end() && size > 0;) {
      auto& memBlocks = it->second;
      VMemoryCacheFrame* next_frame = memBlocks->header_frame;
      VMemoryCacheFrame* this_frame = nullptr;
      while (next_frame != nullptr && size > 0) {
        this_frame = next_frame;
        next_frame = this_frame->next_frame;

        if (this_frame->unit.flag ==
            VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
          currentCacheSize -= this_frame->unit.size;
          if (size < this_frame->unit.size) {
            size = 0;
          } else {
            size -= this_frame->unit.size;
          }
          this_frame->free();
          if (this_frame == memBlocks->header_frame) {
            memBlocks->header_frame = memBlocks->header_frame->next_frame;
          }
          if (this_frame == memBlocks->last_frame) {
            memBlocks->last_frame = memBlocks->last_frame->prev_frame;
          }
          this_frame->removeFrame();
          if (idle_frame_ptrs.size() > 10) {
            delete this_frame;
            this_frame = nullptr;
          } else {
            this_frame->init();
            idle_frame_ptrs.push(this_frame);
          }
          continue;
        }
      }
      ++it;
    }
  } else if (cacheMechanism ==
             VMemoryCache::VMEMORY_CACHE_MECHANISM::FULL_NOTIFICATION) {
    if (cache_full_callback) {
      cache_full_callback(size);
    }
  }
}

VMemoryCacheBlock* VMemoryCache::getHeaderCacheBlock(std::thread::id id) {
  if (header_block_caches.find(id) != header_block_caches.end())
    return header_block_caches[id];
  else {
    return nullptr;
  }
}

VMemoryCacheFrame::VMemoryCacheFrame()
    : unit(), next_frame(nullptr), prev_frame(nullptr) {}

VMemoryCacheFrame::~VMemoryCacheFrame() {
  this->free();
}

void VMemoryCacheFrame::init() {
  unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_NONE;
  unit.ptr = nullptr;
  unit.size = 0;
  next_frame = nullptr;
  prev_frame = nullptr;
}

void VMemoryCacheFrame::free() {
  if (unit.ptr ==
          nullptr ||unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_REPEAL) {
    return;
  }

  while (unit.flag != VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_WAIT_FREE;
  VMemory::free(unit.ptr);
  unit.ptr = nullptr;
  unit.size = 0;
  unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_REPEAL;
}

void VMemoryCacheFrame::appandFrame(VMemoryCacheFrame* frame) {
  if (next_frame != nullptr) {
    VLogger::Log->logWarn(
        "AppandFrame VMemoryCacheFrame, this next_frame is not nullptr. You "
        "can call insertFrame");
  }
  if (frame->prev_frame != nullptr) {
    VLogger::Log->logWarn(
        "AppandFrame VMemoryCacheFrame, frame prev_frame is not nullptr. You "
        "can call insertFrame");
  }
  this->next_frame = frame;
  frame->prev_frame = this;
}

void VMemoryCacheFrame::replaceFrame(VMemoryCacheFrame* frame) {
  if (next_frame != nullptr) {
    frame->next_frame = next_frame;
    next_frame->prev_frame = frame;
  }

  if (prev_frame != nullptr) {
    frame->prev_frame = prev_frame;
    prev_frame->next_frame = frame;
  }
}

void VMemoryCacheFrame::insertFrame(VMemoryCacheFrame* frame) {
  if (next_frame != nullptr) {
    frame->next_frame = next_frame;
    next_frame->prev_frame = frame;
  }
  this->next_frame = frame;
  frame->prev_frame = this;
}

void VMemoryCacheFrame::removeFrame() {
  if (next_frame != nullptr) {
    next_frame->prev_frame = prev_frame;
  }

  if (prev_frame != nullptr) {
    prev_frame->next_frame = next_frame;
  }
}

VMemoryCacheBlock::VMemoryCacheBlock()
    : header_frame(nullptr),
      next_block(nullptr),
      prev_block(nullptr),
      frame_type_num(),
      frame_idle_type_num(),
      memory_size(0),
      memory_idle_size(0),
      memory_used_size(0),
      frame_count(0),
      idle_frame_count(0),
      used_frame_count(0),
      wait_free_frame_count(0) {}

VMemoryCacheBlock::~VMemoryCacheBlock() {
  this->free();
}

bool VMemoryCacheBlock::appandFrame(VMemoryCacheFrame* new_frame) {
  if (new_frame == nullptr) {
    return false;
  }
    if (new_frame->unit.ptr == nullptr || new_frame->unit.size == 0) {
    return false;
  }
    if (header_frame == nullptr) {
    header_frame = new_frame;
    last_frame = new_frame;
  } else {
    new_frame->prev_frame = last_frame;
    last_frame->next_frame = new_frame;
    last_frame = new_frame;
  }
  new_frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE;
  return true;
}

bool VMemoryCacheBlock::appandNewFrame(
    VMemoryCacheFrame* new_frame, size_t size) {
    if (new_frame == nullptr) {
    return false;
  }
  void* ptr = VMemory::malloc(size);
  if (ptr == nullptr) {
    return false;
  }

  if (header_frame == nullptr) {
    header_frame = new_frame;
    last_frame = new_frame;
  } else {
    new_frame->prev_frame = last_frame;
    last_frame->next_frame = new_frame;
    last_frame = new_frame;
  }

  new_frame->unit.ptr = ptr;
  new_frame->unit.size = size;
  new_frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE;

  return true;
}

VMemoryCacheFrame* VMemoryCacheBlock::getIdleFrame(size_t size) {
  VMemoryCacheFrame* next_frame = header_frame;
  while (next_frame != nullptr) {
    if (next_frame->unit.flag ==
            VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE &&
        next_frame->unit.size >= size && next_frame->unit.size < size *2) {
      return next_frame;
    }
    next_frame = next_frame->next_frame;
  }
  return nullptr;
}

VMemoryCacheFrame* VMemoryCacheBlock::getIdleFrame(void* ptr) {
  VMemoryCacheFrame* next_frame = header_frame;
  while (next_frame != nullptr) {
    if (next_frame->unit.ptr == ptr) {
      return next_frame;
    }
    next_frame = next_frame->next_frame;
  }

  return nullptr;
}

void VMemoryCacheBlock::updateInfo() {
  VMemoryCacheFrame* next_frame = header_frame;
  memory_size = 0;
  memory_idle_size = 0;
  memory_used_size = 0;
  frame_count = 0;
  idle_frame_count = 0;
  used_frame_count = 0;
  wait_free_frame_count = 0;
  frame_type_num.clear();
  frame_idle_type_num.clear();

  while (next_frame != nullptr) {
    memory_size += next_frame->unit.size;
    ++frame_count;

    switch (next_frame->unit.flag) {
      case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE:
        memory_idle_size += next_frame->unit.size;
        frame_type_num[next_frame->unit.size] += 1;
        frame_idle_type_num[next_frame->unit.size] += 1;
        ++idle_frame_count;
        break;
      case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_USED:
        memory_used_size += next_frame->unit.size;
        frame_type_num[next_frame->unit.size] += 1;
        ++used_frame_count;
        break;
      case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_WAIT_FREE:
        frame_type_num[next_frame->unit.size] += 1;
        ++wait_free_frame_count;
        break;
      default:
        frame_type_num[next_frame->unit.size] += 1;
        break;
    }

    next_frame = next_frame->next_frame;
  }
}

void VMemoryCacheBlock::free() {
  VMemoryCacheFrame* next_frame = header_frame;
  while (next_frame != nullptr) {
    next_frame->free();
    next_frame = next_frame->next_frame;
  }
}

std::vector<VMemoryCacheFrame*> VMemoryCacheBlock::removeUnIdle() {
  std::vector<VMemoryCacheFrame*> removeFrames;
  VMemoryCacheFrame* next_frame = header_frame;
  while (next_frame != nullptr) {
    if (next_frame->unit.flag !=
        VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
      next_frame->removeFrame();
      removeFrames.push_back(next_frame);
    }

    next_frame = next_frame->next_frame;
  }

  return removeFrames;
}

void VMemoryCacheBlock::appandBlock(VMemoryCacheBlock* block) {
  if (next_block != nullptr) {
    VLogger::Log->logWarn(
        "AppandBlock VMemoryCacheBlock, this next_block is not nullptr. You "
        "can call insertBlock");
  }
  if (block->prev_block != nullptr) {
    VLogger::Log->logWarn(
        "AppandBlock VMemoryCacheBlock, block prev_block is not nullptr. You "
        "can call insertBlock");
  }
  this->next_block = block;
  block->prev_block = this;
}

void VMemoryCacheBlock::replaceBlock(VMemoryCacheBlock* block) {
  if (next_block != nullptr) {
    block->next_block = next_block;
    next_block->prev_block = block;
  }

  if (prev_block != nullptr) {
    block->prev_block = prev_block;
    prev_block->next_block = block;
  }
}

void VMemoryCacheBlock::insertBlock(VMemoryCacheBlock* block) {
  if (next_block != nullptr) {
    block->next_block = next_block;
    next_block->prev_block = block;
  }
  this->next_block = block;
  block->prev_block = this;
}

void VMemoryCacheBlock::removeBlock() {
  if (next_block != nullptr) {
    next_block->prev_block = prev_block;
  }

  if (prev_block != nullptr) {
    prev_block->next_block = next_block;
  }
}
