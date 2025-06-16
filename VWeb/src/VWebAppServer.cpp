#include "VWebAppServer.h"



bool VWebAppServer::registerHandler(METHOD_TYPE mType, const VString &path, web_app_cb app_cb) {
  if (!app_cb) {
    return false;
    }
    else {
      VWebInterface *IUser = new VWebInterface(path, mType);

      IUser->setParseHeadersFinishCallBackup(
          [&IUser, &app_cb](VHttpClientObject *obj,
                                                      VHttpRequest *req,
                                                      VHttpResponse *res) { obj->cache_data.clear();
      });
      IUser->setParseFinishCallBackup([this, IUser, app_cb](VHttpClientObject *obj,
                                                  VHttpRequest *req,
                                                  VHttpResponse *res) {
        VWebAppRequest vreq;
        VWebAppResponse vres;
        vreq.request_p = req;
        vres.response_p = res;
        vreq.body = obj->cache_data;

        this->hanldeDataBegin(vreq, vres);

        if (app_cb)
        {
          vres.code = app_cb(vreq, vres);
        }


        this->hanldeDataEnd(obj, vres);

        obj->client->sendResponse(vres.code, vres.body); 

      });
      IUser->setRecvBodyCallBackup([this, IUser, app_cb](VHttpClientObject *obj,
                                                  VHttpRequest *req,
                                                  VHttpResponse *res, const VBuf* buf) {
        obj->cache_data.appendData(buf->getConstData(), buf->size());
        if (obj->cache_data.size() > VWEB_HTTP_MAX_BODY_CACHE_LENGTH -
                                         VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE) {
          VWebAppRequest vreq;
          VWebAppResponse vres;
          vreq.request_p = req;
          vres.response_p = res;
          vreq.body = obj->cache_data;

          this->hanldeDataBegin(vreq, vres);

          if (app_cb) {
            vres.code = app_cb(vreq, vres);
          }

          this->hanldeDataEnd(obj, vres);

          obj->client->sendResponse(vres.code, vres.body); 
        }
      });
      return this->addWebInterface(IUser);
    }
  
}

bool VWebAppServer::registerHandler(const VString &rootPath,
                                    const VString &path, web_app_cb app_cb) {
  VWebInterface *IStatic = new VWebInterface(rootPath, path);
  if (!app_cb) {
    IStatic->setParseHeadersFinishCallBackup(
        [&IStatic, &app_cb](VHttpClientObject *obj, VHttpRequest *req,
                          VHttpResponse *res) { obj->cache_data.clear(); });
    IStatic->setParseFinishCallBackup(
        [this, IStatic, app_cb](VHttpClientObject *obj, VHttpRequest *req,
                              VHttpResponse *res) {
          VWebAppRequest vreq;
          VWebAppResponse vres;
          vreq.request_p = req;
          vres.response_p = res;
          vreq.body = obj->cache_data;

          this->hanldeDataBegin(vreq, vres);

          if (app_cb) {
            vres.code = app_cb(vreq, vres);
          }

          this->hanldeDataEnd(obj, vres);

          obj->client->sendResponse(vres.code, vres.body);
        });
    IStatic->setRecvBodyCallBackup([this, IStatic, app_cb](
                                     VHttpClientObject *obj, VHttpRequest *req,
                                     VHttpResponse *res, const VBuf *buf) {
      obj->cache_data.appendData(buf->getConstData(), buf->size());
      if (obj->cache_data.size() > VWEB_HTTP_MAX_BODY_CACHE_LENGTH -
                                       VWEB_VOPENSSL_MAX_CACHE_BUFFER_SIZE) {
        VWebAppRequest vreq;
        VWebAppResponse vres;
        vreq.request_p = req;
        vres.response_p = res;
        vreq.body = obj->cache_data;

        this->hanldeDataBegin(vreq, vres);

        if (app_cb) {
          vres.code = app_cb(vreq, vres);
        }

        this->hanldeDataEnd(obj, vres);

        obj->client->sendResponse(vres.code, vres.body);
      }
    });
    
  }
  return this->addWebInterface(IStatic);

}

void VWebAppServer::setHttpVersion(VString version) {
  httpVersion_ = version;
}

void VWebAppServer::setContentType(VString ctype) {
  contentType_ = ctype;
}

void VWebAppServer::hanldeDataBegin(VWebAppRequest &vreq,
                                    VWebAppResponse &vres) {
  VHttpRequest *req = vreq.request_p;
  VHttpResponse *res = vres.response_p;

  vreq.headers = req->getHeaders();
  vreq.method_type = req->getMethod();



  if (req->getMethod() == METHOD_TYPE::GET) {
    // 从查询字符串中提取参数
    vreq.query_data =
        req->getVHttpParser()->parseUrlEncoded(req->getParsedURL().query);
  } else if (VString(req->getContentType())
                 .contains("application/x-www-form-urlencoded")) {
    // 解析URL编码的表单数据
    VString body = VString(vreq.body.getConstData(), vreq.body.size());
    vreq.x_www_form_data = req->getVHttpParser()->parseUrlEncoded(body);
  } else if (VString(req->getContentType()).contains("multipart/form-data")) {
    vreq.multi_part_data.parseFromBodyData(vreq.body, req->getContentType());
  }

  res->resetParser();
  res->setHttpVersion(httpVersion_.stdString());
  res->setUseGzip(req->getUseGzip());
  res->setContentType(contentType_.stdString());
}

void VWebAppServer::hanldeDataEnd(VHttpClientObject *obj,
                                  VWebAppResponse &vres) {
  VHttpRequest *req = obj->client->getVHttpRequest();
  VHttpResponse *res = obj->client->getVHttpResponse();

  if (vres.body.size() == 0 && vres.code == 403) {
    res->setContentType("text/html; charset=utf-8");
    obj->client->sendResponse(vres.code, page_403_buf);
  } else if (vres.body.size() == 0 && vres.code == 404) {
    res->setContentType("text/html; charset=utf-8");
    obj->client->sendResponse(vres.code, page_404_buf);
  } else if (vres.body.size() == 0 && vres.code == 405) {
    res->setContentType("text/html; charset=utf-8");
    obj->client->sendResponse(vres.code, page_405_buf);
  } else if (vres.body.size() == 0 && vres.code == 500) {
    res->setContentType("text/html; charset=utf-8");
    obj->client->sendResponse(vres.code, VBuf("Login server error code 500"));
  }
}