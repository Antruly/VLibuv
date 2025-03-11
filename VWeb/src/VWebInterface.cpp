#include "VWebInterface.h"

VWebInterface::VWebInterface(const VString &rootPath, const VString &rawPath)
    : _parse_headers_finish_cb(), _parse_finish_cb(), _recv_body_cb(),
      _root_path(rootPath), _raw_path(rawPath), _method_types(),
      _static_path(true) {
  _method_types.push_back(METHOD_TYPE::GET);
}

VWebInterface::VWebInterface(const VString &rawPath, METHOD_TYPE methodType)
    : _parse_headers_finish_cb(),
      _parse_finish_cb(), _recv_body_cb(), _root_path(),
      _raw_path(rawPath), _method_types(), _static_path(false) {
  _method_types.push_back(methodType);
  }

VWebInterface::VWebInterface(const VString &rawPath, METHOD_TYPE methodType,
                             if_recv_cb parseHeadersFinishCb,
                             if_recv_cb parseFinishCb,
                             if_recv_body_cb recvBodycb)
    : _parse_headers_finish_cb(parseHeadersFinishCb),
      _parse_finish_cb(parseFinishCb), _recv_body_cb(recvBodycb), _root_path(),
      _raw_path(rawPath), _method_types(), _static_path(false) {
  _method_types.push_back(methodType);
}

VWebInterface::~VWebInterface() {}

if_recv_cb VWebInterface::getParseHeadersFinishCallBackup() {
  return _parse_headers_finish_cb;
}

if_recv_cb VWebInterface::getParseFinishCallBackup() {
  return _parse_finish_cb;
}
if_recv_body_cb VWebInterface::getRecvBodyCallBackup() { return _recv_body_cb; }

void VWebInterface::setParseHeadersFinishCallBackup(if_recv_cb cb) {
  _parse_headers_finish_cb = cb;
}

void VWebInterface::setParseFinishCallBackup(if_recv_cb cb) {
  _parse_finish_cb = cb;
}

void VWebInterface::setRecvBodyCallBackup(if_recv_body_cb cb) {
  _recv_body_cb = cb;
}



VString VWebInterface::getRawPath() const { return _raw_path; }

void VWebInterface::setRawPath(const VString &rawPath) { _raw_path = rawPath; }

VString VWebInterface::getRootPath() const { return _root_path; }

void VWebInterface::setRootPath(const VString &rootPath) {
  _root_path = rootPath;
}

std::vector<METHOD_TYPE> VWebInterface::getMethodTypes() const {
  return _method_types;
}

void VWebInterface::addMethodType(METHOD_TYPE methodType) {
  for (auto mtype : _method_types) {
    if (methodType == mtype)
      return;
  }
  _method_types.push_back(methodType);
}

bool VWebInterface::isStaticPath() const { return _static_path; }

void VWebInterface::setStaticPath(bool staticPath) {
  _static_path = staticPath;
}