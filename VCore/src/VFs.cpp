#include "VFs.h"

VFs::VFs() : VReq(this) {
  uv_fs_t* fs = (uv_fs_t*)VCore::malloc(sizeof(uv_fs_t));
  this->setReq(fs);
  this->init();
}
VFs::VFs(VFs* t_p) : VReq(t_p) {}
VFs::~VFs() {}

int VFs::init() {
  memset(VFS_REQ, 0, sizeof(uv_fs_t));
  this->setReqData();
  return 0;
}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 19
uv_fs_type VFs::getType() { return uv_fs_get_type(VFS_REQ); }

ssize_t VFs::getResult() { return uv_fs_get_result(VFS_REQ); }

void* VFs::getPtr() { return uv_fs_get_ptr(VFS_REQ); }

const char* VFs::getPath() { return uv_fs_get_path(VFS_REQ); }

uv_stat_t* VFs::getStatbuf() { return uv_fs_get_statbuf(VFS_REQ); }
#endif
#endif


#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 38
int VFs::getSystemError() { return uv_fs_get_system_error(VFS_REQ); }
#endif
#endif


void VFs::reqCleanup() { uv_fs_req_cleanup(VFS_REQ); }

int VFs::close(VLoop* loop, uv_file file) {
  return uv_fs_close(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, nullptr);
}

int VFs::open(VLoop* loop, const char* path, int flags, int mode) {
  return uv_fs_open(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, flags, mode,
                    nullptr);
}

int VFs::read(VLoop* loop, uv_file file, const VBuf bufs[], unsigned int nbufs,
              int64_t offset) {
  return uv_fs_read(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file,
                    (const uv_buf_t*)bufs, nbufs, offset, nullptr);
}

int VFs::unlink(VLoop* loop, const char* path) {
  return uv_fs_unlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}

int VFs::write(VLoop* loop, uv_file file, const VBuf bufs[], unsigned int nbufs,
               int64_t offset) {
  return uv_fs_write(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file,
                     (const uv_buf_t*)bufs, nbufs, offset, nullptr);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 14
int VFs::copyfile(VLoop* loop, const char* path, const char* new_path,
                  int flags) {
  return uv_fs_copyfile(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path, flags,
                        nullptr);
}
#endif
#endif


int VFs::mkdir(VLoop* loop, const char* path, int mode) {
  return uv_fs_mkdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode, nullptr);
}

int VFs::mkdtemp(VLoop* loop, const char* tpl) {
  return uv_fs_mkdtemp(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, tpl, nullptr);
}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 34
int VFs::mkstemp(VLoop* loop, const char* tpl) {
  return uv_fs_mkstemp(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, tpl, nullptr);
}
#endif
#endif


int VFs::rmdir(VLoop* loop, const char* path) {
  return uv_fs_rmdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}

int VFs::scandir(VLoop* loop, const char* path, int flags) {
  return uv_fs_scandir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, flags, nullptr);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 28
int VFs::opendir(VLoop* loop, const char* path) {
  return uv_fs_opendir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}

int VFs::readdir(VLoop* loop, VDir* dir) {
  return uv_fs_readdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ,
                       (uv_dir_t*)dir->getDir(), nullptr);
}

int VFs::closedir(VLoop* loop, VDir* dir) {
  return uv_fs_closedir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ,
                        (uv_dir_t*)dir->getDir(), nullptr);
}
#endif
#endif


int VFs::stat(VLoop* loop, const char* path) {
  return uv_fs_stat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}

int VFs::fstat(VLoop* loop, uv_file file) {
  return uv_fs_fstat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, nullptr);
}

int VFs::rename(VLoop* loop, const char* path, const char* new_path) {
  return uv_fs_rename(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path,
                      nullptr);
}

int VFs::fsync(VLoop* loop, uv_file file) {
  return uv_fs_fsync(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, nullptr);
}

int VFs::fdatasync(VLoop* loop, uv_file file) {
  return uv_fs_fdatasync(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, nullptr);
}

int VFs::ftruncate(VLoop* loop, uv_file file, int64_t offset) {
  return uv_fs_ftruncate(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, offset,
                         nullptr);
}

