#pragma once
#include "VWebServer.h"



struct VWebAppRequest {
  METHOD_TYPE method_type;
  ParsedURL url_parsed;
  std::map<VString, VString> query_data;
  std::map<VString, VString> x_www_form_data;
  HttpHeaders headers;
  VBuf body;
  VHttpMultiPart multi_part_data;
  VHttpRequest *request_p = nullptr;
  void *vdata = nullptr;
};

struct VWebAppResponse {
  int code = 200;
  HttpHeaders headers;
  VBuf body;
  VHttpResponse *response_p = nullptr;
  void *vdata = nullptr;
};

typedef std::function<int(VWebAppRequest &req, VWebAppResponse &res)>
    web_app_cb;


class VWebAppServer : public VWebServer {

public:
  VWebAppServer()
      : httpVersion_(VWEB_HTTP_VERSION_1_1),
        contentType_("application/json; charset=utf-8") {}
  virtual ~VWebAppServer() {}

  bool registerHandler(METHOD_TYPE mType, const VString &path,
                       web_app_cb app_cb = nullptr);
  bool registerHandler(const VString &rootPath, const VString &path,
                       web_app_cb app_cb = nullptr);

  void setHttpVersion(VString version);
  void setContentType(VString ctype);


protected:
  void hanldeDataBegin(VWebAppRequest &vreq, VWebAppResponse &vres);
  void hanldeDataEnd(VHttpClientObject *obj, VWebAppResponse &vres);

private:
  VString contentType_;
  VString httpVersion_;
};
