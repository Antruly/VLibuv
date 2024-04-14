#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <atomic>
#include "VObject.h"
#include "VMemory.h"

class VMemoryCache;


struct VMemoryUnit
{

};

struct VMemoryCacheBlock
{

};
struct VMemoryCacheFrame {
	void* memory_ptr = nullptr;
	VMemoryCache* memory_cahce_ptr = nullptr;
	size_t memory_size = 0;
};

class VMemoryCache : public VObject {
public:

private:

};



class VMemoryCache {
public:
    enum class VMEMORY_CACHE_MECHANISM {
        QUEUE_CYCLE,     // 队列循环
        COUNT_PRIORITY,  // 计数优先
        FULL_NOTIFICATION  // 存满告知
    };
private:
    std::unordered_map<size_t, std::vector<std::pair<void*, std::atomic<bool>>>> cache; // 缓存内存块的容器
    std::unordered_map <size_t, size_t> cache_count;
    std::unordered_map <void*, size_t> cache_ptr;
    size_t maxCacheSize; // 最大缓存大小
    size_t currentCacheSize; // 当前缓存大小
    VMEMORY_CACHE_MECHANISM cacheMechanism = VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE;
protected:
    std::function<void(size_t)> cache_full_callback;
public:
    VMemoryCache(size_t maxSize) : maxCacheSize(maxSize), currentCacheSize(0) {}

    void setCacheFullCb(std::function<void(size_t)> cache_full) {
        cache_full_callback = cache_full;
    }

    // 申请内存
    void* malloc(size_t size) {
        if (size > maxCacheSize) {
            return VMemory::malloc(size);
        }

        if (cache.find(size) != cache.end() && !cache[size].empty()) {
            for (auto& memBlock : cache[size]) {
                if (!memBlock.second.exchange(true)) {
                    currentCacheSize -= size; // 从缓存中分配内存时，更新当前缓存大小
                    return memBlock.first;
                }
            }
        }

        void* ptr = VMemory::malloc(size);
        if (ptr = nullptr) {
            return ptr;
        }

        if (currentCacheSize + size > maxCacheSize) {
            cleanCache(size - (maxCacheSize - currentCacheSize));
        }
        if (currentCacheSize + size <= maxCacheSize) {
            addToCache(ptr, size);
        }

        return ptr;
    }

    void* realloc(void* ptr, size_t size) {
        if (size > maxCacheSize) {
            return VMemory::realloc(ptr, size);
        }

        if (cache.find(size) != cache.end() && !cache[size].empty()) {
            for (auto& memBlock : cache[size]) {
                if (!memBlock.second.exchange(true)) {
                    currentCacheSize -= size; // 从缓存中分配内存时，更新当前缓存大小
                    this->free(ptr);
                    return memBlock.first;
                }
            }
        }

        void* ptr = VMemory::realloc(ptr, size);
        if (ptr = nullptr) {
            return ptr;
        }

        if (currentCacheSize + size > maxCacheSize) {
            cleanCache(size - (maxCacheSize - currentCacheSize));
        }
        if (currentCacheSize + size <= maxCacheSize) {
            addToCache(ptr, size);
        }

        return ptr;
    }

    // 释放内存
    void free(void* ptr) {
        if (ptr = nullptr) {
            return;
        }
        
        if (cache_ptr.find(ptr) == cache_ptr.end()) {
            return;
        }
        
        if (cache_ptr[ptr] <= maxCacheSize && cache.find(cache_ptr[ptr]) != cache.end()) {
            for (auto& memBlock : cache[cache_ptr[ptr]]) {
                if (memBlock.first == ptr) {
                    memBlock.second.store(false);
                    currentCacheSize += cache_ptr[ptr]; // 释放内存时，更新当前缓存大小
                    return;
                }
            }
        }
        VMemory::free(ptr);
    }

    // 添加内存块到缓存
    void addToCache(void* ptr, size_t size) {
        if (size <= maxCacheSize && currentCacheSize + size <= maxCacheSize) {
            cache_ptr[ptr] = size;
            cache[size].emplace_back(ptr, false);
            currentCacheSize += size;
        }
    }

    // 从缓存中释放空闲内存块
    void cleanCache(size_t size) {
        if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE) {
            for (auto it = cache.begin(); it != cache.end() && size > 0;) {
                auto& memBlocks = it->second;
                for (auto memBlockIt = memBlocks.begin(); memBlockIt != memBlocks.end();) {
                    if (!memBlockIt->second) {
                        currentCacheSize -= it->first;
                        memBlockIt = memBlocks.erase(memBlockIt);
                        if (size < it->first) {
                            size = 0;
                            break;
                        }
                        else {
                            size -= it->first;
                        }
                    }
                    else {
                        ++memBlockIt;
                    }
                }
                if (memBlocks.empty()) {
                    it = cache.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        else if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::COUNT_PRIORITY) {
            if (cache_count.find(size) != cache_count.end()) {
                ++cache_count[size];
            }
            else {
                cache_count[size] = 0;
            }

            for (auto it = cache.begin(); it != cache.end() && size > 0;) {
                auto& memBlocks = it->second;
                for (auto memBlockIt = memBlocks.begin(); memBlockIt != memBlocks.end();) {
                    if (!memBlockIt->second) {
                        currentCacheSize -= it->first;
                        memBlockIt = memBlocks.erase(memBlockIt);
                        if (size < it->first) {
                            size = 0;
                            break;
                        }
                        else {
                            size -= it->first;
                        }
                    }
                    else {
                        ++memBlockIt;
                    }
                }
                if (memBlocks.empty()) {
                    it = cache.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        else if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::FULL_NOTIFICATION) {
            if (cache_full_callback) {
                cache_full_callback(size);
            }
        }

       
    }
};
