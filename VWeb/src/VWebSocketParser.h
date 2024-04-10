#pragma once
extern "C" {
#include "websocket_parser.h"
}
#include <functional>
#include <string>
#include "VCoreDefine.h"
#include "VObject.h"
#include "VWebDefine.h"
#include <vector>
#include <VBase64.h>

#define VWEBSOCKETPARSER_MINI_CACHE_SIZE 14

#define VWEBSOCKETPARSER_GET_OPCODE(p) \
  (p->getWebSocketParser()->flags & WS_OP_MASK)
#define VWEBSOCKETPARSER_HAS_MASK(p) \
  (p->getWebSocketParser()->flags & WS_HAS_MASK)
#define VWEBSOCKETPARSER_HAS_FINAL(p) (p->getWebSocketParser()->flags & WS_FIN)
#define VWEBSOCKETPARSER_HAS_CLOSE(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_CLOSE)
#define VWEBSOCKETPARSER_HAS_TEXT(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_TEXT)
#define VWEBSOCKETPARSER_HAS_BINARY(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_BINARY)
#define VWEBSOCKETPARSER_HAS_PING(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_PING)
#define VWEBSOCKETPARSER_HAS_PONG(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_PONG)
#define VWEBSOCKETPARSER_HAS_CONTINUE(p) \
  ((p->getWebSocketParser()->flags & WS_OP_MASK) == WS_OP_CONTINUE)
struct WebSocketURL {
  std::string protocol;
  std::string host;
  int port;
  std::string path;
};

class VWebSocketParser : public VObject {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VWebSocketParser);
  VCORE_DEFINE_COPY_FUNC(VWebSocketParser);

  websocket_parser* getWebSocketParser() const;

  int setData(void* pdata);
  void* getData();

  void websocketParserInit();
  void websocketParserSettingsInit();
  size_t websocketParserExecute(const char* data, size_t len);

  // Apply XOR mask (see https://tools.ietf.org/html/rfc6455#section-5.3) and
  // store mask's offset
  void websocketParserDecode(char* dst, const char* src, size_t len);

  void generateMask(char mask[4]);

  void setFrameHeaderCallback(std::function<int(VWebSocketParser*)> callback);
  void setFrameBodyCallback(
      std::function<int(VWebSocketParser*, const char*, size_t)> callback);
  void setFrameEndCallback(std::function<int(VWebSocketParser*)> callback);

 public:
  // Apply XOR mask (see https://tools.ietf.org/html/rfc6455#section-5.3) and
  // return mask's offset
  static uint8_t websocketDecode(char* dst,
                                 const char* src,
                                 size_t len,
                                 const char mask[4],
                                 uint8_t mask_offset);
#define websocketEncode(dst, src, len, mask, mask_offset) \
  websocketDecode(dst, src, len, mask, mask_offset)

  // Calculate frame size using flags and data length
  static size_t websocketCalcFrameSize(websocket_flags flags, size_t data_len);

  // Create string representation of frame
  static size_t websocketBuildFrame(char* frame,
                                    websocket_flags flags,
                                    const char mask[4],
                                    const char* data,
                                    size_t data_len);
  
  WebSocketURL parseWebSocketUrl(const std::string& url);
 protected:
  void setWebSocketParser(void* hd);
  void setWebSocketParserData();


  static int callback_frame_header(websocket_parser*);
  static int callback_frame_body(websocket_parser*,
                                 const char* at,
                                 size_t length);
  static int callback_frame_end(websocket_parser*);


 private:
  void freeWebSocketParser();
  websocket_parser_settings* getWebSocketCallBack();

 protected:
  std::function<int(VWebSocketParser*)> frame_header_cb;
  std::function<int(VWebSocketParser*, const char*, size_t)> frame_body_cb;
  std::function<int(VWebSocketParser*)> frame_end_cb;

 private:
  websocket_parser* m_parser = nullptr;
  websocket_parser_settings* m_settings = nullptr;
  void* vdata = nullptr;
};