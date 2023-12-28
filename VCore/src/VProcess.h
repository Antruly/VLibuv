#pragma once
#include "VHandle.h"
#include "VLoop.h"


class VProcess : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VProcess);
  DEFINE_COPY_FUNC_DELETE(VProcess);
  VProcess(VLoop* loop);

  int init();
  int init(VLoop* loop);
  void setOptions(const char* file, const char** args,
                  const char** env = nullptr, const char* cwd = nullptr,
                  unsigned int flags = 0);
  void setOptions(const std::string args);
  int kill(int signum);

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
  int getPid();
  static int processGetPid(const VProcess* hd);
#endif
#endif


  static int processKill(VProcess* hd, int signum);
  static int kill(int pid, int signum);


  int start(std::function<void(VProcess*, int64_t, int)> start_cb);

 protected:
  std::function<void(VProcess*, int64_t, int)> process_start_cb;
 private:
  static void callback_start(uv_process_t* handle, int64_t exit_status,
                             int term_signal);

 private:
  VLoop* loop = nullptr;
  uv_process_options_t* options = nullptr;
  const char** op_args = nullptr;
  std::vector<std::string> argslist;
};
