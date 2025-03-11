#pragma once
#include "VHttpServer.h"
#include "VTimerInfo.h"
#include "VHttpClientObject.h"
#include "VWebInterface.h"
#include "VWebRouter.h"

#pragma region static code
static const char *page_403 = R"raw(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>403 - 禁止访问</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f8d7da;
            text-align: center;
            padding: 50px;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }
        h1 {
            font-size: 50px;
            color: #721c24;
        }
        p {
            font-size: 18px;
            color: #666;
        }
        .footer {
            margin-top: 20px;
            font-size: 14px;
            color: #999;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>403</h1>
        <p>抱歉，您无权访问此页面。</p>
        <p>如果您认为这是一个错误，请联系网站管理员。</p>
        <div class="footer">VLibuv - VWebServer 1.0</div>
    </div>
</body>
</html>
)raw";
static const char *page_404 = R"raw(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>404 - 页面未找到</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            text-align: center;
            padding: 50px;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }
        h1 {
            font-size: 50px;
            color: #333;
        }
        p {
            font-size: 18px;
            color: #666;
        }
        .footer {
            margin-top: 20px;
            font-size: 14px;
            color: #999;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>404</h1>
        <p>抱歉，您访问的页面不存在。</p>
        <p>请检查 URL 是否正确，或返回 <a href="/">首页</a>。</p>
        <div class="footer">VLibuv - VWebServer 1.0</div>
    </div>
</body>
</html>
)raw";
static const char *page_405 = R"raw(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>405 - 方法不被允许</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #fff3cd;
            text-align: center;
            padding: 50px;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }
        h1 {
            font-size: 50px;
            color: #856404;
        }
        p {
            font-size: 18px;
            color: #666;
        }
        .footer {
            margin-top: 20px;
            font-size: 14px;
            color: #999;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>405</h1>
        <p>请求的方法不被允许。</p>
        <p>请检查您的请求方式，或返回 <a href="/">首页</a>。</p>
        <div class="footer">VLibuv - VWebServer 1.0</div>
    </div>
</body>
</html>
)raw";

static const VBuf page_403_buf(VString(page_403).toUtf8String());
static const VBuf page_404_buf(VString(page_404).toUtf8String());
static const VBuf page_405_buf(VString(page_405).toUtf8String());

#pragma endregion

class VWebServer : public VObject {

public:
  VWebServer();
  virtual ~VWebServer() {}
  VHttpServer *getVHttpServer();

  int listenIpv4(const VString &addripv4, short port);
  int run();

  int stop();
  void waitStoped();

  bool addWebInterface(VWebInterface *webInterface);

  std::map<VString, VWebInterface *> &getWebInterfaces();

  bool addClientCookie(VString cookieName, void *clientData);

  std::map<VString, void *> &getClientCookies();

protected:
  virtual bool parseRequest(VHttpClient *client);
  virtual void runClientKeepAlive(VHttpClientObject *clientObj,
                                  uint64_t maxTimeout = 30000);
  virtual void handleIdleState(VHttpClientObject *clientObj);
  virtual void handleReadingHead(VHttpClientObject *clientObj);
  virtual void handleReadingBody(VHttpClientObject *clientObj);
  virtual void removeClosedClients();

  // 新增静态文件处理方法
  void handleStaticFile(VHttpClient *client, VWebInterface *staticInterface,
                        const VString &rawPath);
  // 中间件支持
  void use(std::function<void(VHttpClient *)> middleware) {
    _middlewares.push_back(middleware);
  }

  // 路由参数解析
  std::map<VString, VString> parseRouteParams(const VString &pattern,
                                              const VString &path);

  // 请求过滤
  void addRequestFilter(std::function<bool(VHttpRequest *)> filter);

  // 错误处理
  void setErrorHandler(std::function<void(VHttpResponse *, int)> handler);

  VString getGmtTime();

  VString getMimeType(const VString &filePath);

private:
  VHttpServer *_http_server = nullptr;
  std::map<VHttpClient *, VHttpClientObject *> _request_clients;
  std::map<VString, void *> _client_cookies;
  std::vector<std::function<void(VHttpClient *)>> _middlewares;
  std::vector<std::function<bool(VHttpRequest *)>> _filters;
  std::function<void(VHttpResponse *, int)> _error_handler;
  VWebRouter _router;
  uint64_t _client_id_index = 0;
  bool _running = false;
  std::mutex _client_mutex;
};