int VFs::sendfile(VLoop* loop, uv_file out_fd, uv_file in_fd, int64_t in_offset,
                  size_t length) {
  return uv_fs_sendfile(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, out_fd, in_fd,
                        in_offset, length, nullptr);
}

int VFs::access(VLoop* loop, const char* path, int mode) {
  return uv_fs_access(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode, nullptr);
}

int VFs::chmod(VLoop* loop, const char* path, int mode) {
  return uv_fs_chmod(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode, nullptr);
}

int VFs::utime(VLoop* loop, const char* path, double atime, double mtime) {
  return uv_fs_utime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, atime, mtime,
                     nullptr);
}

int VFs::futime(VLoop* loop, uv_file file, double atime, double mtime) {
  return uv_fs_futime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, atime, mtime,
                      nullptr);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 36
int VFs::lutime(VLoop* loop, const char* path, double atime, double mtime) {
  return uv_fs_lutime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, atime, mtime,
                      nullptr);
}
#endif
#endif


int VFs::lstat(VLoop* loop, const char* path) {
  return uv_fs_lstat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}

int VFs::link(VLoop* loop, const char* path, const char* new_path) {
  return uv_fs_link(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path, nullptr);
}

int VFs::symlink(VLoop* loop, const char* path, const char* new_path,
                 int flags) {
  return uv_fs_symlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path, flags,
                       nullptr);
}

int VFs::readlink(VLoop* loop, const char* path) {
  return uv_fs_readlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 8
int VFs::realpath(VLoop* loop, const char* path) {
  return uv_fs_realpath(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}
#endif
#endif


int VFs::fchmod(VLoop* loop, uv_file file, int mode) {
  return uv_fs_fchmod(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, mode, nullptr);
}

int VFs::chown(VLoop* loop, const char* path, uv_uid_t uid, uv_gid_t gid) {
  return uv_fs_chown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, uid, gid, nullptr);
}

int VFs::fchown(VLoop* loop, uv_file file, uv_uid_t uid, uv_gid_t gid) {
  return uv_fs_fchown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, uid, gid,
                      nullptr);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 21
int VFs::lchown(VLoop* loop, const char* path, uv_uid_t uid, uv_gid_t gid) {
  return uv_fs_lchown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, uid, gid,
                      nullptr);
}
#endif
#endif

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 30
int VFs::statfs(VLoop* loop, const char* path) {
  return uv_fs_statfs(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, nullptr);
}
#endif
#endif


int VFs::close(VLoop* loop,
                      uv_file file,
                      std::function<void(VFs*)> close_cb) {
  fs_close_cb = close_cb;
  return uv_fs_close(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, callback_close);
}

int VFs::open(VLoop* loop,
                     const char* path,
                     int flags,
                     int mode,
                     std::function<void(VFs*)> open_cb) {
  fs_open_cb = open_cb;
  return uv_fs_open(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, flags, mode,
                    callback_open);
}

int VFs::read(VLoop* loop,
                     uv_file file,
                     const VBuf bufs[],
                     unsigned int nbufs,
                     int64_t offset,
                     std::function<void(VFs*)> read_cb) {
  fs_read_cb = read_cb;
  return uv_fs_read(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file,
                    (const uv_buf_t*)bufs, nbufs, offset, callback_read);
}

int VFs::unlink(VLoop* loop,
                       const char* path,
                       std::function<void(VFs*)> unlink_cb) {
  fs_unlink_cb = unlink_cb;
  return uv_fs_unlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, callback_unlink);
}

int VFs::write(VLoop* loop,
                      uv_file file,
                      const VBuf bufs[],
                      unsigned int nbufs,
                      int64_t offset,
                      std::function<void(VFs*)> write_cb) {
  fs_write_cb = write_cb;
  return uv_fs_write(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file,
                     (const uv_buf_t*)bufs, nbufs, offset, callback_write);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 14
int VFs::copyfile(VLoop* loop,
                         const char* path,
                         const char* new_path,
                         int flags,
                         std::function<void(VFs*)> copyfile_cb) {
  fs_copyfile_cb = copyfile_cb;
  return uv_fs_copyfile(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path, flags,
                        callback_copyfile);
}
#endif
#endif


int VFs::mkdir(VLoop* loop,
                      const char* path,
                      int mode,
                      std::function<void(VFs*)> mkdir_cb) {
  fs_mkdir_cb = mkdir_cb;
  return uv_fs_mkdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode,
                     callback_mkdir);
}

