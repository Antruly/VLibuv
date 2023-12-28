#pragma once
#include "VBarrier.h"
#include "VIdle.h"
#include "VInterfaceAddress.h"
#include "VLoop.h"
#include "VProcess.h"
#include "VRwlock.h"
#include "VThread.h"
#include "VTimer.h"
#include "VString.h"
#include <functional>

#ifndef __VTIMER_H_
#define __VTIMER_H_
#include <WinBase.h>
#include <winsock.h>

#include "VBuf.h"
#include "VFs.h"
#include "VPipe.h"
#include "VStream.h"
#include "VTcp.h"
#include <io.h>
#include "VTcpService.h"




//#pragma comment(lib, "Wsock32.lib")
SOCKET m_Socket;
struct timeval m_Tv;
fd_set m_Dummy;

// 获取当前时间
LARGE_INTEGER GetCurrentLARGETime() {
  LARGE_INTEGER SetTime;
  QueryPerformanceCounter(&SetTime);

  return SetTime;
}

// 比较当前时间与time差值(um)
int64_t ComparisonCurrentTime(LARGE_INTEGER time) {
  LARGE_INTEGER perfCnt, now;

  QueryPerformanceFrequency(&perfCnt);
  QueryPerformanceCounter(&now);

  return (now.QuadPart - time.QuadPart) / double(perfCnt.QuadPart) * 1000000;
}

// 比较时间time2与time1差值(um),time2大于time1
int64_t ComparisonTime(LARGE_INTEGER time2, LARGE_INTEGER time1) {
  LARGE_INTEGER perfCnt;

  QueryPerformanceFrequency(&perfCnt);

  return (time2.QuadPart - time1.QuadPart) / double(perfCnt.QuadPart) * 1000000;
}

// socket超时选择, 精度99%
void SleepSelectUS(int64_t usec) {
  FD_ZERO(&m_Dummy);
  FD_SET(m_Socket, &m_Dummy);
  m_Tv.tv_sec = usec / 1000000L;
  m_Tv.tv_usec = usec % 1000000L;
  select(0, 0, 0, &m_Dummy, &m_Tv);
  // int64_t err = GetLastError();
  // if (err != 0)
  //	printf("Error : %d", err);
}

// 时钟轮训, 精度100%
void SleepPerformUS(int64_t usec) {
  LARGE_INTEGER perfCnt, start, now;

  QueryPerformanceFrequency(&perfCnt);
  QueryPerformanceCounter(&start);

  if (usec > 1000000L) {
    Sleep((usec - 1000000L) / 1000L);
    if (usec - 1000000L > 10000L) {
      SleepSelectUS(usec - 1000000L - 10000L);
    }
  } else if (usec > 10000L) {
    SleepSelectUS(usec - 10000L);
  }

  do {
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
  } while ((now.QuadPart - start.QuadPart) / float(perfCnt.QuadPart) * 1000000 <
           usec);
}

#endif  // !__VTIMER_H_

int num = 0;
int num2 = 0;

typedef struct {
  VWrite req;
  VBuf buf;
} write_req_t;

class TestClass {
 public:
  LARGE_INTEGER startTime;
  VBarrier blocker;
  VRwlock numlock;
  int shared_num;

  VLoop loop;

 public:
  void waitVIdleCallback(VIdle* handle) {
    printf("waitcallback:%d \n", num++);

    Sleep(1000);
  }
  void waitVIdleCallback2(VIdle* handle) {
    printf("waitcallback2:%d \n", num2++);

    Sleep(1000);
  }

  void waitVTimerCallback(VTimer* handle) {
    printf("times:%f \n", ComparisonCurrentTime(startTime) / 1.0);
    startTime = GetCurrentLARGETime();
    printf("waitVTimerCallback:%d \n", num++);

    // Sleep(1000);
  }
  void waitVTimerCallback2(VTimer* handle) {
    printf("waitVTimerCallback2:%d \n", num2++);

    // Sleep(1000);
  }

  void waitVThreadCallback(void* pdata) {
    num = 99;
    while (num--) {
      printf("waitVThreadCallback:%d \n", *(int*)pdata);
      Sleep(700);
    }
  }
  void waitVThreadCallback2(void* pdata) {
    num2 = 99;
    while (num2--) {
      printf("waitVThreadCallback2:%d \n", *(int*)pdata);
      Sleep(500);
    }
  }

  void processExit(VProcess* req, int64_t exit_status, int term_signal) {
    printf("Process exited with status %d, signal %d\n", exit_status,
           term_signal);
  }

