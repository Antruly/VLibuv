#pragma once
#include "VBuf.h"
#include "VString.h"


class VFile {
public:
  // 读取文件数据
  static bool readFileData(const VString &filePath, VBuf &fileData);

  // 写入数据到文件
  static bool writeFileData(const VString &filePath, const VBuf &fileData);

  // 删除文件
  static bool removeFile(const VString &filePath);

  // 删除空目录
  static bool removeDirectory(const VString &dirPath);

  // 递归删除目录及其内容
  static bool removeDirectoryRecursive(const VString &dirPath);

  // 递归创建目录
  static bool createDirectory(const VString &path);

  // 提取目录路径
  static VString extractDirectory(const VString &file_path);

  // 创建文件目录
  static bool createFileDirectory(const VString &output_file);

};
