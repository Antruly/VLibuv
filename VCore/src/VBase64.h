#pragma once
#include <string>
#include "VBuf.h"

class VBase64 {
 public:
  VBase64();
  ;
  ~VBase64();
  ;

  size_t encoding(const unsigned char* in_data,
                  size_t in_count,
                  unsigned char* out_base64);

  size_t decoding(const unsigned char* in_base64,
                  size_t in_charcount,
                  unsigned char* out_data);

  void encoding(const VBuf& in_data, VBuf& out_base64);

  void decoding(const VBuf& in_base64, VBuf& out_data);

  void encoding(const std::string& in_data, std::string& out_base64);

  void decoding(const std::string& in_base64, std::string& out_data);

 public:
  static size_t Encoding(const unsigned char* in_data,
                         size_t in_count,
                         unsigned char* out_base64);

  static size_t Decoding(const unsigned char* in_base64,
                         size_t in_charcount,
                         unsigned char* out_data);

  static void Encoding(const VBuf& in_data, VBuf& out_base64);

  static void Decoding(const VBuf& in_base64, VBuf& out_data);

  static void Encoding(const std::string& in_data, std::string& out_base64);

  static void Decoding(const std::string& in_base64, std::string& out_data);

 private:
};