  void reader(void* n) {
    int num = *(int*)n;
    int i;
    for (i = 0; i < 20; i++) {
      numlock.rdlock();
      printf("Reader %d: acquired lock\n", num);
      printf("Reader %d: shared num = %d\n", num, shared_num);
      numlock.rdunlock();
      printf("Reader %d: released lock\n", num);
    }
    blocker.wait();
  }

  void writer(void* n) {
    int num = *(int*)n;
    int i;
    for (i = 0; i < 20; i++) {
      numlock.wrlock();
      printf("Writer %d: acquired lock\n", num);
      shared_num++;
      printf("Writer %d: incremented shared num = %d\n", num, shared_num);
      numlock.wrunlock();

      printf("Writer %d: released lock\n", num);
    }
    blocker.wait();
  }

  void alloc_buffer(VHandle* handle, size_t suggested_size, VBuf* buf) {
    buf->resize(suggested_size);
  }

  void echo_write(VWrite* req, int status) {
    if (status < 0) {
      fprintf(stderr, "Write error %s\n", uv_err_name(status));
    }
    delete req;
  }

  void echo_read(VStream* client, ssize_t nread, const VBuf* buf) {
    if (nread > 0) {
      write_req_t* req = new write_req_t();
      req->buf.resize(nread);


      client->write(
          (VWrite*)req, &req->buf, 1,
                    std::bind(&TestClass::echo_write, this,
                              std::placeholders::_1, std::placeholders::_2));
      return;
    }

    if (nread < 0) {
      if (nread != UV_EOF)
        fprintf(stderr, "Read error %s\n", uv_err_name(nread));

      client->close();
    }

    buf->~VBuf();
    free((void*)buf);
  }

  void on_new_connection(VStream* server, int status) {
    if (status == -1) {
      // error!
      return;
    }

    VPipe* client = new VPipe();
    client->init(&loop, 0);

    if (server->accept(client) == 0) {
        //std::bind(&TestClass::echo_write, this,std::placeholders::_1
      client->readStart(std::bind(&TestClass::alloc_buffer, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3),
          std::bind(&TestClass::echo_read, this, std::placeholders::_1,
                    std::placeholders::_2, std::placeholders::_3));
    } else {
      client->close();
    }
  }

  void testVFs(VFs* fs) {
    int ret = fs->getResult();
    if (ret != 0) {
      fprintf(stderr, "Listen error %s\n", uv_err_name(ret));
      return;
    }
    printf("%s \n", fs->getPath());
    VDir* dir = new VDir();
    VDirent* dirent = new VDirent();
    dir->setDirent(dirent);
    ret = fs->readdir(
        &this->loop, dir,
        std::bind(&TestClass::testVFs2, this, std::placeholders::_1));
   //ret = fs->copyfile(&this->loop, fs->getPath(), "D:/12345.iso", 0,&TestClass::testVFs2);

   if (ret != 0)
   {
      fprintf(stderr, "Listen error %s\n", uv_err_name(ret));
   }
    
  }
  void testVFs2(VFs* fs) { 
      int ret = fs->getResult();
   if (ret != 0) {
      fprintf(stderr, "Listen error %s\n", uv_err_name(ret));
      return;
   }
   }
};

int testTimer() {
  TestClass test;
  VLoop loop;
  VTimer timer(&loop);
  VTimer timer2(&loop);

  timer.start(
      std::bind(&TestClass::waitVTimerCallback, &test, std::placeholders::_1),
      0, 1000);
  timer2.start(
      std::bind(&TestClass::waitVTimerCallback2, &test, std::placeholders::_1),
      9000, 1000);

  return loop.exec();
}

int testVIdle() {
  TestClass test;
  VLoop loop;
  VIdle idle(&loop);
  VIdle idle2(&loop);
  idle.start(
      std::bind(&TestClass::waitVIdleCallback, &test, std::placeholders::_1));
  idle2.start(
      std::bind(&TestClass::waitVIdleCallback2, &test, std::placeholders::_1));

  return loop.exec();
}

int testVThread() {
  TestClass test;
  int data = 10;
  int data2 = 20;
  VThread thread;
  VThread thread2;
  thread.start(
      std::bind(&TestClass::waitVThreadCallback, &test, std::placeholders::_1),
      &data);
  thread2.start(
      std::bind(&TestClass::waitVThreadCallback2, &test, std::placeholders::_1),
      &data);

  thread.join();
  thread2.join();

  return 0;
}

