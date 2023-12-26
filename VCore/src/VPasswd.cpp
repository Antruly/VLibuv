﻿#include "VPasswd.h"
VPasswd::VPasswd() {
  this->passwd = (uv_passwd_t*)VCore::malloc(sizeof(uv_passwd_t));
  this->init();
}

VPasswd::~VPasswd() {}

VPasswd::VPasswd(const VPasswd& obj) {
  if (this->passwd != nullptr) {
    uv_passwd_t* hd = (uv_passwd_t*)VCore::malloc(sizeof(uv_passwd_t));
    memcpy(hd, this->passwd, sizeof(uv_passwd_t));
    this->passwd = hd;
  } else {
    this->passwd = nullptr;
  }
}
VPasswd& VPasswd::operator=(const VPasswd& obj) {
  if (this->passwd != nullptr) {
    uv_passwd_t* hd = (uv_passwd_t*)VCore::malloc(sizeof(uv_passwd_t));
    memcpy(hd, this->passwd, sizeof(uv_passwd_t));
    this->passwd = hd;
  } else {
    this->passwd = nullptr;
  }
  return *this;
}

int VPasswd::init() {
  memset(this->passwd, 0, sizeof(uv_passwd_t));
  return 0;
}