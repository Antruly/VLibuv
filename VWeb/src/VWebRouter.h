#pragma once
#include "VTimerInfo.h"
#include <VCore.h>
#include <map>
#include "VWebDefine.h"

// ����·�ɽ�����
class VWebRouter : public VObject {
private:
  std::map<VString, VWebInterface *> routes;

  VString sanitizePath(const VString &path);

public:
  bool addRoute(const VString &path, VWebInterface *webInterface);
  std::map<VString, VWebInterface *> &getRoutes();

  VWebInterface *resolve(const VString &inputPath);
};


