#pragma once
#include "VObject.h"

class VInterfaceAddress : public VObject {
 public:
  DEFINE_INHERIT_FUNC(VInterfaceAddress);
  DEFINE_COPY_FUNC(VInterfaceAddress);

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
