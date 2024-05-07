#include "VThread.h"
#include "VMemory.h"

VThread::VThread() {
  thread = (uv_thread_t*)VMemory::malloc(sizeof(uv_thread_t));
}

VThread::~VThread() { VCORE_VFREE(this->thread); }

int VThread::start(std::function<void(void*)> start_cb, void* arg){
	thread_start_cb = start_cb;
	this->vdata = arg;
	return uv_thread_create(thread, callback_start, this);
}

int VThread::init() {
	memset(thread, 0, sizeof(uv_thread_t));
  return 0;
}

int VThread::join() {
  return uv_thread_join(thread);
}

int VThread::equal(const VThread* t){
	return uv_thread_equal(thread, t->thread);
}

int VThread::equal(const VThread* t1, const VThread* t2){
	return uv_thread_equal(t1->thread, t2->thread);
}

int VThread::equal(const uv_thread_t* t1, const uv_thread_t* t2){
	return uv_thread_equal(t1, t2);
}

int VThread::equal(const uv_thread_t* t){
	return uv_thread_equal(thread, t);
}

uv_thread_t VThread::self(){
	return uv_thread_self();
}

uv_thread_t* VThread::getThread() const {
  return thread;
}
