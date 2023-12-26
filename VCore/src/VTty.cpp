#include "VTty.h"
DEFINE_COPY_FUNC_HANDLE_CPP(VTty, uv_tty_t);

VTty::VTty() : VHandle(this) {
  uv_tty_t* tty = (uv_tty_t*)VCore::malloc(sizeof(uv_tty_t));
  this->setHandle(tty);
  this->init();
}
VTty::VTty(VTty* t_p) : VHandle(t_p) {}
VTty::~VTty() {}

VTty::VTty(VLoop* loop, uv_file fd, int readable) : VHandle(this) {
  uv_tty_t* tty = (uv_tty_t*)VCore::malloc(sizeof(uv_tty_t));
  this->setHandle(tty);
  this->init(loop, fd, readable);
}

int VTty::init() { 
  memset(VTTY_HANDLE, 0, sizeof(uv_tty_t));
  this->setHandleData();
  return 0;
}

int VTty::init(VLoop* loop, uv_file fd, int readable) {
  int ret = uv_tty_init(OBJ_VLOOP_HANDLE(*loop), VTTY_HANDLE, fd, readable);
  this->setHandleData();
  return ret;
}

int VTty::setMode(uv_tty_mode_t mode) {
  return uv_tty_set_mode(VTTY_HANDLE, mode);
}

int VTty::resetMode(void) { return uv_tty_reset_mode(); }

int VTty::getWinsize(int* width, int* height) {
  return uv_tty_get_winsize(VTTY_HANDLE, width, height);
}

void VTty::setVtermState(uv_tty_vtermstate_t state) {
  uv_tty_set_vterm_state(state);
  return;
}

int VTty::getVtermState(uv_tty_vtermstate_t* state) {
  return uv_tty_get_vterm_state(state);
}
