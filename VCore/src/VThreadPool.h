#pragma once
#include <atomic>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "VObject.h"
#include "VAsync.h"


#define VTHREADPOOL_MAX_SIZE 1024
#define VTHREADPOOL_MAX_IDLE_SIZE 100
#define VTHREADPOOL_MIN_IDLE_SIZE 5
#define VTHREADPOOL_MAX_TASK_QUEUE_SIZE 100
#define VTHREADPOOL_DEFAULT_SIZE 10

class VThreadPool : public VObject {
 private:
      using TaskType = std::function<void()>;
 public:
  struct Statistics {
    Statistics()
        : taskQueueSize(0),
          numThreads(0),
          idleThreads(0),
          workingThreads(0),
          waitCloseThreads(0),
          closedThreads(0),
          maxThreads(0),
          minIdleThreads(0),
          maxIdleThreads(0),
          maxTaskQueueSize(0),
          workedNumber(0) {}
    Statistics(const Statistics& other)
        : taskQueueSize(other.taskQueueSize.load()),
          numThreads(other.numThreads.load()),
          idleThreads(other.idleThreads.load()),
          workingThreads(other.workingThreads.load()),
          waitCloseThreads(other.waitCloseThreads.load()),
          closedThreads(other.closedThreads.load()),
          maxThreads(other.maxThreads.load()),
          minIdleThreads(other.minIdleThreads.load()),
          maxIdleThreads(other.maxIdleThreads.load()),
          maxTaskQueueSize(other.maxTaskQueueSize.load()), 
    workedNumber(other.workedNumber.load())
    {}

    std::atomic<size_t> taskQueueSize;
    std::atomic<size_t> numThreads;
    std::atomic<size_t> idleThreads;
    std::atomic<size_t> workingThreads;
    std::atomic<size_t> waitCloseThreads;
    std::atomic<size_t> closedThreads;
    std::atomic<size_t> minIdleThreads;
    std::atomic<size_t> maxIdleThreads;
    std::atomic<size_t> maxThreads;
    std::atomic<size_t> maxTaskQueueSize;
    std::atomic<size_t> workedNumber;
  };

  struct WorkeStatistics {
    WorkeStatistics()
        : workLock(), qlock(), threadID(), workeNumber(0), isStop(false) {}

    std::mutex workLock;
    std::unique_lock<std::mutex> qlock;
    std::thread::id threadID;
    size_t workeNumber;
    bool isStop;
    TaskType task;
  };

 public:
  /// <summary>
  /// 智能线程池
  /// </summary>
  /// <param name="numThreads">初始线程数量</param>
  /// <param name="maxThreads">最大线程数量</param>
  /// <param name="minIdleThreads">最小空闲线程数量（空闲线程小于该值将自动扩展至不超过最大空闲线程值）</param>
  /// <param name="maxIdleThreads">最大空闲线程数量</param>
  /// <param name="maxTaskQueueSize">最大工作队列（超过该值，enqueue方法将挂起等待，直到有空闲工作线程接手）</param>
  VThreadPool(size_t numThreads = VTHREADPOOL_DEFAULT_SIZE,
              size_t maxThreads = VTHREADPOOL_MAX_SIZE,
              size_t minIdleThreads = VTHREADPOOL_MIN_IDLE_SIZE,
              size_t maxIdleThreads = VTHREADPOOL_MAX_IDLE_SIZE,
              size_t maxTaskQueueSize = VTHREADPOOL_MAX_TASK_QUEUE_SIZE);

  template <class F, class... Args>
  void enqueue(F&& f, Args&&... args) {
    std::unique_lock<std::mutex> locked(task_queue_push_lock_);
    if (statistics.taskQueueSize > statistics.maxTaskQueueSize) {
      Log->logDebug(
          "taskQueueSize:%zu more than maxTaskQueueSize%zu \n",
             statistics.taskQueueSize.load(),
             statistics.maxTaskQueueSize.load());
      std::chrono::milliseconds(10);
    }

    while (statistics.taskQueueSize > statistics.maxTaskQueueSize) {
      std::chrono::milliseconds(10);
    }

    auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    statistics.taskQueueSize.fetch_add(1);
    this->push(std::move(task));
    this->sendWorkingSingal();
  }

  ~VThreadPool();

  size_t createThread(size_t num);

  void setMaxThreadNum(size_t num);
  void setMaxIdleThreadNum(size_t num);
  void setMaxTaskQueueNum(size_t num);
  Statistics getStatistics() const;

 protected:
  void threadRotation(std::unique_lock<std::mutex>& qlock);

  void removeWorker();
  void createWorker();

  bool isStopWorking();
  void waitWorkingSignal(std::unique_lock<std::mutex>& qlock);
  void sendWorkingSingal();
  void sendALLWorkingSingal();

 private:
 
  void push(TaskType task);

  bool pop(TaskType& task);

 private:
  std::map<std::thread::id, std::thread*> workers;
  std::vector<std::thread*> wait_delete_workers;
  std::atomic<bool> stop;
  Statistics statistics;
  std::mutex worker_lock_;
  std::mutex manage_lock_;
#if defined(_MSC_VER) && _MSC_VER <= 1800
  std::mutex send_worker_lock_;
  std::atomic<bool> send_working_stute;
  void worker_callback(VAsync* signal);
  void mamage_callback(VAsync* signal);
  VLoop sigal_working_loop;
  VLoop sigal_mamage_loop;
  VAsync sigal_working;
  VAsync sigal_mamage;
#else
  std::condition_variable sigal_working;
  std::condition_variable sigal_mamage;
  
#endif

  std::thread thread_manage;
  std::mutex task_queue_push_lock_;
  std::mutex task_queue_pop_lock_;
  std::queue<TaskType> task_queue;
};
