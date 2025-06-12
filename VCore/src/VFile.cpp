#include "VFile.h"
#include "VFs.h"
#include "VLogger.h"
#include "VLoop.h"
// ��ȡ�ļ�����

 bool VFile::readFileData(const VString &filePath, VBuf &fileData) {
  VLoop loop;
  VFs fs;
  uv_file fd = fs.open(&loop, filePath.c_str(), O_RDONLY, 0);
  if (fd < 0) {
    Log->logDebugError("Failed to open file: %s\n", uv_strerror(fd));
    return false;
  }

  // ��ȡ�ļ���С
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

// д�����ݵ��ļ�

 bool VFile::writeFileData(const VString &filePath,
                                 const VBuf &fileData) {
  VLoop loop;
  VFs fs;
  VFs fs_write;

  // ��дģʽ���ļ�������ļ��������򴴽���
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

    // ��ȡ��ǰҪд��Ļ�������Ƭ
    VBuf chunk(fileData.getConstData() + written, writeSize);

    // д�����ݵ�һ����
    fs_write.write(&loop, fd, &chunk, chunk.size(), written);

    if (fs.getResult() < 0) {
      Log->logDebugError("Failed to write to file: %s\n",
                         uv_strerror(fs.getResult()));
      fs.close(&loop, fd);
      return false;
    }

    // ������д����ֽ���
    written += writeSize;
  }

  // �ر��ļ�
  fs.close(&loop, fd);
  return true;
}

// ɾ���ļ�

 bool VFile::removeFile(const VString &filePath) {
  VLoop loop;
  VFs fs;

  // ɾ���ļ�
  int result = fs.unlink(&loop, filePath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to remove file: %s\n", uv_strerror(result));
    return false;
  }
  return true;
}

// ɾ����Ŀ¼

 bool VFile::removeDirectory(const VString &dirPath) {
  VLoop loop;
  VFs fs;

  // ɾ����Ŀ¼
  int result = fs.rmdir(&loop, dirPath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to remove directory: %s\n", uv_strerror(result));
    return false;
  }
  return true;
}

// �ݹ�ɾ��Ŀ¼��������

 bool VFile::removeDirectoryRecursive(const VString &dirPath) {
  VLoop loop;
  VFs fs;

  // �г�Ŀ¼����
  VDir entry;
  int result = fs.opendir(&loop, dirPath.c_str());
  if (result < 0) {
    Log->logDebugError("Failed to open directory: %s\n", uv_strerror(result));
    return false;
  }

  // ����Ŀ¼����
  while (fs.readdir(&loop, &entry) == 0) {
    if (entry.getDir()->dirents->name == "." ||
        entry.getDir()->dirents->name == "..")
      continue;

    VString entryPath = VString::Format("%s/%s", dirPath.c_str(),
                                        entry.getDir()->dirents->name);
    if (entry.getDir()->dirents->type == UV_DIRENT_DIR) {
      // �ݹ�ɾ����Ŀ¼
      if (!removeDirectoryRecursive(entryPath)) {
        fs.closedir(&loop, &entry);
        return false;
      }
    } else if (entry.getDir()->dirents->type == UV_DIRENT_FILE) {
      // ɾ���ļ�
      if (!removeFile(entryPath)) {
        fs.closedir(&loop, &entry);
        return false;
      }
    }
  }

  // ɾ����ǰĿ¼
  fs.closedir(&loop, &entry);
  return removeDirectory(dirPath);
 }

 // �ݹ鴴��Ŀ¼

 bool VFile::createDirectory(const VString &path) {
   if (path.empty())
     return true;
   VLoop loop;
   VFs req;
   int ret;

   // ���·���Ƿ����
   ret = req.access(&loop, path.c_str(), F_OK);
   req.reqCleanup();

   if (ret == 0) {
     // ·���Ѵ���
     return true;
   }

   // �Ҹ�Ŀ¼
   size_t pos = path.stdString().find_last_of("/\\");
   if (pos == std::string::npos) {
     // û�и�Ŀ¼��ֱ�Ӵ�����ǰĿ¼
     ret = req.mkdir(&loop, path.c_str(), 0755);
     req.reqCleanup();
     return ret == 0;
   }

   // �ݹ鴴����Ŀ¼
   std::string parent = path.substr(0, pos);
   if (parent.empty()) {
     // ��Ŀ¼��Ӧ���Ѿ�����
     return true;
   }

   if (!createDirectory(parent)) {
     return false;
   }

   // ������ǰĿ¼
   ret = req.mkdir(&loop, path.c_str(), 0755);
   req.reqCleanup();

   if (ret != 0) {
     // ����EEXIST���󣨿��������߳�/�����Ѿ�������
     ret = req.access(&loop, path.c_str(), F_OK);
     req.reqCleanup();
     return ret == 0;
   }

   return true;
 }

 // ��ȡĿ¼·��

 VString VFile::extractDirectory(const VString &file_path) {
   size_t pos = file_path.stdString().find_last_of("/\\");
   if (pos == std::string::npos) {
     return "."; // ��ǰĿ¼
   }
   return file_path.substr(0, pos);
 }

 // �����ļ�Ŀ¼

 bool VFile::createFileDirectory(const VString &output_file) {
   if (output_file.empty()) {
     Log->logError("Error: Output file path is empty");
     return false;
   }

   // ��ȡ�ļ�����Ŀ¼
   std::string dir_path = extractDirectory(output_file);
   if (dir_path.empty()) {
     dir_path = ".";
   }

   // �ݹ鴴��Ŀ¼
   if (!createDirectory(dir_path)) {
     Log->logError("Error: Failed to create directory: %s", dir_path.c_str());
     return false;
   }

   return true;
 }
