#include "VFile.h"
#include "VFs.h"
#include "VLogger.h"
#include "VLoop.h"
// 读取文件数据

 bool VFile::readFileData(const VString &filePath, VBuf &fileData) {
  VLoop loop;
  VFs fs;
  uv_file fd = fs.open(&loop, filePath.c_str(), O_RDONLY, 0);
  if (fd < 0) {
    Log->logDebugError("Failed to open file: %s\n", uv_strerror(fd));
    return false;
  }

  // 获取文件大小
  fs.fstat(&loop, fd);
  if (fs.getResult() < 0) {
    Log->logDebugError("Failed to get file stats: %s\n",
                       uv_strerror(fs.getResult()));
    fs.close(&loop, fd);
    return false;
  }
  size_t file_size = fs.getStatbuf()->st_size;
  fileData.resize(file_size);

  fs.read(&loop, fd, &fileData, 1, 0);

  if (fs.getResult() < 0) {
    Log->logDebugError("Failed to read file: %s\n",
                       uv_strerror(fs.getResult()));
    fs.close(&loop, fd);
    return false;
  }
  fs.close(&loop, fd);
  return true;
}

// 写入数据到文件

 bool VFile::writeFileData(const VString &filePath,
                                 const VBuf &fileData) {
  VLoop loop;
  VFs fs;
  VFs fs_write;

  // 以写模式打开文件（如果文件不存在则创建）
  uv_file fd =
      fs.open(&loop, filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    Log->logDebugError("Failed to open file for writing: %s\n",
                       uv_strerror(fd));
    return false;
  }

  size_t written = 0;
  size_t bufferSize = 1024; // 1mb
  while (written < fileData.size()) {
    size_t remainingSize = fileData.size() - written;
    size_t writeSize = remainingSize > bufferSize ? bufferSize : remainingSize;

    // 获取当前要写入的缓冲区切片
    VBuf chunk(fileData.getConstData() + written, writeSize);

    // 写入数据的一部分
    fs_write.write(&loop, fd, &chunk, chunk.size(), written);

    if (fs.getResult() < 0) {
      Log->logDebugError("Failed to write to file: %s\n",
                         uv_strerror(fs.getResult()));
      fs.close(&loop, fd);
      return false;
    }

    // 更新已写入的字节数
    written += writeSize;
  }

  // 关闭文件
  fs.close(&loop, fd);
  return true;
}

// 删除文件

 bool VFile::removeFile(const VString &filePath) {
  VLoop loop;
  VFs fs;

  // 删除文件
  int result = fs.unlink(&loop, filePath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to remove file: %s\n", uv_strerror(result));
    return false;
  }
  return true;
}

// 删除空目录

 bool VFile::removeDirectory(const VString &dirPath) {
  VLoop loop;
  VFs fs;

  // 删除空目录
  int result = fs.rmdir(&loop, dirPath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to remove directory: %s\n", uv_strerror(result));
    return false;
  }
  return true;
}

// 递归删除目录及其内容

 bool VFile::removeDirectoryRecursive(const VString &dirPath) {
  VLoop loop;
  VFs fs;

  // 列出目录内容
  VDir entry;
  int result = fs.opendir(&loop, dirPath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to open directory: %s\n", uv_strerror(result));
    return false;
  }

  // 遍历目录内容
  while (fs.readdir(&loop, &entry) == 0) {
    if (entry.getDir()->dirents->name == "." ||
        entry.getDir()->dirents->name == "..")
      continue;

    VString entryPath = VString::Format("%s/%s", dirPath.c_str(),
                                        entry.getDir()->dirents->name);
    if (entry.getDir()->dirents->type == UV_DIRENT_DIR) {
      // 递归删除子目录
      if (!removeDirectoryRecursive(entryPath)) {
        fs.closedir(&loop, &entry);
        return false;
      }
    } else if (entry.getDir()->dirents->type == UV_DIRENT_FILE) {
      // 删除文件
      if (!removeFile(entryPath)) {
        fs.closedir(&loop, &entry);
        return false;
      }
    }
  }

  // 删除当前目录
  fs.closedir(&loop, &entry);
  return removeDirectory(dirPath);
 }

 // 递归创建目录

 bool VFile::createDirectory(const VString &path) {
   if (path.empty())
     return true;
   VLoop loop;
   VFs req;
   int ret;

   // 检查路径是否存在
   ret = req.access(&loop, path.c_str(), F_OK);
   req.reqCleanup();

   if (ret == 0) {
     // 路径已存在
     return true;
   }

   // 找父目录
   size_t pos = path.stdString().find_last_of("/\\");
   if (pos == std::string::npos) {
     // 没有父目录，直接创建当前目录
     ret = req.mkdir(&loop, path.c_str(), 0755);
     req.reqCleanup();
     return ret == 0;
   }

   // 递归创建父目录
   std::string parent = path.substr(0, pos);
   if (parent.empty()) {
     // 根目录，应该已经存在
     return true;
   }

   if (!createDirectory(parent)) {
     return false;
   }

   // 创建当前目录
   ret = req.mkdir(&loop, path.c_str(), 0755);
   req.reqCleanup();

   if (ret != 0) {
     // 处理EEXIST错误（可能其他线程/进程已经创建）
     ret = req.access(&loop, path.c_str(), F_OK);
     req.reqCleanup();
     return ret == 0;
   }

   return true;
 }

 // 提取目录路径

 VString VFile::extractDirectory(const VString &file_path) {
   size_t pos = file_path.stdString().find_last_of("/\\");
   if (pos == std::string::npos) {
     return "."; // 当前目录
   }
   return file_path.substr(0, pos);
 }

 // 创建文件目录

 bool VFile::createFileDirectory(const VString &output_file) {
   if (output_file.empty()) {
     Log->logError("Error: Output file path is empty");
     return false;
   }

   // 提取文件所在目录
   std::string dir_path = extractDirectory(output_file);
   if (dir_path.empty()) {
     dir_path = ".";
   }

   // 递归创建目录
   if (!createDirectory(dir_path)) {
     Log->logError("Error: Failed to create directory: %s", dir_path.c_str());
     return false;
   }

   return true;
 }
