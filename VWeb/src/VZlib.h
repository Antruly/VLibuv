#pragma once
#include "VBuf.h"
#include <string>
#include <zlib.h>
#include "VWebDefine.h"


class VZlib {
 public:
  VZlib();
  ~VZlib();

  bool gzipCompress(const VBuf* data, VBuf& compressedData);
  bool gzipDecompress(const VBuf* compressedData, VBuf& decompressedData);

  void initCompressStream();
  void initDecompressStream();
  void closeCompressStream();
  void closeDecompressStream();

  bool gzipCompressChunked(const VBuf& data,
                           VBuf& compressedData,
                           bool isFinal = false,
                           size_t cacheSize = 1024 * 1024 * 10);
  bool gzipDecompressChunked(const VBuf& compressedData,
                             VBuf& decompressedData,
                             bool isFinal = false,
                             size_t cacheSize = 1024 * 1024 * 10);

  std::string getLastError() const;

 protected:
  std::string error_;

 private:
  z_stream* zstrm_compress_ = nullptr;
  z_stream* zstrm_decompress_ = nullptr;
  VBuf tempCompressed_;
  VBuf tempDecompressed_;
};
