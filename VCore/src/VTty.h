#pragma once
#include "VLoop.h"

class VTty : public VHandle {
 public:
  DEFINE_INHERIT_FUNC(VTty);
  DEFINE_COPY_FUNC(VTty);

  VTty(VLoop* loop, uv_file fd, int readable);

  int init();
  int init(VLoop* loop, uv_file fd, int readable);
  int setMode(uv_tty_mode_t mode);
  int resetMode(void);
  int getWinsize(int* width, int* height);
  void setVtermState(uv_tty_vtermstate_t state);
  int getVtermState(uv_tty_vtermstate_t* state);

 protected:
 private:
 private:
};
