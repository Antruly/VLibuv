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
    <title>403 - ��ֹ����</title>
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
        <p>��Ǹ������Ȩ���ʴ�ҳ�档</p>
        <p>�������Ϊ����һ����������ϵ��վ����Ա��</p>
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
    <title>404 - ҳ��δ�ҵ�</title>
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
        <p>��Ǹ�������ʵ�ҳ�治���ڡ�</p>
        <p>���� URL �Ƿ���ȷ���򷵻� <a href="/">��ҳ</a>��</p>
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
    <title>405 - ������������</title>
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
        <p>����ķ�����������</p>
        <p>������������ʽ���򷵻� <a href="/">��ҳ</a>��</p>
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

  // ������̬�ļ�������
  void handleStaticFile(VHttpClient *client, VWebInterface *staticInterface,
                        const VString &rawPath);
  // �м��֧��
  void use(std::function<void(VHttpClient *)> middleware) {
    _middlewares.push_back(middleware);
  }

  // ·�ɲ�������
  std::map<VString, VString> parseRouteParams(const VString &pattern,
                                              const VString &path);

  // �������
  void addRequestFilter(std::function<bool(VHttpRequest *)> filter);

  // ������
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
