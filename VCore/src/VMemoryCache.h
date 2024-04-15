#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <atomic>

#include "VObject.h"
#include "VMemory.h"
#include "VLogger.h"
#include <thread>
class VMemoryCache;
class VMemoryCacheBlock;
class VMemoryCacheFrame;
class VMemoryCacheUnit;



struct VMemoryCacheUnit
{
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

struct VMemoryCacheFrame
{
    VMemoryCacheFrame():unit(), next_frame(nullptr), prev_frame(nullptr){
       
    }
    ~VMemoryCacheFrame() { this->free(); }

    VMemoryCacheUnit unit;
    VMemoryCacheFrame* next_frame = nullptr;
    VMemoryCacheFrame* prev_frame = nullptr;

    void free() {
        if (unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_REPEAL) {
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
    void appandFrame(VMemoryCacheFrame* frame) {
        if (next_frame != nullptr){
            VLogger::Log->logWarn("AppandFrame VMemoryCacheFrame, this next_frame is not nullptr. You can call insertFrame");
        }
        if (frame->prev_frame != nullptr) {
            VLogger::Log->logWarn("AppandFrame VMemoryCacheFrame, frame prev_frame is not nullptr. You can call insertFrame");
        }
        this->next_frame = frame;
        frame->prev_frame = this;

    }
    void replaceFrame(VMemoryCacheFrame* frame) {
        if (next_frame != nullptr) {
            frame->next_frame = next_frame;
            next_frame->prev_frame = frame;
        }

        if (prev_frame != nullptr) {
            frame->prev_frame = prev_frame;
            prev_frame->next_frame = frame;
        }
    }
    void insertFrame(VMemoryCacheFrame* frame) {
        if (next_frame != nullptr) {
            frame->next_frame = next_frame;
            next_frame->prev_frame = frame;
        }
        this->next_frame = frame;
        frame->prev_frame = this;
    }
    void removeFrame() {
        if (next_frame != nullptr) {
            next_frame->prev_frame = prev_frame;
        }

        if (prev_frame != nullptr) {
            prev_frame->next_frame = next_frame;
        }
    }
};

struct VMemoryCacheBlock
{
    VMemoryCacheBlock() :
        header_frame(nullptr),
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
        wait_free_frame_count(0){

    }
    ~VMemoryCacheBlock() {
        this->free();
    }
   
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

    VMemoryCacheFrame* createFrame(size_t size) {
        VMemoryCacheFrame* new_frame = new VMemoryCacheFrame();
      
        if (header_frame == nullptr) {
            header_frame = new_frame;
            last_frame = new_frame;
        }
        else {
            new_frame->prev_frame = last_frame;
            last_frame->next_frame = new_frame;
            last_frame = new_frame;
        }

        new_frame->unit.ptr = VMemory::malloc(size);
        new_frame->unit.size = size;
        new_frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE;
        return new_frame;
    }
    VMemoryCacheFrame* getIdleFrame(size_t size, bool create = false) {
        VMemoryCacheFrame* next_frame = header_frame;
        while (next_frame != nullptr) {
            if (next_frame->unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE && next_frame->unit.size == size) {
                return next_frame;
            }
            next_frame = next_frame->next_frame;
        }
        if (create) {
            return this->createFrame(size);
        }
        return nullptr;
    }

    VMemoryCacheFrame* getIdleFrame(void* ptr) {
        VMemoryCacheFrame* next_frame = header_frame;
        while (next_frame != nullptr) {
            if (next_frame->unit.ptr == ptr) {
                return next_frame;
            }
            next_frame = next_frame->next_frame;
        }
        
        return nullptr;
    }
   
    void updateInfo() {
        VMemoryCacheFrame* next_frame = header_frame;
        memory_size = 0;
        memory_idle_size = 0;
        memory_used_size = 0;
        frame_count = 0;
        idle_frame_count = 0;
        used_frame_count = 0;
        wait_free_frame_count = 0;

        while (next_frame != nullptr) {
            memory_size += next_frame->unit.size;
            ++frame_count;

            switch (next_frame->unit.flag)
            {
            case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE:
                memory_idle_size += next_frame->unit.size;
                ++idle_frame_count;
                break;
            case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_USED:
                memory_used_size += next_frame->unit.size;
                ++used_frame_count;
                break;
            case VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_WAIT_FREE:
                ++wait_free_frame_count;
                break;
            default:
                break;
            }
            
            next_frame = next_frame->next_frame;
        }
    }

    void free() {
        VMemoryCacheFrame* next_frame = header_frame;
        while (next_frame != nullptr) {
            next_frame->free();
            next_frame = next_frame->next_frame;
        }
    }
    std::vector<VMemoryCacheFrame*> removeUnIdle() {
        std::vector<VMemoryCacheFrame*> removeFrames;
        VMemoryCacheFrame* next_frame = header_frame;
        while (next_frame != nullptr) {
            if (next_frame->unit.flag != VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
                next_frame->removeFrame();
                removeFrames.push_back(next_frame);
            }
           
            next_frame = next_frame->next_frame;
        }

        return removeFrames;
    }

    void appandBlock(VMemoryCacheBlock* block) {
        if (next_block != nullptr) {
            VLogger::Log->logWarn("AppandBlock VMemoryCacheBlock, this next_block is not nullptr. You can call insertBlock");
        }
        if (block->prev_block != nullptr) {
            VLogger::Log->logWarn("AppandBlock VMemoryCacheBlock, block prev_block is not nullptr. You can call insertBlock");
        }
        this->next_block = block;
        block->prev_block = this;

    }
    void replaceBlock(VMemoryCacheBlock* block) {
        if (next_block != nullptr) {
            block->next_block = next_block;
            next_block->prev_block = block;
        }

        if (prev_block != nullptr) {
            block->prev_block = prev_block;
            prev_block->next_block = block;
        }
    }
    void insertBlock(VMemoryCacheBlock* block) {
        if (next_block != nullptr) {
            block->next_block = next_block;
            next_block->prev_block = block;
        }
        this->next_block = block;
        block->prev_block = this;
    }
    void removeBlock() {
        if (next_block != nullptr) {
            next_block->prev_block = prev_block;
        }

        if (prev_block != nullptr) {
            prev_block->next_block = next_block;
        }
    }
};



class VMemoryCache : public VObject {
public:
    enum class VMEMORY_CACHE_MECHANISM {
        QUEUE_CYCLE,     // 队列循环
        COUNT_PRIORITY,  // 计数优先
        FULL_NOTIFICATION  // 存满告知
    };
private:
    std::unordered_map<std::thread::id, VMemoryCacheBlock*> header_block_caches;
    std::unordered_map <size_t, size_t> cache_count;
    std::unordered_map <void*, size_t> cache_ptr;
    size_t maxCacheSize; // 最大缓存大小
    size_t currentCacheSize; // 当前缓存大小
    VMEMORY_CACHE_MECHANISM cacheMechanism = VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE;
protected:
    std::function<void(size_t)> cache_full_callback;
public:
    VMemoryCache(size_t maxSize) : maxCacheSize(maxSize), currentCacheSize(0) {}
    ~VMemoryCache() {
        for (auto it = header_block_caches.begin(); it != header_block_caches.end();) {
            auto& memBlocks = it->second;
            VMemoryCacheFrame* next_frame = memBlocks->header_frame;
            while (next_frame != nullptr) {
                if (next_frame->unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
                    currentCacheSize -= next_frame->unit.size;
                    next_frame->free();
                    if (next_frame == memBlocks->header_frame) {
                        memBlocks->header_frame = memBlocks->header_frame->next_frame;
                    }
                    if (next_frame == memBlocks->last_frame) {
                        memBlocks->last_frame = memBlocks->last_frame->prev_frame;
                    }
                    next_frame->removeFrame();
                    delete next_frame;
                    next_frame = nullptr;
                }
                next_frame = next_frame->next_frame;
            }
            ++it;
        }

        for (auto it = header_block_caches.begin(); it != header_block_caches.end();) {
            auto& memBlocks = it->second;
            VMemoryCacheFrame* next_frame = memBlocks->header_frame;
            while (next_frame != nullptr) {
                if (next_frame->unit.flag != VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
                    currentCacheSize -= next_frame->unit.size;
                    next_frame->free();
                    if (next_frame == memBlocks->header_frame) {
                        memBlocks->header_frame = memBlocks->header_frame->next_frame;
                    }
                    if (next_frame == memBlocks->last_frame) {
                        memBlocks->last_frame = memBlocks->last_frame->prev_frame;
                    }
                    next_frame->removeFrame();
                    delete next_frame;
                    next_frame = nullptr;
                }
                next_frame = next_frame->next_frame;
            }
            ++it;
        }

    }
    void setCacheFullCb(std::function<void(size_t)> cache_full) {
        cache_full_callback = cache_full;
    }

    // 申请内存
    void* malloc(size_t size) {
        if (size > maxCacheSize) {
            return VMemory::malloc(size);
        }
        if (header_block_caches.find(std::this_thread::get_id()) != header_block_caches.end() 
            && header_block_caches[std::this_thread::get_id()] != nullptr) {
            auto frame = header_block_caches[std::this_thread::get_id()]->getIdleFrame(size);
            if (frame != nullptr) {
                frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_USED;
                return frame->unit.ptr;
            }
            if (currentCacheSize + size > maxCacheSize) {
                cleanCache(size - (maxCacheSize - currentCacheSize));
            }
            return header_block_caches[std::this_thread::get_id()]->createFrame(size)->unit.ptr;
        }
        else {
            header_block_caches[std::this_thread::get_id()] = new VMemoryCacheBlock();
            if (currentCacheSize + size > maxCacheSize) {
                cleanCache(size - (maxCacheSize - currentCacheSize));
            }
            return header_block_caches[std::this_thread::get_id()]->createFrame(size)->unit.ptr;
        }

        return VMemory::malloc(size);
    }

    void* realloc(void* ptr, size_t size) {
        if (size > maxCacheSize) {
            return VMemory::realloc(ptr, size);
        }
        if (header_block_caches.find(std::this_thread::get_id()) != header_block_caches.end()
            && header_block_caches[std::this_thread::get_id()] != nullptr) {
            auto frame = header_block_caches[std::this_thread::get_id()]->getIdleFrame(size);
            if (frame != nullptr) {
                frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_USED;
                return frame->unit.ptr;
            }
            if (currentCacheSize + size > maxCacheSize) {
                cleanCache(size - (maxCacheSize - currentCacheSize));
            }
            return header_block_caches[std::this_thread::get_id()]->createFrame(size)->unit.ptr;
        }
        else {
            header_block_caches[std::this_thread::get_id()] = new VMemoryCacheBlock();
            if (currentCacheSize + size > maxCacheSize) {
                cleanCache(size - (maxCacheSize - currentCacheSize));
            }
            return header_block_caches[std::this_thread::get_id()]->createFrame(size)->unit.ptr;
        }

        return VMemory::realloc(ptr, size);
    }

    // 释放内存
    void free(void* ptr) {
        if (ptr == nullptr) {
            return;
        }
        
        if (cache_ptr.find(ptr) == cache_ptr.end()) {
            return;
        }
        if (header_block_caches.find(std::this_thread::get_id()) != header_block_caches.end()
            && header_block_caches[std::this_thread::get_id()] != nullptr) {
            auto frame = header_block_caches[std::this_thread::get_id()]->getIdleFrame(ptr);
            if (frame != nullptr) {
                frame->unit.flag = VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE;
                return;
            }
        }
        VMemory::free(ptr);
    }


    // 从缓存中释放空闲内存块
    void cleanCache(size_t size) {
        if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::QUEUE_CYCLE) {
            for (auto it = header_block_caches.begin(); it != header_block_caches.end() && size > 0;) {
                auto& memBlocks = it->second;
                VMemoryCacheFrame* next_frame = memBlocks->header_frame;
                while (next_frame != nullptr && size > 0) {
                    if (next_frame->unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
                        currentCacheSize -= next_frame->unit.size;
                        if (size < next_frame->unit.size) {
                            size = 0;
                        }
                        else {
                            size -= next_frame->unit.size;
                        }
                        next_frame->free();
                        if (next_frame == memBlocks->header_frame) {
                            memBlocks->header_frame = memBlocks->header_frame->next_frame;
                        }
                        if (next_frame == memBlocks->last_frame) {
                            memBlocks->last_frame = memBlocks->last_frame->prev_frame;
                        }
                        next_frame->removeFrame();
                        delete next_frame;
                        next_frame = nullptr;
                    }
                    next_frame = next_frame->next_frame;
                }
                ++it;
            }
        }
        else if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::COUNT_PRIORITY) {
            if (cache_count.find(size) != cache_count.end()) {
                ++cache_count[size];
            }
            else {
                cache_count[size] = 0;
            }

            for (auto it = header_block_caches.begin(); it != header_block_caches.end() && size > 0;) {
                auto& memBlocks = it->second;
                VMemoryCacheFrame* next_frame = memBlocks->header_frame;
                while (next_frame != nullptr && size > 0) {
                    if (next_frame->unit.flag == VMemoryCacheUnit::VMEMORY_CACHE_FLAGS::CACHE_IDLE) {
                        currentCacheSize -= next_frame->unit.size;
                        if (size < next_frame->unit.size) {
                            size = 0;
                        }
                        else {
                            size -= next_frame->unit.size;
                        }
                        next_frame->free();
                        if (next_frame == memBlocks->header_frame) {
                            memBlocks->header_frame = memBlocks->header_frame->next_frame;
                        }
                        if (next_frame == memBlocks->last_frame) {
                            memBlocks->last_frame = memBlocks->last_frame->prev_frame;
                        }
                        next_frame->removeFrame();
                        delete next_frame;
                        next_frame = nullptr;
                    }
                    next_frame = next_frame->next_frame;
                }
                ++it;
            }
        }
        else if (cacheMechanism == VMemoryCache::VMEMORY_CACHE_MECHANISM::FULL_NOTIFICATION) {
            if (cache_full_callback) {
                cache_full_callback(size);
            }
        }

       
    }
};
