#include <VHttpClient.h>
#include <VString.h>
int main() {
  VHttpClient client;
  VHttpRequest* pRequest = client.getVHttpRequest();
  VHttpResponse* pResponse = client.getVHttpResponse();

  pRequest->setMethod(GET);
  pRequest->setHttpVersion(VWEB_HTTP_VERSION_1_1);
  pRequest->setKeepAlive(true);
  pRequest->setUseGzip(false);
  pRequest->setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
  pRequest->addHeader("Upgrade-Insecure-Requests", "1");
  pRequest->setAccept("text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7");
  pRequest->addHeader("Accept-Language", "zh-CN,zh;q=0.9");

  bool bRet = client.sendRequest("https://www.baidu.com");

  
  if (!bRet) {
    return -1;
  }
  client.readStart();
  VBuf recvBody;
  size_t index = client.waitRecvResponse();
  int statusCode = pResponse->getStatusCode();
  std::map<std::string, std::string> responseHeaders = pResponse->getHeaders();

  VLogger::Log->logDebug("response Headers:%d\n", statusCode);
  for (auto header : responseHeaders) {
    VLogger::Log->logDebug("%s: %s\n", header.first.c_str(),
                                header.second.c_str());
  }
  
  VLogger::Log->logDebug("response Status:%d\n", statusCode);
  switch (statusCode) {
    case 200:{


    }
      break;
    case 301:
    case 302: {
      VLogger::Log->logDebug("response Status:%d Location:%s\n",
                                  statusCode, pResponse->getLocation().c_str());
      bRet = client.sendRequest(pResponse->getLocation());
      if (!bRet) {
        return -1;
      }
      client.readStart();
      index = client.waitRecvResponse();
      statusCode = pResponse->getStatusCode();
      responseHeaders = pResponse->getHeaders();

    }
      break;
    case 404: {
      }
      break;
    default:
      break;
  }
 

  VLogger::Log->logDebug("response body data:\n");
  while (client.waitRecvResponse() > 0) {
    VBuf recvBuf = client.getRecvResponseBody();
    index += recvBuf.size();
    recvBody.appand(recvBuf);
  }

  if (recvBody.size() > 0) {
    VString str(recvBody.getConstData(), recvBody.size());
    VLogger::Log->logDebug("%s", str.c_str());
    recvBody.clear();
  }
  VLogger::Log->logDebug("\nresponse body size:%zu\n", index);

  client.getVTcpClient()->close();
  return 0;
}