int VFs::mkdtemp(VLoop* loop,
                        const char* tpl,
                        std::function<void(VFs*)> mkdtemp_cb) {
  fs_mkdtemp_cb = mkdtemp_cb;
  return uv_fs_mkdtemp(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, tpl, callback_mkdtemp);
}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 34
int VFs::mkstemp(VLoop* loop,
                        const char* tpl,
                        std::function<void(VFs*)> mkstemp_cb) {
  fs_mkstemp_cb = mkstemp_cb;
  return uv_fs_mkstemp(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, tpl, callback_mkstemp);
}
#endif
#endif


int VFs::rmdir(VLoop* loop,
                      const char* path,
                      std::function<void(VFs*)> rmdir_cb) {
  fs_rmdir_cb = rmdir_cb;
  return uv_fs_rmdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, callback_rmdir);
}

int VFs::scandir(VLoop* loop,
                        const char* path,
                        int flags,
                        std::function<void(VFs*)> scandir_cb) {
  fs_scandir_cb = scandir_cb;
  return uv_fs_scandir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, flags,
                       callback_scandir);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 28
int VFs::opendir(VLoop* loop,
                        const char* path,
                        std::function<void(VFs*)> opendir_cb) {
  fs_opendir_cb = opendir_cb;
  return uv_fs_opendir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path,
                       callback_opendir);
}

int VFs::readdir(VLoop* loop,
                        VDir* dir,
                        std::function<void(VFs*)> readdir_cb) {
  fs_readdir_cb = readdir_cb;
  return uv_fs_readdir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ,
                       (uv_dir_t*)dir->getDir(), callback_readdir);
}

int VFs::closedir(VLoop* loop,
                         VDir* dir,
                         std::function<void(VFs*)> closedir_cb) {
  fs_closedir_cb = closedir_cb;
  return uv_fs_closedir(OBJ_VLOOP_HANDLE(*loop), VFS_REQ,
                        (uv_dir_t*)dir->getDir(), callback_closedir);
}
#endif
#endif


int VFs::stat(VLoop* loop,
                     const char* path,
                     std::function<void(VFs*)> stat_cb) {
  fs_stat_cb = stat_cb;
  return uv_fs_stat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, callback_stat);
}

int VFs::fstat(VLoop* loop,
                      uv_file file,
                      std::function<void(VFs*)> fstat_cb) {
  fs_fstat_cb = fstat_cb;
  return uv_fs_fstat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, callback_fstat);
}

int VFs::rename(VLoop* loop,
                       const char* path,
                       const char* new_path,
                       std::function<void(VFs*)> rename_cb) {
  fs_rename_cb = rename_cb;
  return uv_fs_rename(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path,
                      callback_rename);
}

int VFs::fsync(VLoop* loop,
                      uv_file file,
                      std::function<void(VFs*)> fsync_cb) {
  fs_fsync_cb = fsync_cb;
  return uv_fs_fsync(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, callback_fsync);
}

int VFs::fdatasync(VLoop* loop,
                          uv_file file,
                          std::function<void(VFs*)> fdatasync_cb) {
  fs_fdatasync_cb = fdatasync_cb;
  return uv_fs_fdatasync(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file,
                         callback_fdatasync);
}

int VFs::ftruncate(VLoop* loop,
                          uv_file file,
                          int64_t offset,
                          std::function<void(VFs*)> ftruncate_cb) {
  fs_ftruncate_cb = ftruncate_cb;
  return uv_fs_ftruncate(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, offset,
                         callback_ftruncate);
}

int VFs::sendfile(VLoop* loop,
                         uv_file out_fd,
                         uv_file in_fd,
                         int64_t in_offset,
                         size_t length,
                         std::function<void(VFs*)> sendfile_cb) {
  fs_sendfile_cb = sendfile_cb;
  return uv_fs_sendfile(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, out_fd, in_fd,
                        in_offset, length, callback_sendfile);
}

int VFs::access(VLoop* loop,
                       const char* path,
                       int mode,
                       std::function<void(VFs*)> access_cb) {
  fs_access_cb = access_cb;
  return uv_fs_access(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode,
                      callback_access);
}

