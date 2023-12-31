﻿#include "VWrite.h"
VWrite::VWrite() : VReq(this) {
  uv_write_t* write = (uv_write_t*)VCore::malloc(sizeof(uv_write_t));
  this->setReq(write);
  this->init();
}

VWrite::VWrite(VWrite* t_p) : VReq(t_p) {}

VWrite::~VWrite() {  }
int VWrite::init() {
  memset(VWRITE_REQ, 0, sizeof(uv_write_t));
  this->setReqData();
  return 0;
}


void VWrite::setBuf(const VBuf* bf) { buf = bf; }

const VBuf* VWrite::getBuf() { return buf; }

void VWrite::setBuf2(const VBuf* bf) { buf2 = bf; }

const VBuf* VWrite::getBuf2() { return buf2; }