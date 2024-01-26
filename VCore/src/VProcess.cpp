#include "VProcess.h"


const char* _STR_NULL = "";
const char* _STR_NULL_ARRY[] = {_STR_NULL, nullptr};

std::vector<std::string> stringSplit(const std::string& str, char delim) {
  std::size_t previous = 0;
  std::size_t current = str.find(delim);
  std::vector<std::string> elems;
  while (current != std::string::npos) {
    if (current > previous) {
      elems.push_back(str.substr(previous, current - previous));
    }
    previous = current + 1;
    current = str.find(delim, previous);
  }
  if (previous != str.size()) {
    elems.push_back(str.substr(previous));
  }
  return elems;
}

VProcess::VProcess() : VHandle(this) {
  uv_process_t* process = (uv_process_t*)VMemory::malloc(sizeof(uv_process_t));
  this->setHandle(process);
  this->init();
  options = (uv_process_options_t*)VMemory::malloc(sizeof(uv_process_options_t));
  memset(options, 0, sizeof(uv_process_options_t));
}

VProcess::VProcess(VLoop* loop) : VHandle(this) {
  uv_process_t* process = (uv_process_t*)VMemory::malloc(sizeof(uv_process_t));
  this->setHandle(process);
  this->init(loop);
  options = (uv_process_options_t*)VMemory::malloc(sizeof(uv_process_options_t));
  memset(options, 0, sizeof(uv_process_options_t));
 
}
VProcess::VProcess(VProcess* t_p) : VHandle(t_p) {}
VProcess::~VProcess() {
  VMemory::free(options);
  if (op_args != nullptr) {
    delete op_args;
    op_args = nullptr;
  }
}

int VProcess::init() { 
  memset(VPROCESS_HANDLE, 0, sizeof(uv_process_t));
  this->setHandleData();
  return 0;
}

int VProcess::init(VLoop* lp) {
  loop = lp;
  memset(VPROCESS_HANDLE, 0, sizeof(uv_process_t));
  this->setHandleData();
  return 0;
}

void VProcess::setOptions(const char* file, const char** args, const char** env,
                          const char* cwd, unsigned int flags) {
  memset(options, 0, sizeof(uv_process_options_t));

  if (args == nullptr) {
    args = _STR_NULL_ARRY;
  }
  options->file = file;
  options->args = (char**)args;
  options->env = (char**)env;
  options->cwd = cwd;
  options->flags = flags;
}

void VProcess::setOptions(const std::string args) {
  memset(options, 0, sizeof(uv_process_options_t));
  argslist = stringSplit(args, ' ');

  if (op_args != nullptr) {
    delete op_args;
    op_args = nullptr;
  }
  op_args = new const char*[argslist.size() + 1];
  op_args[argslist.size()] = nullptr;

  int i = 0;
  for (std::vector<std::string>::iterator item = argslist.begin();
       item != argslist.end(); item++, i++) {
    op_args[i] = item->c_str();
  }

  options->file = argslist.begin()->c_str();
  options->args = (char**)op_args;
  options->env = nullptr;
  options->cwd = nullptr;
  options->flags = 0;
}

int VProcess::kill(int signum) {
  return uv_process_kill(VPROCESS_HANDLE, signum);
}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 18
int VProcess::getPid() { return uv_process_get_pid(VPROCESS_HANDLE); }
int VProcess::processGetPid(const VProcess* hd) {
  return uv_process_get_pid(OBJ_VPROCESS_HANDLE(*hd));
}
#endif
#endif


int VProcess::processKill(VProcess* hd, int signum) {
  return uv_process_kill(OBJ_VPROCESS_HANDLE(*hd), signum);
}

int VProcess::kill(int pid, int signum) { return uv_kill(pid, signum); }



 int VProcess::start(
    std::function<void(VProcess*, int64_t, int)> start_cb) {
  if (loop == nullptr) {
    return -1;
  }
  process_start_cb = start_cb;
  options->exit_cb = callback_start;
  return uv_spawn((uv_loop_t*)loop->getHandle(), VPROCESS_HANDLE, options);
 }

  void VProcess::callback_start(uv_process_t* handle,
                                      int64_t exit_status,
                                      int term_signal) {
  if (reinterpret_cast<VProcess*>(handle->data)->process_start_cb)
    reinterpret_cast<VProcess*>(handle->data)
        ->process_start_cb(reinterpret_cast<VProcess*>(handle->data),
                           exit_status, term_signal);
 }
