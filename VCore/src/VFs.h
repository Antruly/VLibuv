#pragma once
#include "VBuf.h"
#include "VDir.h"
#include "VLoop.h"
#include "VReq.h"


class VFs : public VReq {
 public:
  DEFINE_INHERIT_FUNC(VFs);
  DEFINE_COPY_FUNC_DELETE(VFs);

  int init();
  uv_fs_type getType();
  ssize_t getResult();
  int getSystemError();
  void* getPtr();
  const char* getPath();
  uv_stat_t* getStatbuf();

  void reqCleanup();

  int close(VLoop* loop, uv_file file);

  int open(VLoop* loop, const char* path, int flags, int mode);

  int read(VLoop* loop,
           uv_file file,
           const VBuf bufs[],
           unsigned int nbufs,
           int64_t offset);

  int unlink(VLoop* loop, const char* path);

  int write(VLoop* loop,
            uv_file file,
            const VBuf bufs[],
            unsigned int nbufs,
            int64_t offset);

  int copyfile(VLoop* loop, const char* path, const char* new_path, int flags);

  int mkdir(VLoop* loop, const char* path, int mode);

  int mkdtemp(VLoop* loop, const char* tpl);

  int mkstemp(VLoop* loop, const char* tpl);

  int rmdir(VLoop* loop, const char* path);

  int scandir(VLoop* loop, const char* path, int flags);

  int opendir(VLoop* loop, const char* path);

  int readdir(VLoop* loop, VDir* dir);

  int closedir(VLoop* loop, VDir* dir);

  int stat(VLoop* loop, const char* path);

  int fstat(VLoop* loop, uv_file file);

  int rename(VLoop* loop, const char* path, const char* new_path);

  int fsync(VLoop* loop, uv_file file);

  int fdatasync(VLoop* loop, uv_file file);

  int ftruncate(VLoop* loop, uv_file file, int64_t offset);

  int sendfile(VLoop* loop,
               uv_file out_fd,
               uv_file in_fd,
               int64_t in_offset,
               size_t length);

  int access(VLoop* loop, const char* path, int mode);

  int chmod(VLoop* loop, const char* path, int mode);

  int utime(VLoop* loop, const char* path, double atime, double mtime);

  int futime(VLoop* loop, uv_file file, double atime, double mtime);

  int lutime(VLoop* loop, const char* path, double atime, double mtime);

  int lstat(VLoop* loop, const char* path);

  int link(VLoop* loop, const char* path, const char* new_path);

  int symlink(VLoop* loop, const char* path, const char* new_path, int flags);

  int readlink(VLoop* loop, const char* path);
  int realpath(VLoop* loop, const char* path);
  int fchmod(VLoop* loop, uv_file file, int mode);
  int chown(VLoop* loop, const char* path, uv_uid_t uid, uv_gid_t gid);
  int fchown(VLoop* loop, uv_file file, uv_uid_t uid, uv_gid_t gid);
  int lchown(VLoop* loop, const char* path, uv_uid_t uid, uv_gid_t gid);
  int statfs(VLoop* loop, const char* path);

  int close(VLoop* loop, uv_file file, std::function<void(VFs*)> close_cb);

  int open(VLoop* loop,
           const char* path,
           int flags,
           int mode,
           std::function<void(VFs*)> open_cb);

  int read(VLoop* loop,
           uv_file file,
           const VBuf bufs[],
           unsigned int nbufs,
           int64_t offset,
           std::function<void(VFs*)> read_cb);

  int unlink(VLoop* loop,
             const char* path,
             std::function<void(VFs*)> unlink_cb);

  int write(VLoop* loop,
            uv_file file,
            const VBuf bufs[],
            unsigned int nbufs,
            int64_t offset,
            std::function<void(VFs*)> write_cb);

  int copyfile(VLoop* loop,
               const char* path,
               const char* new_path,
               int flags,
               std::function<void(VFs*)> copyfile_cb);

  int mkdir(VLoop* loop,
            const char* path,
            int mode,
            std::function<void(VFs*)> mkdir_cb);

  int mkdtemp(VLoop* loop,
              const char* tpl,
              std::function<void(VFs*)> mkdtemp_cb);

  int mkstemp(VLoop* loop,
              const char* tpl,
              std::function<void(VFs*)> mkstemp_cb);

  int rmdir(VLoop* loop, const char* path, std::function<void(VFs*)> rmdir_cb);

  int scandir(VLoop* loop,
              const char* path,
              int flags,
              std::function<void(VFs*)> scandir_cb);

  int opendir(VLoop* loop,
              const char* path,
              std::function<void(VFs*)> opendir_cb);

