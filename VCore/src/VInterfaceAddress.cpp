#include "VInterfaceAddress.h"
#include "VMemory.h"

VInterfaceAddress::VInterfaceAddress(const VInterfaceAddress& addrs) {
  interface_address =
      (uv_interface_address_t*)VMemory::malloc(sizeof(uv_interface_address_t));
  memcpy(interface_address, addrs.interface_address,
         sizeof(uv_interface_address_t));
}

VInterfaceAddress& VInterfaceAddress::operator=(
    const VInterfaceAddress& addrs) {
  memcpy(this->interface_address, addrs.interface_address,
         sizeof(uv_interface_address_t));
  return *this;
}

std::vector<VInterfaceAddress>& VInterfaceAddress::getAllInterfaceAddresses() {
  static std::vector<VInterfaceAddress> addrs;
  addrs.clear();
  uv_interface_address_t* paddrs;
  int count = 0;
  uv_interface_addresses(&paddrs, &count);

  for (int i = 0; i < count; ++i) {
    VInterfaceAddress vaddrs;
    if (vaddrs.interface_address == nullptr) {
      vaddrs.interface_address = (uv_interface_address_t*)VMemory::malloc(
          sizeof(uv_interface_address_t));
    }

    memcpy(vaddrs.interface_address, &paddrs[i],
           sizeof(uv_interface_address_t));

    addrs.push_back(vaddrs);
  }
  uv_free_interface_addresses(paddrs, count);

  return addrs;
}

uv_interface_address_t *VInterfaceAddress::getInterfaceAddress() const {
  return this->interface_address;
}

VInterfaceAddress::VInterfaceAddress() {
  this->interface_address =
      (uv_interface_address_t*)VMemory::malloc(sizeof(uv_interface_address_t));
}

VInterfaceAddress::~VInterfaceAddress() {
  VCORE_VFREE(this->interface_address);
}

int VInterfaceAddress::init() {
  memset(this->interface_address, 0, sizeof(uv_interface_address_t));
  return 0;
}

std::string VInterfaceAddress::getIpv4Addrs() {
  char buffer[512] = {0};

  if (this->interface_address->address.address4.sin_family == AF_INET) {
    uv_ip4_name(&this->interface_address->address.address4, buffer,
                sizeof(buffer));
  }

  std::string strRet(buffer);
  return strRet;
}

std::string VInterfaceAddress::getIpv6Addrs() {
  char buffer[512] = {0};

  if (this->interface_address->address.address4.sin_family == AF_INET6) {
    uv_ip6_name(&this->interface_address->address.address6, buffer,
                sizeof(buffer));
  }

  std::string strRet(buffer);
  return strRet;
}
