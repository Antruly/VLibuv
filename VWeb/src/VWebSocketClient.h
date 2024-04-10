#pragma once
#include "VHttpClient.h"
#include "VWebSocketParser.h"

enum class WebSocketVersion {
  Version0 = 0,
  Version7 = 7,
  Version8 = 8,
  Version10 = 10,
  Version11 = 11,
  Version13 = 13
};

class VWebSocketClient : public VHttpClient {
 public:
  VWebSocketClient();
  VWebSocketClient(VTcpClient* tcp_client);
  virtual ~VWebSocketClient() override;

  VWebSocketParser* getVWebSocketParser()const;
  void initData();
  void initWebsocketCallback(bool isSsl = false);

  bool websocketConnect(const std::string& url);


  bool websocketSendText(const std::string& data,
                         bool final = true,
                         bool mask = true);

  bool websocketSendBinary(const VBuf& data,
                           bool final = true,
                           bool mask = true);

  bool websocketSendPing();
  bool websocketSendPong();
  bool websocketSendClose();

  // 分包发送，除了首包为特定类型，后续包都为续包(continue)，final = false，最后一个包发送final = true
  bool websocketSendContinue(const VBuf& data,
                             bool final = true,
                             bool mask = true);

  bool websocketSend(websocket_flags flags);
  bool websocketSend(const VBuf& data,
                     websocket_flags flags = static_cast<websocket_flags>(
                         WS_OP_TEXT | WS_FINAL_FRAME | WS_HAS_MASK));
  bool websocketSend(const VBuf& data, websocket_flags flags, char mask[4]);

  uint64_t websocketPing();

  void websocketClose();

  bool isParser();

  void waitRecvNewBody(const uint64_t& maxTimout = 30000);

  size_t waitRecvWebsocketFrame(const uint64_t& maxTimeout = 30000);
  VBuf getRecvWebsocketBody(size_t count = 0);
  void clearRecvWebsocketBody();

  std::string generateWebSocketKey();


  void setWebsocketConnectiondCb(
      std::function<void(VWebSocketClient*, int)> connectiond_cb);
  void setWebsocketSendFinishCb(
      std::function<void(VWebSocketClient*, int)> send_finish_cb);
  void setWebsocketParserFinishCb(
      std::function<void(VWebSocketClient*, int)> parser_finish_cb);
  void setWebsocketParserMaskFinishCb(
      std::function<void(VWebSocketClient*, const char[4])>
          parser_mask_finish_cb);
  void setWebsocketRecvBodyCb(
      std::function<bool(VWebSocketClient*, const VBuf*)> recv_body_cb);
  void setWebsocketParserFinalFinishCb(
      std::function<void(VWebSocketClient*, int)> parser_final_finish_cb);
  void setWebsocketCloseCb(
      std::function<void(VWebSocketClient*, int)> close_cb);
  void setWebsocketParserHeaderCb(
      std::function<void(VWebSocketClient*, websocket_flags)> parser_header_cb);
 protected:
  bool writeData(const VBuf& sendBuf);

 protected:
  std::function<void(VWebSocketClient*, int)> websocket_client_connectiond_cb;
  std::function<void(VWebSocketClient*, int)> websocket_client_send_finish_cb;
  std::function<void(VWebSocketClient*, int)> websocket_client_parser_finish_cb;
  std::function<void(VWebSocketClient*, const char[4])>
      websocket_client_parser_mask_finish_cb;
  std::function<bool(VWebSocketClient*, const VBuf*)>
      websocket_client_recv_body_cb;
  std::function<void(VWebSocketClient*, int)>
      websocket_client_parser_final_finish_cb;

  std::function<void(VWebSocketClient*, int)> websocket_client_close_cb;
  std::function<void(VWebSocketClient*, websocket_flags)>
      websocket_client_parser_header_cb;
 private:
  VWebSocketParser* websocket_parser_ = nullptr;
  VAsync* async_recv = nullptr;
  VBase64 base64_;
  VBuf websocket_body_;
  WebSocketURL websocket_url_parser_;
  std::string websocket_url_;
  size_t max_body_cache_length_;
  WebSocketVersion websocket_version_ = WebSocketVersion::Version13;
  std::atomic<bool> parser_finish_ = false;
  std::atomic<bool> send_close_ = false;
  std::mutex parser_mutex_;
};
