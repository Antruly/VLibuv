#include "VThreadPool.h"

VThreadPool::VThreadPool(size_t numThreads) : VObject(),stop(false) {

    if (numThreads > VTHREADPOOL_MAX_SIZE) {
    numThreads = VTHREADPOOL_MAX_SIZE;
    }
  for (size_t i = 0; i < numThreads; ++i) {
    workers.emplace_back([this] {
      while (true) {
        TaskType task;
        if (taskQueue.pop(task)) {
          task();
        } else {
          if (stop)
            return;
          //std::this_thread::yield();  // Avoid busy waiting
          std::this_thread::sleep_for(
              std::chrono::milliseconds(1));  // Add a short sleep
        }
      }
    });
  }
}

VThreadPool::~VThreadPool() {
  stop = true;
  for (std::thread& worker : workers)
    worker.join();
}

VThreadPool::LockFreeQueue::LockFreeQueue()
    : head(nullptr), tail(nullptr) {}

void VThreadPool::LockFreeQueue::push(TaskType task) {
  Node* newNode = new Node(std::move(task));
  Node* oldTail = tail.load(std::memory_order_relaxed);
  while (!tail.compare_exchange_weak(
      oldTail, newNode, std::memory_order_release, std::memory_order_relaxed)) {
    oldTail = tail.load(std::memory_order_relaxed);
  }
  if (!head.exchange(newNode, std::memory_order_acq_rel)) {
    while (head.load(std::memory_order_relaxed) != newNode) {
    }  // Wait for the consumer to update head
  }
}

bool VThreadPool::LockFreeQueue::pop(TaskType& task) {
  Node* oldHead = head.load(std::memory_order_relaxed);
  while (oldHead) {
    if (head.compare_exchange_weak(oldHead, oldHead->next,
                                   std::memory_order_acquire,
                                   std::memory_order_relaxed)) {
      task = std::move(oldHead->task);
      delete oldHead;
      return true;
    }
  }
  return false;
}

VThreadPool::LockFreeQueue::Node::Node(TaskType task_)
    : task(std::move(task_)), next(nullptr) {}
