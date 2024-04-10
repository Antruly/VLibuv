#include "VBase64.h"

VBase64::VBase64() {}

VBase64::~VBase64() {}

 size_t VBase64::encoding(const unsigned char* in_data,
                       size_t in_count,
                       unsigned char* out_base64) {
  static const char* base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
      "+/";  // -_
  size_t cnt = in_count / 3 * 3;
  for (size_t i = 0; i < cnt; i += 3, out_base64 += 4) {
    unsigned char a0 = in_data[i];
    unsigned char a1 = in_data[i + 1];
    unsigned char a2 = in_data[i + 2];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
    out_base64[2] = base64_chars[((a1 & 0x0f) << 2) + (a2 >> 6)];
    out_base64[3] = base64_chars[a2 & 0x3f];
  }

  if (cnt + 1 == in_count) {
    unsigned char a0 = in_data[cnt];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[(a0 & 0x03) << 4];
    out_base64[2] = '=';
    out_base64[3] = '=';
  } else if (cnt + 2 == in_count) {
    unsigned char a0 = in_data[cnt];
    unsigned char a1 = in_data[cnt + 1];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
    out_base64[2] = base64_chars[(a1 & 0x0f) << 2];
    out_base64[3] = '=';
  }
  return (in_count + 2) / 3 * 4;
}

 size_t VBase64::decoding(const unsigned char* in_base64,
                       size_t in_charcount,
                       unsigned char* out_data) {
  static const unsigned char pm[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 10
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 20
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 30
      0,  0,  0,  62, 0,  62, 0,  63, 52, 53,  // 40
      54, 55, 56, 57, 58, 59, 60, 61, 0,  0,   // 50
      0,  0,  0,  0,  0,  0,  1,  2,  3,  4,   // 60
      5,  6,  7,  8,  9,  10, 11, 12, 13, 14,  // 70
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 80
      25, 0,  0,  0,  0,  63, 0,  26, 27, 28,  // 90
      29, 30, 31, 32, 33, 34, 35, 36, 37, 38,  // 100
      39, 40, 41, 42, 43, 44, 45, 46, 47, 48,  // 110
      49, 50, 51, 0,  0,  0,  0,  0            // 120
  };

  size_t cnt = in_charcount - 4;
  for (size_t i = 0; i < cnt; i += 4, out_data += 3) {
    unsigned char a0 = pm[in_base64[i]];
    unsigned char a1 = pm[in_base64[i + 1]];
    unsigned char a2 = pm[in_base64[i + 2]];
    unsigned char a3 = pm[in_base64[i + 3]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_data[2] = (a2 << 6) + a3;
  }

  size_t out_cnt = cnt / 4 * 3;
  if (in_base64[cnt + 2] == '=') {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_cnt += 1;
  } else if (in_base64[cnt + 3] == '=') {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    unsigned char a2 = pm[in_base64[cnt + 2]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_cnt += 2;
  } else {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    unsigned char a2 = pm[in_base64[cnt + 2]];
    unsigned char a3 = pm[in_base64[cnt + 3]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_data[2] = (a2 << 6) + a3;
    out_cnt += 3;
  }
  return out_cnt;
}

void VBase64::encoding(const VBuf& in_data, VBuf& out_base64) {
  out_base64.resize(in_data.size() * 2);
  size_t size =
      encoding(in_data.getConstUData(), in_data.size(), out_base64.getUData());
  out_base64.resize(size);
}

void VBase64::decoding(const VBuf& in_base64, VBuf& out_data) {
  out_data.resize(in_base64.size() * 2);
  size_t size =
      decoding(in_base64.getConstUData(), in_base64.size(), out_data.getUData());
  out_data.resize(size);
}

void VBase64::encoding(const std::string& in_data, std::string& out_base64) {
  VBuf in_buf(in_data);
  VBuf out_buf;
  encoding(in_buf, out_buf);
  out_base64 = out_buf.toString();
}

void VBase64::decoding(const std::string& in_base64, std::string& out_data) {
  VBuf in_buf(in_base64);
  VBuf out_buf;
  decoding(in_buf, out_buf);
  out_data = out_buf.toString();
}


size_t VBase64::Encoding(const unsigned char* in_data,
                       size_t in_count,
                      unsigned char* out_base64) {
  static const char* base64_chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
      "+/";  // -_
  size_t cnt = in_count / 3 * 3;
  for (size_t i = 0; i < cnt; i += 3, out_base64 += 4) {
    unsigned char a0 = in_data[i];
    unsigned char a1 = in_data[i + 1];
    unsigned char a2 = in_data[i + 2];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
    out_base64[2] = base64_chars[((a1 & 0x0f) << 2) + (a2 >> 6)];
    out_base64[3] = base64_chars[a2 & 0x3f];
  }

  if (cnt + 1 == in_count) {
    unsigned char a0 = in_data[cnt];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[(a0 & 0x03) << 4];
    out_base64[2] = '=';
    out_base64[3] = '=';
  } else if (cnt + 2 == in_count) {
    unsigned char a0 = in_data[cnt];
    unsigned char a1 = in_data[cnt + 1];
    out_base64[0] = base64_chars[a0 >> 2];
    out_base64[1] = base64_chars[((a0 & 0x03) << 4) + (a1 >> 4)];
    out_base64[2] = base64_chars[(a1 & 0x0f) << 2];
    out_base64[3] = '=';
  }
  return (in_count + 2) / 3 * 4;
}

size_t VBase64::Decoding(const unsigned char* in_base64,
                       size_t in_charcount,
                      unsigned char* out_data) {
  static const unsigned char pm[] = {
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 0
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 10
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 20
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   // 30
      0,  0,  0,  62, 0,  62, 0,  63, 52, 53,  // 40
      54, 55, 56, 57, 58, 59, 60, 61, 0,  0,   // 50
      0,  0,  0,  0,  0,  0,  1,  2,  3,  4,   // 60
      5,  6,  7,  8,  9,  10, 11, 12, 13, 14,  // 70
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 80
      25, 0,  0,  0,  0,  63, 0,  26, 27, 28,  // 90
      29, 30, 31, 32, 33, 34, 35, 36, 37, 38,  // 100
      39, 40, 41, 42, 43, 44, 45, 46, 47, 48,  // 110
      49, 50, 51, 0,  0,  0,  0,  0            // 120
  };

  size_t cnt = in_charcount - 4;
  for (size_t i = 0; i < cnt; i += 4, out_data += 3) {
    unsigned char a0 = pm[in_base64[i]];
    unsigned char a1 = pm[in_base64[i + 1]];
    unsigned char a2 = pm[in_base64[i + 2]];
    unsigned char a3 = pm[in_base64[i + 3]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_data[2] = (a2 << 6) + a3;
  }

  size_t out_cnt = cnt / 4 * 3;
  if (in_base64[cnt + 2] == '=') {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_cnt += 1;
  } else if (in_base64[cnt + 3] == '=') {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    unsigned char a2 = pm[in_base64[cnt + 2]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_cnt += 2;
  } else {
    unsigned char a0 = pm[in_base64[cnt]];
    unsigned char a1 = pm[in_base64[cnt + 1]];
    unsigned char a2 = pm[in_base64[cnt + 2]];
    unsigned char a3 = pm[in_base64[cnt + 3]];
    out_data[0] = (a0 << 2) + (a1 >> 4);
    out_data[1] = (a1 << 4) + (a2 >> 2);
    out_data[2] = (a2 << 6) + a3;
    out_cnt += 3;
  }
  return out_cnt;
}

void VBase64::Encoding(const VBuf& in_data, VBuf& out_base64) {
  out_base64.resize(in_data.size() * 2);
  size_t size = Encoding(in_data.getConstUData(), in_data.size(), out_base64.getUData());
  out_base64.resize(size);
}

void VBase64::Decoding(const VBuf& in_base64, VBuf& out_data) {
  out_data.resize(in_base64.size() * 2);
  size_t size =
      Decoding(in_base64.getConstUData(), in_base64.size(), out_data.getUData());
  out_data.resize(size);
}

void VBase64::Encoding(const std::string& in_data, std::string& out_base64) {
  VBuf in_buf(in_data);
  VBuf out_buf;
  Encoding(in_buf, out_buf);
  out_base64 = out_buf.toString();
}

void VBase64::Decoding(const std::string& in_base64, std::string& out_data) {
  VBuf in_buf(in_base64);
  VBuf out_buf;
  Decoding(in_buf, out_buf);
  out_data = out_buf.toString();
}
