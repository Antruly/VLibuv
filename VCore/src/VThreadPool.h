#pragma once
#include <atomic>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include "VObject.h"

#define VTHREADPOOL_MAX_SIZE 1024
#define VTHREADPOOL_DEFAULT_SIZE 10

class VThreadPool : public VObject {
 public:
  VThreadPool(size_t numThreads = VTHREADPOOL_DEFAULT_SIZE);

  template <class F, class... Args>
  void enqueue(F&& f, Args&&... args) {
    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    taskQueue.push(std::move(task));
  }

  ~VThreadPool();

 private:
  using TaskType = std::function<void()>;

  class LockFreeQueue {
   public:
    LockFreeQueue();

    void push(TaskType task);

    bool pop(TaskType& task);

   private:
    struct Node {
      TaskType task;
      Node* next;
      Node(TaskType task_);
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
  };

  std::vector<std::thread> workers;
  LockFreeQueue taskQueue;
  std::atomic<bool> stop;
};

