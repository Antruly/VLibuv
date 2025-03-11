#include <VHttpClient.h>
#include <VString.h>

int test1() {
  VHttpClient client;
  VHttpRequest *pRequest = client.getVHttpRequest();
  VHttpResponse *pResponse = client.getVHttpResponse();

  pRequest->setMethod(GET);
  pRequest->setHttpVersion(VWEB_HTTP_VERSION_1_1);
  pRequest->setKeepAlive(true);
  pRequest->setUseGzip(false);
  pRequest->setUserAgent(
      "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/122.0.0.0 Safari/537.36");
  pRequest->addHeader("Upgrade-Insecure-Requests", "1");
  pRequest->setAccept(
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
  pRequest->addHeader("Accept-Language", "zh-CN,zh;q=0.9");

  bool bRet = client.sendRequest("https://www.baidu.com");

  if (!bRet) {
    return -1;
  }
  client.readStart();
  VBuf recvBody;
  size_t index = client.waitRecvResponse();
  int statusCode = pResponse->getStatusCode();
  HttpHeaders responseHeaders = pResponse->getHeaders();

  Log->logDebug("response Headers:%d\n", statusCode);
  for (auto header : responseHeaders) {
    Log->logDebug("%s: %s\n", header.first.c_str(), header.second.c_str());
  }

  Log->logDebug("response Status:%d\n", statusCode);
  switch (statusCode) {
  case 200: {

  } break;
  case 301:
  case 302: {
    Log->logDebug("response Status:%d Location:%s\n", statusCode,
                  pResponse->getLocation().c_str());
    bRet = client.sendRequest(pResponse->getLocation());
    if (!bRet) {
      return -1;
    }
    client.readStart();
    index = client.waitRecvResponse();
    statusCode = pResponse->getStatusCode();
    responseHeaders = pResponse->getHeaders();

  } break;
  case 404: {
  } break;
  default:
    break;
  }

  Log->logDebug("response body data:\n");
  while (client.waitRecvResponse() > 0) {
    VBuf recvBuf = client.getRecvResponseBody();
    index += recvBuf.size();
    recvBody.append(recvBuf);
  }

  if (recvBody.size() > 0) {
    VString str(recvBody.getConstData(), recvBody.size());
    Log->logDebug("%s", str.c_str());
    recvBody.clear();
  }
  Log->logDebug("\nresponse body size:%ull\n", index);

  client.getVTcpClient()->close();
  return 0;
}
int test2() {
  VHttpClient client;
  VHttpRequest *pRequest = client.getVHttpRequest();
  VHttpResponse *pResponse = client.getVHttpResponse();

  pRequest->setMethod(POST);
  pRequest->setHttpVersion(VWEB_HTTP_VERSION_1_1);
  pRequest->setKeepAlive(true);
  pRequest->setUseGzip(true);
  pRequest->setAccept("*/*");
 

  VHttpMultiPart httpMultiPart;
  pRequest->setContentType( VString("multipart/form-data; boundary=")+
                           httpMultiPart.getMuitiPartString());

  
  VHttpPart httpPart1{"name1", VBuf("value1"), ""};
  VHttpPart httpPart2{"name2", VBuf("value2"), ""};
  VHttpPart httpPart3{"name3", VBuf("value3"), ""};

  httpMultiPart.append(httpPart1);
  httpMultiPart.append(httpPart2);
  httpMultiPart.appendFinally(httpPart3);

  VBuf body = httpMultiPart.getConstData();


  pRequest->setContentLength(body.size());
  bool bRet = client.sendRequest("http://192.168.1.210:9999/test");
  if (!bRet) {
    return -1;
  }
  bRet = client.sendRequestBody(body);

  if (!bRet) {
    return -1;
  }

  client.readStart();
  VBuf recvBody;
  size_t index = client.waitRecvResponse();
  int statusCode = pResponse->getStatusCode();
  HttpHeaders responseHeaders = pResponse->getHeaders();

  Log->logDebug("response Headers:%d\n", statusCode);
  for (auto header : responseHeaders) {
    Log->logDebug("%s: %s\n", header.first.c_str(), header.second.c_str());
  }

  Log->logDebug("response Status:%d\n", statusCode);
  switch (statusCode) {
  case 200: {

  } break;
  case 301:
  case 302: {
    Log->logDebug("response Status:%d Location:%s\n", statusCode,
                  pResponse->getLocation().c_str());
    bRet = client.sendRequest(pResponse->getLocation());
    if (!bRet) {
      return -1;
    }
    bRet = client.sendRequestBody(body);

    if (!bRet) {
      return -1;
    }
    client.readStart();
    index = client.waitRecvResponse();
    statusCode = pResponse->getStatusCode();
    responseHeaders = pResponse->getHeaders();

  } break;
  case 404: {
  } break;
  default:
    break;
  }

  Log->logDebug("response body data:\n");
  while (client.waitRecvResponse() > 0) {
    VBuf recvBuf = client.getRecvResponseBody();
    index += recvBuf.size();
    recvBody.append(recvBuf);
  }

  if (recvBody.size() > 0) {
    VString str(recvBody.getConstData(), recvBody.size());
    Log->logDebug("%s", str.c_str());
    recvBody.clear();
  }
  Log->logDebug("\nresponse body size:%llu\n", index);

  client.getVTcpClient()->close();
  return 0;
}
int main() {
  test1();
  //test2();
  return 0;
}