int testVProcess() {
  TestClass test;
  VLoop loop;

  VProcess process(&loop);

  const char* args[6];
  args[0] = "D:\\TeamViewer\\TeamViewer.exe";
  args[1] = "-i";
  args[2] = "192.168.1.200";
  args[3] = "-P";
  args[4] = "yyqujnbs";
  args[5] = nullptr;

  // process.setOptions("D:\\TeamViewer\\TeamViewer.exe", args);
  process.setOptions(
      "D:\\TeamViewer\\TeamViewer.exe -i 192.168.1.200 -P yyqujnbs");

  int r;
  
  if ((r = process.start(std::bind(&TestClass::processExit, &test,
                                   std::placeholders::_1, std::placeholders::_2,
                                   std::placeholders::_3)))) {
    fprintf(stderr, "%s\n", uv_strerror(r));
    return 1;
  } else {
    fprintf(stderr, "Launched process with ID %d\n", process.getPid());
  }

  return loop.exec();
}

void testLock() {
  TestClass* test = new TestClass();

  test->blocker.init(4);

  test->shared_num = 0;
  test->numlock.init();

  VThread* threads = new VThread[3]();

  int* thread_nums = new int[3];
  thread_nums[0] = 1;
  thread_nums[1] = 2;
  thread_nums[2] = 1;

  threads[0].start(std::bind(&TestClass::reader, test, std::placeholders::_1),
                   &thread_nums[0]);
  threads[1].start(std::bind(&TestClass::reader, test, std::placeholders::_1), &
                   thread_nums[1]);
  threads[2].start(std::bind(&TestClass::writer, test, std::placeholders::_1),
                   &thread_nums[2]);

  test->blocker.wait();
  test->blocker.destroy();
  test->numlock.destroy();
}

void testVInterfaceAddress() {
  std::vector<VInterfaceAddress> addrs =
      VInterfaceAddress::getAllInterfaceAddresses();

  for (std::vector<VInterfaceAddress>::iterator item = addrs.begin();
       item != addrs.end(); item++) {
    printf("%s \n", item->getIpv4Addrs().c_str());
  }
}

int testVStream() {
  TestClass* test = new TestClass();

  VPipe server;

  server.init(&test->loop, 0);

  // signal(SIGINT, remove_sock);

  int r;
  if ((r = server.bind("\\\\?\\pipe\\echo.sock"))) {
    fprintf(stderr, "Bind error %s\n", uv_err_name(r));
    return 1;
  }

  if ((r = server.listen(
           std::bind(&TestClass::on_new_connection, test, std::placeholders::_1,
                     std::placeholders::_2),
                         128))) {
    fprintf(stderr, "Listen error %s\n", uv_err_name(r));
    return 2;
  }
  return test->loop.exec();
}




int testVFs() {
  TestClass* test = new TestClass();
  VFs fs;
  VDir dir;
  char gbk[] = {0x44, 0x3a, 0x2f, 0xe5, 0xb7, 0xa5, 0xe5,
                0x85, 0xb7, 0x2f, 0x56, 0x53, 0x32, 0x30,
                0x31, 0x30, 0x2e, 0x69, 0x73, 0x6f, 0x00};

   char utf8[] = {0x44, 0x3a, 0x2f, 0xe5, 0xb7, 0xa5, 0xe5,
                0x85, 0xb7, 0x2f, 0x56, 0x53, 0x32, 0x30,
                0x31, 0x30, 0x2e, 0x69, 0x73, 0x6f, 0x00};
  char utf8_2[] = {0x44, 0x3a, 0x2f, 0x56, 0x53, 0x32, 0x30,
                 0x31, 0x30, 0x2e, 0x69, 0x73, 0x6f, 0x00};

  const char* sss = "D:/123.html";

  int ret = 0;

  fs.open(&test->loop, utf8, 0, O_RDONLY,
          std::bind(&TestClass::testVFs, test, std::placeholders::_1));
  return test->loop.exec();
}
void uvfscb(uv_fs_t* fs) {
  int ret = uv_fs_get_result(fs);
  if (ret >= 0) {
    fprintf(stderr, "Listen error %s\n", uv_err_name(ret));
    return;
  }
}
class testClass {
 public:
  void start(uv_idle_t*  aaa) {
  
  }
};


int main() {


    { 
         VTcpService test;
    VTcpService test2;

    test.listenIpv4("0.0.0.0", 8085, 128);
         test2.listenIpv4("0.0.0.0", 8086, 128);

    return test.run();
    }
 
  int aa = sizeof(VBasePtr);

   testVIdle();
   testTimer();
   testVThread();
   testLock();
   testVProcess();
   testVInterfaceAddress();
   testVStream();
  testVFs();

  system("pause");

  return 0;
}
