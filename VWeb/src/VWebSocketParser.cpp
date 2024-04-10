#pragma once
#include "VWebSocketParser.h"
#include "VMemory.h"
#include <functional>
#include <random>
#include <regex>

VWebSocketParser::VWebSocketParser()
    : VObject(this),
      frame_header_cb(),
      frame_body_cb(),
      frame_end_cb() {
  m_parser = (websocket_parser*)VMemory::malloc(sizeof(websocket_parser));
  memset(m_parser, 0, sizeof(websocket_parser));
  m_settings =
      (websocket_parser_settings*)VMemory::malloc(sizeof(websocket_parser_settings));
  memset(m_settings, 0, sizeof(websocket_parser_settings));
  this->websocketParserInit();
  this->websocketParserSettingsInit();
  this->setWebSocketParserData();
}

VWebSocketParser::VWebSocketParser(const VWebSocketParser& obj)
    : VObject(nullptr) {
  if (obj.m_parser != nullptr) {
    m_parser = (websocket_parser*)VMemory::malloc(sizeof(websocket_parser));
    memcpy(obj.m_parser, this->m_parser, sizeof(websocket_parser));
    this->setWebSocketParserData();
    vdata = obj.vdata;
  } else {
    m_parser = nullptr;
  }

  if (obj.m_settings != nullptr) {
    m_settings =
        (websocket_parser_settings*)VMemory::malloc(sizeof(websocket_parser_settings));
    memcpy(obj.m_settings, this->m_settings, sizeof(websocket_parser_settings));
    this->websocketParserSettingsInit();
  } else {
    m_settings = nullptr;
  }
}
VWebSocketParser& VWebSocketParser::operator=(const VWebSocketParser& obj) {
  this->freeWebSocketParser();
  if (obj.m_parser != nullptr) {
    m_parser = (websocket_parser*)VMemory::malloc(sizeof(websocket_parser));
    memcpy(obj.m_parser, this->m_parser, sizeof(websocket_parser));
    this->setWebSocketParserData();
    vdata = obj.vdata;
  } else {
    m_parser = nullptr;
  }

  if (obj.m_settings != nullptr) {
    m_settings = (websocket_parser_settings*)VMemory::malloc(
        sizeof(websocket_parser_settings));
    memcpy(obj.m_settings, this->m_settings, sizeof(websocket_parser_settings));
    this->websocketParserSettingsInit();
  } else {
    m_settings = nullptr;
  }
  return *this;
}
VWebSocketParser::~VWebSocketParser() {
  this->freeWebSocketParser();
}
    websocket_parser* VWebSocketParser::getWebSocketParser() const {
  return m_parser;
}

int VWebSocketParser::setData(void* pdata) {
  vdata = pdata;
  return 0;
}

void* VWebSocketParser::getData() {
  return vdata;
}

void VWebSocketParser::websocketParserInit() {
  websocket_parser_init(m_parser);
}

void VWebSocketParser::websocketParserSettingsInit() {
  websocket_parser_settings_init(m_settings);
  m_settings->on_frame_header = &VWebSocketParser::callback_frame_header;
  m_settings->on_frame_body = &VWebSocketParser::callback_frame_body;
  m_settings->on_frame_end = &VWebSocketParser::callback_frame_end;
}

size_t VWebSocketParser::websocketParserExecute(const char* data, size_t len) {
  return websocket_parser_execute(m_parser, m_settings, data, len);
}

void VWebSocketParser::websocketParserDecode(char* dst,
                                             const char* src,
                                             size_t len) {
  websocket_parser_decode(dst, src, len, m_parser);
}

void VWebSocketParser::generateMask(char mask[4]) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 255);

  for (int i = 0; i < 4; ++i) {
    mask[i] = static_cast<char>(dis(gen));
  }
}

void VWebSocketParser::setFrameHeaderCallback(
    std::function<int(VWebSocketParser*)> callback) {
  this->frame_header_cb = callback;
}

void VWebSocketParser::setFrameBodyCallback(
    std::function<int(VWebSocketParser*, const char*, size_t)> callback) {
  this->frame_body_cb = callback;
}

void VWebSocketParser::setFrameEndCallback(
    std::function<int(VWebSocketParser*)> callback) {
  this->frame_end_cb = callback;
}

uint8_t VWebSocketParser::websocketDecode(char* dst,
                                          const char* src,
                                          size_t len,
                                          const char mask[4],
                                          uint8_t mask_offset) {
  return websocket_decode(dst, src, len, mask, mask_offset);
}

size_t VWebSocketParser::websocketCalcFrameSize(websocket_flags flags,
                                                size_t data_len) {
  return websocket_calc_frame_size(flags, data_len);
}

size_t VWebSocketParser::websocketBuildFrame(char* frame,
                                             websocket_flags flags,
                                             const char mask[4],
                                             const char* data,
                                             size_t data_len) {
  return websocket_build_frame(frame, flags, mask, data, data_len);
}

WebSocketURL VWebSocketParser::parseWebSocketUrl(const std::string& url) {
  std::regex regex("(wss?)://([^:/\\s]+):(\\d+)(/.*)?");
  std::smatch match;
  if (std::regex_match(url, match, regex)) {
    return {match[1].str(), match[2].str(), std::stoi(match[3].str()),
            match[4].str()};
  } else {
    return WebSocketURL();
  }
}

void VWebSocketParser::setWebSocketParser(void* hd) {
  this->freeWebSocketParser();
  m_parser = (websocket_parser*)hd;
  this->setWebSocketParserData();
  return;
}

void VWebSocketParser::setWebSocketParserData() {
  m_parser->data = this;
}

int VWebSocketParser::callback_frame_header(websocket_parser* m_parser) {
  if (reinterpret_cast<VWebSocketParser*>(m_parser->data)->frame_header_cb)
    return reinterpret_cast<VWebSocketParser*>(m_parser->data)
        ->frame_header_cb(
            reinterpret_cast<VWebSocketParser*>(m_parser->data));
}

int VWebSocketParser::callback_frame_body(websocket_parser* m_parser,
                                          const char* at,
                                          size_t length) {
  if (reinterpret_cast<VWebSocketParser*>(m_parser->data)->frame_body_cb)
    return reinterpret_cast<VWebSocketParser*>(m_parser->data)
        ->frame_body_cb(reinterpret_cast<VWebSocketParser*>(m_parser->data), at,
                        length);
}

int VWebSocketParser::callback_frame_end(websocket_parser* m_parser) {
  if (reinterpret_cast<VWebSocketParser*>(m_parser->data)->frame_end_cb)
    return reinterpret_cast<VWebSocketParser*>(m_parser->data)
        ->frame_end_cb(reinterpret_cast<VWebSocketParser*>(m_parser->data));
}

void VWebSocketParser::freeWebSocketParser() {
  if (m_parser != nullptr) {
    VMemory::free(m_parser);
    m_parser = nullptr;
  }
  if (m_settings != nullptr) {
    VMemory::free(m_settings);
    m_settings = nullptr;
  }
}
websocket_parser_settings* VWebSocketParser::getWebSocketCallBack() {
  return m_settings;
}
