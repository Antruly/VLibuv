#pragma once
extern "C" {
#include "uv.h"
}

#ifndef NDIS_IF_MAX_STRING_SIZE
#define NDIS_IF_MAX_STRING_SIZE 256
#endif  // !NDIS_IF_MAX_STRING_SIZE

#define STD_NO_ZERO_ERROR_SHOW_INT(_ret, _remark)                 \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_NO_ZERO_ERROR_SHOW(_ret, _remark)                     \
  if (_ret) {                                                     \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }

#define STD_G_ZERO_ERROR_SHOW_INT(_ret, _remark)                  \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_G_ZERO_ERROR_SHOW(_ret, _remark)                      \
  if (_ret > 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }

#define STD_L_ZERO_ERROR_SHOW_INT(_ret, _remark)                  \
  if (_ret < 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return _ret;                                                  \
  }

#define STD_L_ZERO_ERROR_SHOW(_ret, _remark)                      \
  if (_ret < 0) {                                                 \
    fprintf(stderr, "%s error %s\n", _remark, uv_strerror(_ret)); \
    return;                                                       \
  }