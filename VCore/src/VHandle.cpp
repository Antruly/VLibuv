#include "VHandle.h"

VHandle::VHandle() : VObject(this)
,handle_close_cb()
,handle_alloc_cb() {
  handle = (uv_handle_t*)VMemory::malloc(sizeof(uv_handle_t));
  memset(handle, 0, sizeof(uv_handle_t));
  this->setHandleData();
}
VHandle::VHandle(VHandle* t_p) : VObject(t_p) {}
VHandle::~VHandle() {
  this->freeHandle();
}

int VHandle::setData(void* pdata) {
  vdata = pdata;
  return 0;
}

void* VHandle::getData() {
  return vdata;
}

void VHandle::ref() {
  uv_ref(handle);
}

void VHandle::unref() {
  uv_unref(handle);
}

int VHandle::hasRef() {
  return uv_has_ref(handle);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
VHandleType VHandle::handleGetType() {
  return uv_handle_get_type(handle);
}
const char* VHandle::handleTypeName() {
  return uv_handle_type_name(handle->type);
}
void* VHandle::handleGetData() {
  return uv_handle_get_data(handle);
}

void* VHandle::handleGetLoop() {
  return uv_handle_get_loop(handle);
}

void VHandle::handleSetData(void* data) {
  uv_handle_set_data(handle, data);
}
#endif
#endif

size_t VHandle::handleSize() {
  return uv_handle_size(handle->type);
}

int VHandle::isActive() {
  return uv_is_active(handle);
}

int VHandle::isActive(const VHandle* vhd) {
  return uv_is_active(vhd->handle);
}

void VHandle::close(VHandle* vhd) {
  uv_close((uv_handle_t*)vhd, nullptr);
}

int VHandle::isClosing(const VHandle* vhd) {
  return uv_is_closing(vhd->handle);
}



void VHandle::setHandleData() {
    handle->data = this;
}

void VHandle::callback_alloc(uv_handle_t* handle,
                             size_t suggested_size,
                             uv_buf_t* buf) {
  if (reinterpret_cast<VHandle*>(handle->data)->handle_alloc_cb)
    reinterpret_cast<VHandle*>(handle->data)
        ->handle_alloc_cb(reinterpret_cast<VHandle*>(handle->data),
                          suggested_size, reinterpret_cast<VBuf*>(buf));
}

// uv_close_cb

void VHandle::callback_close(uv_handle_t* handle) {
  if (reinterpret_cast<VHandle*>(handle->data)->handle_close_cb)
    reinterpret_cast<VHandle*>(handle->data)
        ->handle_close_cb(reinterpret_cast<VHandle*>(handle->data));
}

void VHandle::close() {
  uv_close((uv_handle_t*)handle, NULL);
}

int VHandle::isClosing() {
  return uv_is_closing(handle);
}

VHandle::VHandle(const VHandle& obj) : VObject(nullptr) {
  if (obj.handle != nullptr) {
    handle = (uv_handle_t*)VMemory::malloc(sizeof(uv_handle_t));
    memcpy(obj.handle, this->handle, sizeof(uv_handle_t));
   this->setHandleData();
    vdata = obj.vdata;
  } else {
    handle = nullptr;
  }
}

VHandle& VHandle::operator=(const VHandle& obj) {
  this->freeHandle();

  if (obj.handle != nullptr) {
    handle = (uv_handle_t*)VMemory::malloc(sizeof(uv_handle_t));
    memcpy(obj.handle, this->handle, sizeof(uv_handle_t));
    this->setHandleData();
    vdata = obj.vdata;
  } else {
    handle = nullptr;
  }

  return *this;
}

VHandle* VHandle::clone(VHandle* obj, int memSize) {
  VHandle* newObj = (VHandle*)new char[memSize];
  newObj->setHandleData();
  memcpy(newObj, obj, memSize);
  return obj;
}

void VHandle::ref(void* hd) {
  uv_ref((uv_handle_t*)hd);
}

void VHandle::unref(void* hd) {
  uv_unref((uv_handle_t*)hd);
}

int VHandle::hasRef(const void* hd) {
  return uv_has_ref((uv_handle_t*)hd);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
VHandleType VHandle::handleGetType(const VHandle* vhd) {
  return uv_handle_get_type(vhd->handle);
}
const char* VHandle::handleTypeName(VHandle* vhd) {
  return uv_handle_type_name(vhd->handle->type);
}

void* VHandle::handleGetData(const VHandle* vhd) {
  return uv_handle_get_data(vhd->handle);
}

void* VHandle::handleGetLoop(const VHandle* vhd) {
  return uv_handle_get_loop(vhd->handle);
}

void VHandle::handleSetData(VHandle* vhd, void* data) {
  uv_handle_set_data(vhd->handle, data);
}
#endif
#endif


size_t VHandle::handleSize(VHandle* vhd) {
  return uv_handle_size(vhd->handle->type);
}

// std::function<void(int)> funcBind = std::bind(&MyClass::myMethod, &obj, std::placeholders::_1);
void VHandle::close(std::function<void(VHandle*)> closeCallback) {
  handle_close_cb = closeCallback;
  uv_close(handle, callback_close);
  return;
}

uv_handle_t* VHandle::getHandle() const {
  return handle;
}

void VHandle::setHandle(void* hd) {
  this->freeHandle();
  handle = (uv_handle_t*)hd;
  handle_union = *(HandleUnion*)&handle;
  this->setHandleData();
  return;
}

void VHandle::freeHandle() {
  if (handle != nullptr) {
    if (uv_is_closing(handle) && !handle->flags && uv_is_active(handle)) {
      uv_close(handle, nullptr);
    }
    VMemory::free(handle);
    handle = nullptr;
  }
}
