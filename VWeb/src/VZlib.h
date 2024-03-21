#pragma once
#include "VBuf.h"
#include <string>

#define VLIB_MAX_BUFFER_CHACE_SIZE (2 * 1024 * 1024 * 1024)

class VZlib {
 public:
  VZlib();
  ~VZlib();

  bool gzipCompress(const VBuf* data, VBuf& compressedData);
  bool gzipDecompress(const VBuf* compressedData, VBuf& decompressedData);

  std::string getLastError() const;

 protected:
  std::string error_;
};
