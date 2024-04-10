#pragma once
#include "VHttpParser.h"
#include "VLibuv.h"

void setCallback(VHttpParser& httpParser) {
  httpParser.setMessageBeginCallback(
      [&](VHttpParser* http_parser) -> int { return 0; });

  httpParser.setUrlCallback(
      [&](VHttpParser* http_parser, const char* data, size_t size) -> int {
        std::string url_raw = std::string(data, size);
        return 0;
      });

  httpParser.setStatusCallback([&](VHttpParser* http_parser, const char* data,
                                   size_t size) -> int { return 0; });

  httpParser.setHeaderFieldCallback(
      [&](VHttpParser* http_parser, const char* data, size_t size) -> int {
        std::string header_cache = std::string(data, size);

        return 0;
      });

  httpParser.setHeaderValueCallback(
      [&](VHttpParser* http_parser, const char* data, size_t size) -> int {
        std::string header_value = std::string(data, size);
        return 0;
      });

  httpParser.setHeadersCompleteCallback(
      [&](VHttpParser* http_parser) -> int { return 0; });

  httpParser.setBodyCallback(
      [&](VHttpParser* http_parser, const char* data, size_t size) -> int {
        VBuf body;
        body.appandData(data, size);
        return 0;
      });

  httpParser.setMessageCompleteCallback(
      [&](VHttpParser* http_parser) -> int { return 0; });

  httpParser.setChunkHeaderCallback(
      [&](VHttpParser* http_parser) -> int { return 0; });

  httpParser.setChunkCompleteCallback(
      [&](VHttpParser* http_parser) -> int { return 0; });
}
int main() {
  VHttpParser httpRequestParser;
  VHttpParser httpResponseParser;
  httpRequestParser.httpParserInit(HTTP_REQUEST);
  httpResponseParser.httpParserInit(HTTP_RESPONSE);
  setCallback(httpRequestParser);
  setCallback(httpResponseParser);

  std::string httpPostRequestData =
      "POST /path/to/endpoint HTTP/1.1\r\nHost: "
      "example.com\r\nContent-Type: application/json\r\nContent-Length: "
      "27\r\n\r\n{\"key\": \"value\", \"foo\": \"bar\"}";
  std::string httpResponseData =
      "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
      "12\r\n\r\nHello World!";

  httpRequestParser.httpParserExecute(httpPostRequestData.c_str(),
                                      httpPostRequestData.size());

  httpResponseParser.httpParserExecute(httpResponseData.c_str(),
                                       httpResponseData.size());

  return 0;
}
