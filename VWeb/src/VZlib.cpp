#include "VZlib.h"
#include <zlib.h>
#include <VLogger.h>

VZlib::VZlib()
    : error_(), zstrm_compress_(nullptr), zstrm_decompress_(nullptr) {}

VZlib::~VZlib() {
  closeCompressStream();
  closeDecompressStream();
}

bool VZlib::gzipCompress(const VBuf* data, VBuf& compressedData) {
  z_stream zstrm;

  if (compressedData.size() < data->size() * 2) {
    compressedData.resize(data->size() * 2);
  }

RESTRAT:
  // 初始化压缩器
  memset(&zstrm, 0, sizeof(zstrm));

  // 初始化输入输出缓冲区
  zstrm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data->getData()));
  zstrm.avail_in = static_cast<uInt>(data->size());
  zstrm.next_out = reinterpret_cast<Bytef*>(compressedData.getData());
  zstrm.avail_out = static_cast<uInt>(compressedData.size());

  // 初始化压缩器参数
  int ret = deflateInit2(&zstrm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                         MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
  if (ret != Z_OK) {
    error_ = "Gzip deflateInit2 initialization failed: zlib error";
    deflateEnd(&zstrm);
    goto ERROR_TO_END;
  }

  // 执行压缩
  ret = deflate(&zstrm, Z_FINISH);
  if (ret == Z_STREAM_END || ret == Z_OK ||
      (ret == Z_BUF_ERROR && zstrm.avail_out != 0)) {
#ifdef _DEBUG
    VLogger::Log->logDebug(
        "compression succed, before compression size is %d, after compressione "
        "size is %zu\n",
        zstrm.total_in, compressedData.size() - zstrm.avail_out);
#endif
    // 压缩结束
    deflateEnd(&zstrm);

    // 更新压缩后数据的大小
    compressedData.resize(zstrm.total_out);

    // 清空错误信息
    error_ = "";
  } else if (ret == Z_STREAM_ERROR) {
    error_ = "Gzip compression failed: zlib error";
    deflateEnd(&zstrm);
    goto ERROR_TO_END;
  } else if (ret == Z_BUF_ERROR) {
    error_ = "Gzip compression failed: zlib out buffer error";
    deflateEnd(&zstrm);
    if (compressedData.size() < VWEB_VLIB_MAX_BUFFER_CHACE_SIZE) {
      compressedData.resize(compressedData.size() * 2);
      goto RESTRAT;
    } else {
      goto ERROR_TO_END;
    }

  } else {
    error_ = "Gzip compression failed: zlib error code " + std::to_string(ret);
    deflateEnd(&zstrm);
    goto ERROR_TO_END;
  }

  return true;
ERROR_TO_END:
  VLogger::Log->logDebug(error_.c_str());
  return false;
}

bool VZlib::gzipDecompress(const VBuf* compressedData, VBuf& decompressedData) {
  z_stream zstrm;
  if (decompressedData.size() < compressedData->size() * 2) {
    decompressedData.resize(compressedData->size() * 2);
  }

RESTRAT:
  // 初始化解压缩器
  memset(&zstrm, 0, sizeof(zstrm));

  // 初始化输入输出缓冲区
  zstrm.next_in =
      reinterpret_cast<Bytef*>(const_cast<char*>(compressedData->getData()));
  zstrm.avail_in = static_cast<uInt>(compressedData->size());
  zstrm.next_out = reinterpret_cast<Bytef*>(decompressedData.getData());
  zstrm.avail_out = static_cast<uInt>(decompressedData.size());

  // 初始化解压缩器参数
  int ret = inflateInit2(&zstrm, MAX_WBITS + 16);
  if (ret != Z_OK) {
    error_ = "Gzip inflateInit2 initialization failed: zlib error";
    inflateEnd(&zstrm);
    goto ERROR_TO_END;
  }

  // 执行解压缩
  ret = inflate(&zstrm, Z_FINISH);
  if (ret == Z_STREAM_END || ret == Z_OK ||
      (ret == Z_BUF_ERROR && zstrm.avail_out != 0)) {
#ifdef _DEBUG
    VLogger::Log->logDebug(
        "decompress succed, before decompress size is %d, after decompress "
        "size is %zu\n",
        zstrm.total_in, decompressedData.size() - zstrm.avail_out);
#endif
    // 解压缩结束
    inflateEnd(&zstrm);

    // 更新解压缩后数据的大小
    decompressedData.resize(zstrm.total_out);

    // 清空错误信息
    error_ = "";
  } else if (ret == Z_STREAM_ERROR) {
    error_ = "Gzip decompression failed: zlib error";
    inflateEnd(&zstrm);
    goto ERROR_TO_END;
  } else if (ret == Z_BUF_ERROR) {
    error_ = "Gzip decompression failed: zlib out buffer error";
    inflateEnd(&zstrm);
    decompressedData.resize(decompressedData.size() * 2);
    goto RESTRAT;

    if (decompressedData.size() < VWEB_VLIB_MAX_BUFFER_CHACE_SIZE) {
      decompressedData.resize(decompressedData.size() * 2);
      goto RESTRAT;
    } else {
      goto ERROR_TO_END;
    }
  } else {
    error_ =
        "Gzip decompression failed: zlib error code " + std::to_string(ret);
    inflateEnd(&zstrm);
    goto ERROR_TO_END;
  }

  return true;
ERROR_TO_END:
  VLogger::Log->logDebug(error_.c_str());
  return false;
}

  void VZlib::initCompressStream() {
  if (zstrm_compress_ != nullptr) {
      closeCompressStream();
  }
  zstrm_compress_ = new z_stream();
  memset(zstrm_compress_, 0, sizeof(z_stream));
  if (deflateInit2(zstrm_compress_, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16,
                   8, Z_DEFAULT_STRATEGY) != Z_OK) {
    error_ = ("Failed to initialize compress stream.");
  }
}

