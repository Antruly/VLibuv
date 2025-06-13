#pragma once
#include "VBuf.h"
#include "VString.h"


class VFile {
public:
  // ��ȡ�ļ�����
  static bool readFileData(const VString &filePath, VBuf &fileData);

  // д�����ݵ��ļ�
  static bool writeFileData(const VString &filePath, const VBuf &fileData);

  // ɾ���ļ�
  static bool removeFile(const VString &filePath);

  // ɾ����Ŀ¼
  static bool removeDirectory(const VString &dirPath);

  // �ݹ�ɾ��Ŀ¼��������
  static bool removeDirectoryRecursive(const VString &dirPath);

  // �ݹ鴴��Ŀ¼
  static bool createDirectory(const VString &path);

  // ��ȡĿ¼·��
  static VString extractDirectory(const VString &file_path);

  // �����ļ�Ŀ¼
  static bool createFileDirectory(const VString &output_file);

};
