#pragma once
#include "VMemory.h"
std::function<void(size_t)> VMemory::malloc_callback;
std::function<void(void*, size_t)> VMemory::realloc_callback;
std::function<void(void*)> VMemory::free_callback;
std::function<void(void*)> VMemory::error_callback;