  int readdir(VLoop* loop, VDir* dir, std::function<void(VFs*)> readdir_cb);

  int closedir(VLoop* loop, VDir* dir, std::function<void(VFs*)> closedir_cb);

  int stat(VLoop* loop, const char* path, std::function<void(VFs*)> stat_cb);

  int fstat(VLoop* loop, uv_file file, std::function<void(VFs*)> fstat_cb);

  int rename(VLoop* loop,
             const char* path,
             const char* new_path,
             std::function<void(VFs*)> rename_cb);

  int fsync(VLoop* loop, uv_file file, std::function<void(VFs*)> fsync_cb);

  int fdatasync(VLoop* loop,
                uv_file file,
                std::function<void(VFs*)> fdatasync_cb);

  int ftruncate(VLoop* loop,
                uv_file file,
                int64_t offset,
                std::function<void(VFs*)> ftruncate_cb);

  int sendfile(VLoop* loop,
               uv_file out_fd,
               uv_file in_fd,
               int64_t in_offset,
               size_t length,
               std::function<void(VFs*)> sendfile_cb);

  int access(VLoop* loop,
             const char* path,
             int mode,
             std::function<void(VFs*)> access_cb);

  int chmod(VLoop* loop,
            const char* path,
            int mode,
            std::function<void(VFs*)> chmod_cb);

  int utime(VLoop* loop,
            const char* path,
            double atime,
            double mtime,
            std::function<void(VFs*)> utime_cb);

  int futime(VLoop* loop,
             uv_file file,
             double atime,
             double mtime,
             std::function<void(VFs*)> futime_cb);

  int lutime(VLoop* loop,
             const char* path,
             double atime,
             double mtime,
             std::function<void(VFs*)> lutime_cb);

  int lstat(VLoop* loop, const char* path, std::function<void(VFs*)> lstat_cb);

  int link(VLoop* loop,
           const char* path,
           const char* new_path,
           std::function<void(VFs*)> link_cb);

  int symlink(VLoop* loop,
              const char* path,
              const char* new_path,
              int flags,
              std::function<void(VFs*)> symlink_cb);

  int readlink(VLoop* loop,
               const char* path,
               std::function<void(VFs*)> readlink_cb);

  int realpath(VLoop* loop,
               const char* path,
               std::function<void(VFs*)> realpath_cb);

  int fchmod(VLoop* loop,
             uv_file file,
             int mode,
             std::function<void(VFs*)> fchmod_cb);

  int chown(VLoop* loop,
            const char* path,
            uv_uid_t uid,
            uv_gid_t gid,
            std::function<void(VFs*)> chown_cb);

  int fchown(VLoop* loop,
             uv_file file,
             uv_uid_t uid,
             uv_gid_t gid,
             std::function<void(VFs*)> fchown_cb);

  int lchown(VLoop* loop,
             const char* path,
             uv_uid_t uid,
             uv_gid_t gid,
             std::function<void(VFs*)> lchown_cb);

  int statfs(VLoop* loop,
             const char* path,
             std::function<void(VFs*)> statfs_cb);
  int scandirNext(uv_dirent_t* ent);
 protected:
  std::function<void(VFs*)> fs_close_cb;
  std::function<void(VFs*)> fs_open_cb;
  std::function<void(VFs*)> fs_read_cb;
  std::function<void(VFs*)> fs_unlink_cb;
  std::function<void(VFs*)> fs_write_cb;
  std::function<void(VFs*)> fs_copyfile_cb;
  std::function<void(VFs*)> fs_mkdir_cb;
  std::function<void(VFs*)> fs_mkdtemp_cb;
  std::function<void(VFs*)> fs_mkstemp_cb;
  std::function<void(VFs*)> fs_rmdir_cb;
  std::function<void(VFs*)> fs_scandir_cb;
  std::function<void(VFs*)> fs_opendir_cb;
  std::function<void(VFs*)> fs_readdir_cb;
  std::function<void(VFs*)> fs_closedir_cb;
  std::function<void(VFs*)> fs_stat_cb;
  std::function<void(VFs*)> fs_fstat_cb;
  std::function<void(VFs*)> fs_rename_cb;
  std::function<void(VFs*)> fs_fsync_cb;
  std::function<void(VFs*)> fs_fdatasync_cb;
  std::function<void(VFs*)> fs_ftruncate_cb;
  std::function<void(VFs*)> fs_sendfile_cb;
  std::function<void(VFs*)> fs_access_cb;
  std::function<void(VFs*)> fs_chmod_cb;
  std::function<void(VFs*)> fs_utime_cb;
  std::function<void(VFs*)> fs_futime_cb;
  std::function<void(VFs*)> fs_lutime_cb;
  std::function<void(VFs*)> fs_lstat_cb;
  std::function<void(VFs*)> fs_link_cb;
  std::function<void(VFs*)> fs_symlink_cb;
  std::function<void(VFs*)> fs_readlink_cb;
  std::function<void(VFs*)> fs_realpath_cb;
  std::function<void(VFs*)> fs_fchmod_cb;
  std::function<void(VFs*)> fs_chown_cb;
  std::function<void(VFs*)> fs_fchown_cb;
  std::function<void(VFs*)> fs_lchown_cb;
  std::function<void(VFs*)> fs_statfs_cb;