int VFs::chmod(VLoop* loop,
                      const char* path,
                      int mode,
                      std::function<void(VFs*)> chmod_cb) {
  fs_chmod_cb = chmod_cb;
  return uv_fs_chmod(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, mode,
                     callback_chmod);
}

int VFs::utime(VLoop* loop,
                      const char* path,
                      double atime,
                      double mtime,
                      std::function<void(VFs*)> utime_cb) {
  fs_utime_cb = utime_cb;
  return uv_fs_utime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, atime, mtime,
                     callback_utime);
}

int VFs::futime(VLoop* loop,
                       uv_file file,
                       double atime,
                       double mtime,
                       std::function<void(VFs*)> futime_cb) {
  fs_futime_cb = futime_cb;
  return uv_fs_futime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, atime, mtime,
                      callback_futime);
}

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 36
int VFs::lutime(VLoop* loop,
                       const char* path,
                       double atime,
                       double mtime,
                       std::function<void(VFs*)> lutime_cb) {
  fs_lutime_cb = lutime_cb;
  return uv_fs_lutime(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, atime, mtime,
                      callback_lutime);
}
#endif
#endif


int VFs::lstat(VLoop* loop,
                      const char* path,
                      std::function<void(VFs*)> lstat_cb) {
  fs_lstat_cb = lstat_cb;
  return uv_fs_lstat(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, callback_lstat);
}

int VFs::link(VLoop* loop,
                     const char* path,
                     const char* new_path,
                     std::function<void(VFs*)> link_cb) {
  fs_link_cb = link_cb;
  return uv_fs_link(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path,
                    callback_link);
}

int VFs::symlink(VLoop* loop,
                        const char* path,
                        const char* new_path,
                        int flags,
                        std::function<void(VFs*)> symlink_cb) {
  fs_symlink_cb = symlink_cb;
  return uv_fs_symlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, new_path, flags,
                       callback_symlink);
}

int VFs::readlink(VLoop* loop,
                         const char* path,
                         std::function<void(VFs*)> readlink_cb) {
  fs_readlink_cb = readlink_cb;
  return uv_fs_readlink(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path,
                        callback_readlink);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 8
int VFs::realpath(VLoop* loop,
                         const char* path,
                         std::function<void(VFs*)> realpath_cb) {
  fs_realpath_cb = realpath_cb;
  return uv_fs_realpath(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path,
                        callback_realpath);
}
#endif
#endif


int VFs::fchmod(VLoop* loop,
                       uv_file file,
                       int mode,
                       std::function<void(VFs*)> fchmod_cb) {
  fs_fchmod_cb = fchmod_cb;
  return uv_fs_fchmod(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, mode,
                      callback_fchmod);
}

int VFs::chown(VLoop* loop,
                      const char* path,
                      uv_uid_t uid,
                      uv_gid_t gid,
                      std::function<void(VFs*)> chown_cb) {
  fs_chown_cb = chown_cb;
  return uv_fs_chown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, uid, gid,
                     callback_chown);
}

int VFs::fchown(VLoop* loop,
                       uv_file file,
                       uv_uid_t uid,
                       uv_gid_t gid,
                       std::function<void(VFs*)> fchown_cb) {
  fs_fchown_cb = fchown_cb;
  return uv_fs_fchown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, file, uid, gid,
                      callback_fchown);
}
#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 21
int VFs::lchown(VLoop* loop,
                       const char* path,
                       uv_uid_t uid,
                       uv_gid_t gid,
                       std::function<void(VFs*)> lchown_cb) {
  fs_lchown_cb = lchown_cb;
  return uv_fs_lchown(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, uid, gid,
                      callback_lchown);
}
#endif
#endif

#if UV_VERSION_MAJOR >= 1
#if UV_VERSION_MINOR >= 30
int VFs::statfs(VLoop* loop,
                       const char* path,
                       std::function<void(VFs*)> statfs_cb) {
  fs_statfs_cb = statfs_cb;
  return uv_fs_statfs(OBJ_VLOOP_HANDLE(*loop), VFS_REQ, path, callback_statfs);
}
#endif
#endif


int VFs::scandirNext(uv_dirent_t* ent) {
  return uv_fs_scandir_next(VFS_REQ, ent);
}


