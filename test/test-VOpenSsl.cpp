#pragma once
#include <VHttpClient.h>
#include <VOpenSsl.h>
#include <VString.h>

bool connect(VTcpClient& client, const std::string& ip, int port) {
  client.connect(ip.c_str(), port);
  client.run(uv_run_mode::UV_RUN_ONCE);

  if ((client.getStatus() & VTCP_WORKER_STATUS::VTCP_WORKER_STATUS_CONNECTED) ==
      0) {
    return false;
  }
  return true;
}

void readData(VTcpClient& client, VBuf& buf, uint64_t maxTimeout = 30000) {
  VBuf readBuf;
  client.setReadCb(
      [&readBuf](VTcpClient* tcp, const VBuf* buf) { readBuf.append(*buf); });
  client.clientReadStart();

  size_t readIndex = 0;
  uint64_t timeout = 0;
  while (true) {
    if (timeout > maxTimeout) {
      break;
    }
    client.run(uv_run_mode::UV_RUN_NOWAIT);
    if (readBuf.size() == readIndex && readIndex != 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      client.run(uv_run_mode::UV_RUN_NOWAIT);
      if (readBuf.size() == readIndex) {
        break;
      }
    }
    if (readBuf.size() > 0) {
      readIndex = readBuf.size();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout += 10;
  }
  buf = readBuf;
}

void writeData(VTcpClient& client, const VBuf& buf) {
  client.writeData(buf);
  client.run(uv_run_mode::UV_RUN_ONCE);

}
int main1() {
  std::string sendData =
      "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: "
      "keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 "
      "(Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/122.0.0.0 Safari/537.36\r\nAccept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/apng,*/*;q=0.8,application/"
      "signed-exchange;v=b3;q=0.7\r\nAccept-Language: zh-CN,zh;q=0.9\r\n\r\n";
  VTcpClient client;
  connect(client, "183.2.172.185", 443);

  VOpenSsl ssl(SSLv23_client_method());

  VBuf readSslConnectData;
  VBuf writeSslConnectData;

  VBuf readSslSendData;
  VBuf writSsleSendData;

  bool isConnect = false;
  ssl.setSslConnectiondCb(
      [&client, &isConnect](VOpenSsl* vssl, int status) { isConnect = true; });

  ssl.setSslWriteCb([&ssl, &client, &isConnect, &writeSslConnectData, &writSsleSendData](
          VOpenSsl* vssl, const VBuf* buf, int status) {
    if (isConnect) {
      writSsleSendData.append(*buf);
    } else {
      writeSslConnectData.append(*buf);
    }

    if (buf->size() > 0) {
      writeData(client, *buf);
      VBuf readBuf;
      readData(client, readBuf);

      if (readBuf.size() > 0) {
        VBuf writeBuf;
        ssl.sslParserData(readBuf, writeBuf);
      } else {
        Log->logDebugWarn("readData is null\n");
      }
    } else {
      Log->logDebugWarn("writeData is null\n");
    }
  });

  ssl.setSslReadCb([&ssl, &client, &isConnect, &readSslConnectData,
                    &readSslSendData](VOpenSsl* vssl, const VBuf* buf) {
    if (isConnect) {
      readSslSendData.append(*buf);
    } else {
      readSslConnectData.append(*buf);
    }
  });

 if (!ssl.sslConnect()) {
      return -1;
    }


  VBuf readSendData;
  VBuf writeSendData(sendData.c_str(), sendData.size());

  ssl.sslPackData(writeSendData, writSsleSendData);

  return 0;
}
int main2() {
  std::string sendData =
      "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection: "
      "keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 "
      "(Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
      "Chrome/122.0.0.0 Safari/537.36\r\nAccept: "
      "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/"
      "webp,image/apng,*/*;q=0.8,application/"
      "signed-exchange;v=b3;q=0.7\r\nAccept-Language: zh-CN,zh;q=0.9\r\n\r\n";
  VTcpClient client;
  connect(client, "183.2.172.185", 443);

  VOpenSsl ssl(&client, SSLv23_client_method());

  VBuf readSslConnectData;
  VBuf writeSslConnectData;

  VBuf readSslSendData;
  VBuf writSsleSendData;

   bool isConnect = false;
  ssl.setSslConnectiondCb(
      [&client, &isConnect](VOpenSsl* ssl, int status) {
     isConnect = true;
   });

  ssl.setSslWriteCb([&ssl, &client, &isConnect, &writeSslConnectData, &writSsleSendData](
          VOpenSsl* ssl, const VBuf* buf, int status) {
    if (isConnect) {
      writSsleSendData.append(*buf);
    } else {
      writeSslConnectData.append(*buf);
    }

  });

  ssl.setSslReadCb([&ssl, &client, &isConnect, &readSslConnectData,
                    &readSslSendData](VOpenSsl* ssl, const VBuf* buf) {
    if (isConnect) {
      readSslSendData.append(*buf);
    } else {
      readSslConnectData.append(*buf);
    }
  });

   if (!ssl.sslConnect()) {
    return -1;
  }

  
  VBuf readSendData;
  VBuf writeSendData(sendData.c_str(), sendData.size());

  ssl.sslPackData(writeSendData, writSsleSendData);
  return 0;
}

int main() {
  //main1();
  main2();
  return 0;
}
