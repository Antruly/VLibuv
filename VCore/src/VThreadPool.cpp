#include "VThreadPool.h"
#include <cassert>

VThreadPool::VThreadPool(size_t numThreads,
                         size_t maxThreads,
                         size_t minIdleThreads,
                         size_t maxIdleThreads,
                         size_t maxTaskQueueSize)
    : VObject(), stop(false), statistics(), sigal_working() {
  statistics.maxThreads.store(maxThreads);
  statistics.minIdleThreads.store(minIdleThreads);
  statistics.maxIdleThreads.store(maxIdleThreads);
  statistics.maxTaskQueueSize.store(maxTaskQueueSize);

  thread_manage = std::thread([this] {
    while (!stop) {
      {
        std::unique_lock<std::mutex> locked(manage_lock_);

        this->threadRotation(locked);

        if (sigal_mamage.wait_for(locked, std::chrono::milliseconds(1000)) ==
            std::cv_status::timeout) {

            this->sendWorkingSingal();
        } else {
        }
      }

      std::unique_lock<std::mutex> locked(worker_lock_);
      if (wait_delete_workers.size() > 0) {
        for (auto& worker : wait_delete_workers) {
          worker->join();
          delete worker;
        }
        wait_delete_workers.clear();
      }
    }
  });

  this->createThread(numThreads);
}

VThreadPool::~VThreadPool() {
  stop = true;
  this->sendALLWorkingSingal();
  std::unique_lock<std::mutex> locked(worker_lock_);
  for (auto& worker : workers) {
    if (worker.second != nullptr)
      worker.second->join();
  }
  workers.clear();

  thread_manage.join();
}

size_t VThreadPool::createThread(size_t num) {
  if (num == 0 || num > statistics.maxThreads) {
    return 0;
  }
  std::unique_lock<std::mutex> locked(worker_lock_);
  size_t i = 0;
  for (; i < num && statistics.numThreads < statistics.maxThreads; ++i) {
    statistics.numThreads.fetch_add(1);
    std::thread* pTd = new std::thread;

    *pTd = std::thread([this, pTd] {

      WorkeStatistics workeStatistics;
      std::unique_lock<std::mutex> qlock(workeStatistics.workLock);
      workeStatistics.threadID = std::this_thread::get_id();
      workeStatistics.workeNumber = 0;
      workeStatistics.isStop = false;

      statistics.workingThreads.fetch_add(1);

      
      while (true) {
        this->waitWorkingSignal(qlock);
        bool haveTask = false; 
        {
          std::unique_lock<std::mutex> locked(task_queue_pop_lock_);
          haveTask = this->pop(workeStatistics.task);
        }
      
        if (haveTask) {
          statistics.taskQueueSize.fetch_sub(1);

          workeStatistics.task();
          ++workeStatistics.workeNumber;
          ++statistics.workedNumber;
        } else {
          if (stop)
            break;

          if (statistics.taskQueueSize > statistics.maxTaskQueueSize / 2)
            this->threadRotation(qlock);
          if (workeStatistics.workeNumber != 0) {
            if (this->isStopWorking()) {
              workeStatistics.isStop = true;
              break;
            }
          }
          std::this_thread::yield();
        }
      }

      std::unique_lock<std::mutex> locked(worker_lock_);
      statistics.workingThreads.fetch_sub(1);
      statistics.numThreads.fetch_sub(1);
      workers[workeStatistics.threadID] = nullptr;
      wait_delete_workers.push_back(pTd);
      if (workeStatistics.isStop)
        statistics.waitCloseThreads.fetch_sub(1);

      statistics.closedThreads.fetch_add(1);
    });

    workers[pTd->get_id()] = pTd;
  }
  return i;
}

void VThreadPool::push(TaskType task) {
  task_queue.push(task);
  return;
}

void VThreadPool::setMaxThreadNum(size_t num) {
  statistics.maxThreads = num;
}

bool VThreadPool::pop(TaskType& task) {
  if (task_queue.size() > 0) {
    task = task_queue.front();
    task_queue.pop();
    return true;
  } else {
    return false;
  }
}

void VThreadPool::setMaxIdleThreadNum(size_t num) {
  statistics.maxIdleThreads = num;
}

void VThreadPool::setMaxTaskQueueNum(size_t num) {
  statistics.maxTaskQueueSize = num;
}

VThreadPool::Statistics VThreadPool::getStatistics() const {
  return statistics;
}

void VThreadPool::threadRotation(std::unique_lock<std::mutex>& qlock) {
  this->createWorker();
  this->removeWorker();
}

void VThreadPool::removeWorker() {
  if (statistics.closedThreads > 10) {
    std::unique_lock<std::mutex> locked(worker_lock_);

    for (auto it = workers.begin(); it != workers.end();) {
      if (it->second == nullptr)
        it = workers.erase(it);
      else
        ++it;
    }
    statistics.closedThreads.store(0);
  }
}

void VThreadPool::createWorker() {
  // 空闲线程少于最小值时，开始扩充线程(最多扩充至最大线程数量)
  if (statistics.idleThreads < statistics.minIdleThreads &&
      statistics.numThreads < statistics.maxThreads) {
    if ((statistics.taskQueueSize + statistics.numThreads +
         statistics.minIdleThreads) < statistics.maxThreads) {
      if ((statistics.taskQueueSize + statistics.minIdleThreads) >
          statistics.maxIdleThreads) {
        this->createThread(statistics.maxIdleThreads);
      }
      else {
        this->createThread(statistics.taskQueueSize +
                           statistics.minIdleThreads);
      }
      
    } else {
      this->createThread(statistics.maxThreads - statistics.numThreads);
    }
  }
}

bool VThreadPool::isStopWorking() {
  if (statistics.idleThreads >= statistics.maxIdleThreads) {
    statistics.waitCloseThreads.fetch_add(1);
    return true;
  }
  return false;
}

void VThreadPool::waitWorkingSignal(std::unique_lock<std::mutex>& qlock) {
  /*if (statistics.taskQueueSize > 0) {
    return;
  }*/
  statistics.workingThreads.fetch_sub(1);
  statistics.idleThreads.fetch_add(1);

  if (sigal_working.wait_for(qlock, std::chrono::milliseconds(100)) ==
      std::cv_status::timeout) {
  } else {
  }

  // sigal_working.wait(qlock, [] { return true; });
  statistics.idleThreads.fetch_sub(1);
  statistics.workingThreads.fetch_add(1);
}

void VThreadPool::sendWorkingSingal() {
  sigal_working.notify_one();
}

void VThreadPool::sendALLWorkingSingal() {
  sigal_working.notify_all();
}
