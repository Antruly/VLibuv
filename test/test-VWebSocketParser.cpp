#pragma once
#include "VLibuv.h"
#include "VWebSocketParser.h"


void setCallback(VWebSocketParser& webSocketParser) {
  webSocketParser.setFrameHeaderCallback(
      [&](VWebSocketParser* http_parser) -> int { 
          VBuf mask;
        mask.appendData(http_parser->getWebSocketParser()->mask, 4);

          return 0; 
      });

  webSocketParser.setFrameBodyCallback(
      [&](VWebSocketParser* http_parser, const char* data, size_t size) -> int {
        VBuf body;
        if (VWEBSOCKETPARSER_HAS_MASK(http_parser)) {
          body.resize(size);
          http_parser->websocketParserDecode(
              body.getData(), data, size);
        }
        else {
          body.appendData(data, size);
        }
        
        return 0;
      });

  webSocketParser.setFrameEndCallback(
      [&](VWebSocketParser* http_parser) -> int { 
          http_parser->websocketParserInit();
        return 0;
      });
}
int main() {
  VWebSocketParser websocketParser;
  setCallback(websocketParser);


  

  {
    std::string strFrameData = "hello world!";

    VBuf frmaeData;
    frmaeData.resize(strFrameData.size() * 2 +
                     VWEBSOCKETPARSER_MINI_CACHE_SIZE);
    websocket_flags flags =
        static_cast<websocket_flags>(WS_OP_TEXT | WS_FINAL_FRAME | WS_HAS_MASK);
    char mask[4] = {0};
    websocketParser.generateMask(mask);
    size_t frmaeSize = websocketParser.websocketBuildFrame(
        frmaeData.getData(), flags, mask, strFrameData.c_str(),
        strFrameData.size());
    frmaeData.resize(frmaeSize);

    websocketParser.websocketParserExecute(frmaeData.getConstData(),
                                           frmaeData.size());
  }
  
  {
    std::string strFrameData = "hi hi hi , hello world too!";

    VBuf frmaeData;
    frmaeData.resize(strFrameData.size() * 2 +
                     VWEBSOCKETPARSER_MINI_CACHE_SIZE);
    websocket_flags flags =
        static_cast<websocket_flags>(WS_OP_TEXT | WS_FINAL_FRAME | WS_HAS_MASK);
    char mask[4] = {0};
    websocketParser.generateMask(mask);
    size_t frmaeSize = websocketParser.websocketBuildFrame(
        frmaeData.getData(), flags, mask, strFrameData.c_str(),
        strFrameData.size());
    frmaeData.resize(frmaeSize);

    websocketParser.websocketParserExecute(frmaeData.getConstData(),
                                           frmaeData.size());
  }


  return 0;
}
