#pragma once

bool clientResponseRecvBody(VHttpResponse *recv_response, const VBuf *data);

void clientResponseParserFinish(VHttpResponse *recv_response, int status);

void clientResponseParserHeadersFinish(
    VHttpResponse *recv_response, std::map<std::string, std::string> *headers);
