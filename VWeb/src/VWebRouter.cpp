#pragma once
#include "VWebInterface.h"
#include "VWebRouter.h"

VString VWebRouter::sanitizePath(const VString &path) {
  size_t query_pos = path.find('?');
  if (query_pos != VString::npos) {
    return path.substr(0, query_pos);
  }

  if (path.size() > 1 && path.back() == '/') {
    return path.substr(0, path.size() - 1);
  }
  return path;
}

bool VWebRouter::addRoute(const VString &path, VWebInterface *webInterface) {
  try {
    routes.insert(std::make_pair(path, webInterface));
    return true;
  } catch (const std::exception &e) {
    Log->logDebugError("Exception caught in addRoute: %s", e.what());
    return false;
  } catch (...) {
    Log->logDebugError("Unknown exception caught in addRoute.");
    return false;
  }
}

std::map<VString, VWebInterface *> &VWebRouter::getRoutes() { return routes; }

VWebInterface *VWebRouter::resolve(const VString &inputPath) {
  VString path = sanitizePath(inputPath);

  // ¾«È·Æ¥Åä
  auto exactMatch = routes.find(path);
  if (exactMatch != routes.end()) {
    return exactMatch->second;
  }

  // ¾²Ì¬Â·¾¶Æ¥Åä
  VWebInterface *bestMatch = nullptr;
  VString relativePath;
  size_t maxLength = 0;

  for (auto kv : routes) {
    VString ggg = "/";
    VString strpa = kv.second->getRawPath();
    VString str = strpa + ggg;
    bool b1 = path.startsWith(str);
    bool b2 = path == kv.second->getRawPath();

    if (kv.second->isStaticPath() &&
        (path.startsWith(kv.second->getRawPath() + "/") ||
         path == kv.second->getRawPath())) {
      if (kv.second->getRawPath().length() > maxLength) {
        maxLength = kv.second->getRawPath().length();
        bestMatch = kv.second;
        relativePath = path.substr(kv.second->getRawPath().length() + 1);
      }
    }
  }


  return bestMatch;
}
