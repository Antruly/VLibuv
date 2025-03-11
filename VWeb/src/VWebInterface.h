#pragma once
#include <VCore.h>
#include "VWebDefine.h"
#include "VHttpResponse.h"
#include "VHttpRequest.h"

typedef std::function<void(VHttpClientObject *, VHttpRequest *, VHttpResponse *)> if_recv_cb;
typedef std::function<void(VHttpClientObject *, VHttpRequest *, VHttpResponse *, const VBuf *)> if_recv_body_cb;

class VWebInterface : public VObject {
public:
  VWebInterface(const VString &rootPath, const VString &rawPath = VString("/"));
  VWebInterface(const VString &rawPath, METHOD_TYPE methodType);
  VWebInterface(const VString &rawPath, METHOD_TYPE methodType,
                if_recv_cb parseHeadersFinishCb, if_recv_cb parseFinishCb,
                if_recv_body_cb recvBodycb = nullptr);
  ~VWebInterface();

  if_recv_cb getParseHeadersFinishCallBackup();
  if_recv_cb getParseFinishCallBackup();
  if_recv_body_cb getRecvBodyCallBackup();

  void setParseHeadersFinishCallBackup(if_recv_cb cb);
  void setParseFinishCallBackup(if_recv_cb cb);
  void setRecvBodyCallBackup(if_recv_body_cb cb);

  VString getRawPath() const;
  void setRawPath(const VString &rawPath);

  VString getRootPath() const;
  void setRootPath(const VString &rootPath);

  std::vector<METHOD_TYPE> getMethodTypes() const;
  void addMethodType(METHOD_TYPE methodType);
  bool isStaticPath() const;
  void setStaticPath(bool staticPath);

private:
  if_recv_cb _parse_headers_finish_cb;
  if_recv_cb _parse_finish_cb;
  if_recv_body_cb _recv_body_cb;
  VString _raw_path;
  VString _root_path;
  std::vector<METHOD_TYPE> _method_types;
  bool _static_path = false;
};