void VZlib::initDecompressStream() {
  if (zstrm_decompress_ != nullptr) {
    closeDecompressStream();
  }
  zstrm_decompress_ = new z_stream();
  memset(zstrm_decompress_, 0, sizeof(z_stream));
  if (inflateInit2(zstrm_decompress_, 15 + 16) != Z_OK) {
    error_ = ("Failed to initialize decompress stream.");
  }
}

void VZlib::closeCompressStream() {
  if (zstrm_compress_) {
    deflateEnd(zstrm_compress_);
    delete zstrm_compress_;
    zstrm_compress_ = nullptr;
  }
}

void VZlib::closeDecompressStream() {
  if (zstrm_decompress_) {
    inflateEnd(zstrm_decompress_);
    delete zstrm_decompress_;
    zstrm_decompress_ = nullptr;
  }
}

bool VZlib::gzipCompressChunked(const VBuf& data,
                         VBuf& compressedData,
                                bool isFinal,
                                size_t cacheSize) {
  if (!zstrm_compress_) {
    error_ = "Compress stream not initialized.";
    return false;
  }

  zstrm_compress_->next_in = reinterpret_cast<Bytef*>(data.getData());
  zstrm_compress_->avail_in = data.size();


  tempCompressed_.resize(cacheSize);  // 临时缓冲区大小
  tempCompressed_.setZero();

  do {
    zstrm_compress_->next_out =
        reinterpret_cast<Bytef*>(tempCompressed_.getData());
    zstrm_compress_->avail_out = tempCompressed_.size();

    int ret = deflate(zstrm_compress_, isFinal ? Z_FINISH : Z_NO_FLUSH);
      if (ret == Z_STREAM_END || ret == Z_OK ||
        (ret == Z_BUF_ERROR && zstrm_compress_->avail_out != 0)) {
#ifdef _DEBUG
        VLogger::Log->logDebug(
          "compression succed, before compression size is %d, after "
          "compressione "
          "size is %zu\n",
            zstrm_compress_->total_in,
            tempCompressed_.size() - zstrm_compress_->avail_out);
#endif
      // 清空错误信息
      error_ = "";
    } else if (ret == Z_STREAM_ERROR) {
      error_ = "Gzip compression failed: zlib error";
      return false;
    } else if (ret == Z_BUF_ERROR) {
      error_ = "Gzip compression failed: zlib out buffer error";
    } else {
      error_ =
          "Gzip compression failed: zlib error code " + std::to_string(ret);
      return false;
    }


    size_t have = tempCompressed_.size() - zstrm_compress_->avail_out;
    if (have > 0) {
      compressedData.appandData(tempCompressed_.getConstData(), have);
    }
  } while (zstrm_compress_->avail_out == 0);

  return true;
}

bool VZlib::gzipDecompressChunked(const VBuf& compressedData,
                                  VBuf& decompressedData,
                                  bool isFinal,
                                  size_t cacheSize) {
  if (!zstrm_decompress_) {
    error_ = "Decompress stream not initialized.";
    return false;
  }

  zstrm_decompress_->next_in =
      reinterpret_cast<Bytef*>(compressedData.getData());
  zstrm_decompress_->avail_in = compressedData.size();

  tempDecompressed_.resize(cacheSize);  // 临时缓冲区大小
  tempDecompressed_.setZero();


  do {
    zstrm_decompress_->next_out =
        reinterpret_cast<Bytef*>(tempDecompressed_.getData());
    zstrm_decompress_->avail_out = tempDecompressed_.size();
    int ret = inflate(zstrm_decompress_, isFinal ? Z_FINISH : Z_NO_FLUSH);
    if (ret == Z_STREAM_END || ret == Z_OK ||
        (ret == Z_BUF_ERROR && zstrm_decompress_->avail_out != 0)) {
#ifdef _DEBUG
      VLogger::Log->logDebug(
          "decompress succed, before decompress size is %d, after decompress "
          "size is %zu\n",
          zstrm_decompress_->total_in,
          tempDecompressed_.size() - zstrm_decompress_->avail_out);
#endif
      // 清空错误信息
      error_ = "";
    } else if (ret == Z_STREAM_ERROR) {
      error_ = "Gzip decompression failed: zlib error";
      return false;
    } else if (ret == Z_BUF_ERROR) {
      error_ = "Gzip decompression failed: zlib out buffer error";
      return false;
    } else {
      error_ =
          "Gzip decompression failed: zlib error code " + std::to_string(ret);
      return false;
    }

    size_t have = tempDecompressed_.size() - zstrm_decompress_->avail_out;
    if (have > 0) {
      decompressedData.appandData(tempDecompressed_.getConstData(), have);
    }
  } while (zstrm_decompress_->avail_out == 0);

  return true;
}


std::string VZlib::getLastError() const {
  return error_;
}