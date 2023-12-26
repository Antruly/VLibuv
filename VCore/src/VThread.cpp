#include "VThread.h"

VThread::VThread() {
  thread = (uv_thread_t*)VCore::malloc(sizeof(uv_thread_t));
}

VThread::~VThread() {
  free(thread);
}

int VThread::init() {
	memset(thread, 0, sizeof(uv_thread_t));
  return 0;
}

int VThread::join() {
  return uv_thread_join(thread);
}

uv_thread_t* VThread::getThread() const {
  return thread;
}
