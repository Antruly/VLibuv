#pragma once
#define VWEB_HTTP_VERSION_1_0 "HTTP/1.0"
#define VWEB_HTTP_VERSION_1_1 "HTTP/1.1"
#define VWEB_HTTP_VERSION_2_0 "HTTP/2.0"

#define VWEB_HTTP_MAX_BODY_CACHE_LENGTH 104857600ULL  // 1024 * 1024 * 100

#define VWEB_VLIB_MAX_BUFFER_CHACE_SIZE 2147483648    //2 * 1024 * 1024 * 1024

#define VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE 65536

enum METHOD_TYPE {
  OPTIONS = 0,
  GET = 1,
  HEAD = 2,
  POST = 3,
  PUT = 4,
  DELETE_METHOD = 5,
  TRACE = 6,
  CONNECT = 7
};

class VWebRouter;
class VWebInterface;
class VHttpClientObject;
class VWebServer;