﻿#pragma once
#include "VLibuvInclude.h"
#include "VCoreDefine.h"
#include "VObject.h"
#include <string>
#include <vector>

class VInterfaceAddress : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VInterfaceAddress);
  VCORE_DEFINE_COPY_FUNC(VInterfaceAddress);

  int init();

  std::string getIpv4Addrs();
  std::string getIpv6Addrs();

  static std::vector<VInterfaceAddress> getAllInterfaceAddresses();

  void* getInterfaceAddress() const;

 protected:
 private:
 private:
  uv_interface_address_t* interface_address = nullptr;

  static std::vector<VInterfaceAddress> addrs;
};
