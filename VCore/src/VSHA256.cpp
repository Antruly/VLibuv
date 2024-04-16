#include "VSha256.h"
#include <cstring>
#include <iomanip>
#include <sstream>

VSha256* VSha256::Sha256 = new VSha256();

const uint32_t SHA_K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

VSha256::VSha256() : blocklen_(0), bitlen_(0) {
  init();
}

VSha256::~VSha256() {}

void VSha256::init() {
  state_[0] = 0x6a09e667;
  state_[1] = 0xbb67ae85;
  state_[2] = 0x3c6ef372;
  state_[3] = 0xa54ff53a;
  state_[4] = 0x510e527f;
  state_[5] = 0x9b05688c;
  state_[6] = 0x1f83d9ab;
  state_[7] = 0x5be0cd19;
}
std::string VSha256::hashFile256(const std::string& fileName,
                                 size_t readOnceSize) {
  init();
  updateFileHash(fileName);
  return digestToString();
}
std::string VSha256::hash256(const std::string& data) {
  init();
  update(data);
  return digestToString();
}

std::string VSha256::hash256(const VBuf& data) {
  init();
  update(data);
  return digestToString();
}

void VSha256::update(const VBuf& data) {
  update(data.getConstUData(), data.size());
}

void VSha256::update(const uint8_t* data, size_t length) {
  for (size_t i = 0; i < length; i++) {
    data_[blocklen_++] = data[i];
    if (blocklen_ == 64) {
      transform();

      bitlen_ += 512;
      blocklen_ = 0;
    }
  }
}

void VSha256::update(const std::string& data) {
  update(reinterpret_cast<const uint8_t*>(data.c_str()), data.size());
}

void VSha256::updateFileHash(const std::string& filename, const size_t readOnceSize) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    VLogger::Log->logError("Failed to open file: %s", filename.c_str());
    return;
  }

  uint8_t* buffer = new uint8_t[readOnceSize]();
  while (!file.eof()) {
    file.read(reinterpret_cast<char*>(buffer), readOnceSize);
    size_t bytes_read = file.gcount();
    if (bytes_read > 0) {
      this->update(buffer, bytes_read);
    }
  }
  delete[] buffer;
  file.close();
}

void VSha256::digest(uint8_t hash[32]) {
  pad();
  revert(hash);
}

std::string VSha256::digestToString() {
  uint8_t hash[32];
  digest(hash);
  return toString(hash);
}

uint32_t VSha256::rotr(uint32_t x, uint32_t n) {
  return (x >> n) | (x << (32 - n));
}

uint32_t VSha256::choose(uint32_t e, uint32_t f, uint32_t g) {
  return (e & f) ^ (~e & g);
}

uint32_t VSha256::majority(uint32_t a, uint32_t b, uint32_t c) {
  return (a & (b | c)) | (b & c);
}

uint32_t VSha256::sig0(uint32_t x) {
  return VSha256::rotr(x, 7) ^ VSha256::rotr(x, 18) ^ (x >> 3);
}

uint32_t VSha256::sig1(uint32_t x) {
  return VSha256::rotr(x, 17) ^ VSha256::rotr(x, 19) ^ (x >> 10);
}

void VSha256::transform() {
  uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
  uint32_t state[8];

  for (uint8_t i = 0, j = 0; i < 16;
       i++, j += 4) { 
    m[i] = (data_[j] << 24) | (data_[j + 1] << 16) | (data_[j + 2] << 8) |
           (data_[j + 3]);
  }

  for (uint8_t k = 16; k < 64; k++) { 
    m[k] = VSha256::sig1(m[k - 2]) + m[k - 7] + VSha256::sig0(m[k - 15]) +
           m[k - 16];
  }

  for (uint8_t i = 0; i < 8; i++) {
    state[i] = state_[i];
  }

  for (uint8_t i = 0; i < 64; i++) {
    maj = VSha256::majority(state[0], state[1], state[2]);
    xorA = VSha256::rotr(state[0], 2) ^ VSha256::rotr(state[0], 13) ^
           VSha256::rotr(state[0], 22);

    ch = choose(state[4], state[5], state[6]);

    xorE = VSha256::rotr(state[4], 6) ^ VSha256::rotr(state[4], 11) ^
           VSha256::rotr(state[4], 25);

	sum = m[i] + SHA_K[i] + state[7] + ch + xorE;
    newA = xorA + maj + sum;
    newE = state[3] + sum;

    state[7] = state[6];
    state[6] = state[5];
    state[5] = state[4];
    state[4] = newE;
    state[3] = state[2];
    state[2] = state[1];
    state[1] = state[0];
    state[0] = newA;
  }

  for (uint8_t i = 0; i < 8; i++) {
    state_[i] += state[i];
  }
}

void VSha256::pad() {
  uint64_t i = blocklen_;
  uint8_t end = blocklen_ < 56 ? 56 : 64;

  data_[i++] = 0x80;
  while (i < end) {
    data_[i++] = 0x00; 
  }

  if (blocklen_ >= 56) {
    transform();
    memset(data_, 0, 56);
  }

  bitlen_ += blocklen_ * 8;
  data_[63] = bitlen_;
  data_[62] = bitlen_ >> 8;
  data_[61] = bitlen_ >> 16;
  data_[60] = bitlen_ >> 24;
  data_[59] = bitlen_ >> 32;
  data_[58] = bitlen_ >> 40;
  data_[57] = bitlen_ >> 48;
  data_[56] = bitlen_ >> 56;
  transform();
}

void VSha256::revert(uint8_t hash[32]) {
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      hash[i + (j * 4)] = (state_[j] >> (24 - i * 8)) & 0x000000ff;
    }
  }
}

std::string VSha256::toString(const uint8_t digest[32]) {
  std::stringstream s;
  s << std::setfill('0') << std::hex;

  for (uint8_t i = 0; i < 32; i++) {
    s << std::setw(2) << (unsigned int)digest[i];
  }

  return s.str();
}
