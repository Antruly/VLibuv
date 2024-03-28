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
  pRequest->setUserAgent("Test Client version 1.0");
  pRequest->addHeader("header1", "value1");
  pRequest->addHeader("header2", "value2");
  bool bRet = client.sendRequest("https://www.huorong.cn");
  if (!bRet) {
    return -1;
  }
  client.readStart();
  VBuf recvBody;
  size_t index = client.waitRecvResponse();
  int statusCode = pResponse->getStatusCode();
  std::map<std::string, std::string> responseHeaders = pResponse->getHeaders();

  printf("response Headers:%d\n", statusCode);
  for (auto header : responseHeaders) {
    printf("%s: %s\n", header.first.c_str(), header.second.c_str());
  }
  
  printf("response Status:%d\n", statusCode);
  switch (statusCode) {
    case 200:{


    }
      break;
    case 301:
    case 302: {
      printf("response Status:%d Location:%s\n", statusCode, pResponse->getLocation().c_str());
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
 

  printf("response body data:\n");
  while (client.waitRecvResponse() > 0) {
    VBuf recvBuf = client.getRecvResponseBody();
    index += recvBuf.size();
    recvBody.appand(recvBuf);
  }

  if (recvBody.size() > 0) {
    VString str(recvBody.getConstData(), recvBody.size());
    printf("%s", str.c_str());
    recvBody.clear();
  }
  printf("\nresponse body size:%zu\n", index);

  client.getVTcpClient()->close();
  return 0;
}