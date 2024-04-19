#pragma once
#include <iomanip>
#include <sstream>
#include <vector>
#include "VBuf.h"
#include "VLogger.h"
#include "VObject.h"



class VSha256 : public VObject {
 public:
  static VSha256* Sha256;
  VSha256();
  ~VSha256();
  void init();
  std::string hashFile256(const std::string& fileName,
                          const size_t readOnceSize = 4096);
  std::string hash256(const std::string& data);
  std::string hash256(const VBuf& data);

  void update(const VBuf& data);
  void update(const uint8_t* data, size_t length);
  void update(const std::string& data);
  void updateFileHash(const std::string& filename,
                      const size_t readOnceSize = 4096);
  void digest(uint8_t hash[32]);
  std::string digestToString();
  void digestToFile(const std::string& fileName) {
    std::ofstream file(fileName);
    if (!file) {
      Log->logError("Failed to create file: %s", fileName.c_str());
      return;
    }

    file << digestToString();
    file.close();
  }

  static std::string toString(const uint8_t digest[32]);

 private:
  uint8_t data_[64];
  uint32_t blocklen_;
  uint64_t bitlen_;
  uint32_t state_[8];

  static uint32_t rotr(uint32_t x, uint32_t n);
  static uint32_t choose(uint32_t e, uint32_t f, uint32_t g);
  static uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
  static uint32_t sig0(uint32_t x);
  static uint32_t sig1(uint32_t x);
  void transform();
  void pad();
  void revert(uint8_t hash[32]);
};
