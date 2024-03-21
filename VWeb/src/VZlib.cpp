#include "VZlib.h"
#include <zlib.h>

VZlib::VZlib() {
  // 初始化错误信息
  error_ = "";
}

VZlib::~VZlib() {}

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
    printf(
        "compression succed, before compression size is %d, after compressione "
        "size is %d\n",
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
    if (compressedData.size() < VLIB_MAX_BUFFER_CHACE_SIZE) {
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
#ifdef _DEBUG
  printf(error_.c_str());
  printf("\n");
#endif
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
    printf(
        "decompress succed, before decompress size is %d, after decompress "
        "size is %d\n",
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

    if (decompressedData.size() < VLIB_MAX_BUFFER_CHACE_SIZE) {
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
#ifdef _DEBUG
  printf(error_.c_str());
  printf("\n");
#endif
  return false;
}

std::string VZlib::getLastError() const {
  return error_;
}