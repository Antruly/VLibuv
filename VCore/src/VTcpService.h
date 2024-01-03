#pragma once
#include "VTcp.h"
class VTcpService {
public:
	VTcpService();
	~VTcpService();

public:
	int run(uv_run_mode md = UV_RUN_DEFAULT);
	int listenIpv4(const char* addripv4, int port, int flags);
	void close();
	void close(std::function<void(VHandle*)> closeCallback);

	void closeClient(VTcp* client);

	void writeData(VTcp* client, const VBuf& data);
	void writeCloneData(VTcp* client, const VBuf& data);

	void setCloseCb(std::function<void(VTcp*)> close_cb);
	void setWriteCb(std::function<void(VTcp*, const VBuf*)> write_cb);
	void setReadCb(std::function<void(VTcp*, const VBuf*)> read_cb);
	void setNewClientCb(std::function<void(VTcp*)> new_connection_cb);

private:
	void on_close(VHandle* client);
	void echo_write(VWrite* req, int status);
	void echo_write_clone(VWrite* req, int status);
	void alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf);
	void echo_read(VStream* client, ssize_t nread, const VBuf* buf);
	void on_new_connection(VStream* tcp, int status);

protected:
	VTcp* getVTcpServer();
	VLoop* getVLoop();
	
	virtual void readData(VTcp* client, const VBuf& data);
	virtual void newConnection(VTcp* client);
protected:
	std::function<void(VTcp*)> tcpService_close_cb;
	std::function<void(VTcp*, const VBuf*)> tcpService_write_cb;
	std::function<void(VTcp*, const VBuf*)> tcpService_read_cb;
	std::function<void(VTcp*)> tcpService_new_connection_cb;
private:
	VLoop* loop = nullptr;
	VTcp* tcpServer = nullptr;
	sockaddr_in addr;

private:
};