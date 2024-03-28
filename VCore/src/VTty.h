#pragma once
#include "VLoop.h"

class VTty : public VHandle {
 public:
  VCORE_DEFINE_INHERIT_FUNC(VTty);
  VCORE_DEFINE_COPY_FUNC(VTty);

  VTty(VLoop* loop, uv_file fd, int readable);

  int init();
  int init(VLoop* loop, uv_file fd, int readable);
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 1
  int setMode(uv_tty_mode_t mode);
#endif
#endif

  int resetMode(void);
  int getWinsize(int* width, int* height);

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 33
 void setVtermState(uv_tty_vtermstate_t state);
  int getVtermState(uv_tty_vtermstate_t* state);
#endif
#endif
 

 protected:
 private:
 private:
};