 private:
  static void callback_close(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_close_cb)
      reinterpret_cast<VFs*>(req->data)->fs_close_cb(
          reinterpret_cast<VFs*>(req->data));
  }
  static void callback_open(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_open_cb)
      reinterpret_cast<VFs*>(req->data)->fs_open_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_read(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_read_cb)
      reinterpret_cast<VFs*>(req->data)->fs_read_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_unlink(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_unlink_cb)
      reinterpret_cast<VFs*>(req->data)->fs_unlink_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_write(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_write_cb)
      reinterpret_cast<VFs*>(req->data)->fs_write_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_copyfile(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_copyfile_cb)
      reinterpret_cast<VFs*>(req->data)->fs_copyfile_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_mkdir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_mkdir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_mkdir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_mkdtemp(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_mkdtemp_cb)
      reinterpret_cast<VFs*>(req->data)->fs_mkdtemp_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_mkstemp(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_mkstemp_cb)
      reinterpret_cast<VFs*>(req->data)->fs_mkstemp_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_rmdir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_rmdir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_rmdir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_scandir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_scandir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_scandir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_opendir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_opendir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_opendir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_readdir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_readdir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_readdir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_closedir(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_closedir_cb)
      reinterpret_cast<VFs*>(req->data)->fs_closedir_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_stat(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_stat_cb)
      reinterpret_cast<VFs*>(req->data)->fs_stat_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_fstat(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_fstat_cb)
      reinterpret_cast<VFs*>(req->data)->fs_fstat_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_rename(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_rename_cb)
      reinterpret_cast<VFs*>(req->data)->fs_rename_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_fsync(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_fsync_cb)
      reinterpret_cast<VFs*>(req->data)->fs_fsync_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_fdatasync(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_fdatasync_cb)
      reinterpret_cast<VFs*>(req->data)->fs_fdatasync_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_ftruncate(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_ftruncate_cb)
      reinterpret_cast<VFs*>(req->data)->fs_ftruncate_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_sendfile(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_sendfile_cb)
      reinterpret_cast<VFs*>(req->data)->fs_sendfile_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_access(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_access_cb)
      reinterpret_cast<VFs*>(req->data)->fs_access_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_chmod(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_chmod_cb)
      reinterpret_cast<VFs*>(req->data)->fs_chmod_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_utime(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_utime_cb)
      reinterpret_cast<VFs*>(req->data)->fs_utime_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_futime(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_futime_cb)
      reinterpret_cast<VFs*>(req->data)->fs_futime_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_lutime(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_lutime_cb)
      reinterpret_cast<VFs*>(req->data)->fs_lutime_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_lstat(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_lstat_cb)
      reinterpret_cast<VFs*>(req->data)->fs_lstat_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_link(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_link_cb)
      reinterpret_cast<VFs*>(req->data)->fs_link_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_symlink(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_symlink_cb)
      reinterpret_cast<VFs*>(req->data)->fs_symlink_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_readlink(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_readlink_cb)
      reinterpret_cast<VFs*>(req->data)->fs_readlink_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_realpath(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_realpath_cb)
      reinterpret_cast<VFs*>(req->data)->fs_realpath_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_fchmod(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_fchmod_cb)
      reinterpret_cast<VFs*>(req->data)->fs_fchmod_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_chown(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_chown_cb)
      reinterpret_cast<VFs*>(req->data)->fs_chown_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_fchown(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_fchown_cb)
      reinterpret_cast<VFs*>(req->data)->fs_fchown_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_lchown(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_lchown_cb)
      reinterpret_cast<VFs*>(req->data)->fs_lchown_cb(
          reinterpret_cast<VFs*>(req->data));
  }

  static void callback_statfs(uv_fs_t* req) {
    if (reinterpret_cast<VFs*>(req->data)->fs_statfs_cb)
      reinterpret_cast<VFs*>(req->data)->fs_statfs_cb(
          reinterpret_cast<VFs*>(req->data));
  }

 private:
};
