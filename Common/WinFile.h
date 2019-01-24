#pragma once

#include <windows.h>
#include <string>

class WinFile
{
public:
  HANDLE hFile;
  DWORD dwAccess;
  DWORD dwFilepointer;
  DWORD flags;
  std::string sFilename;
  std::string lineBuffer;
  BYTE* MemoryFile;
  WinFile();
  ~WinFile();
  // access   : GENERIC_READ or GENERIC_WRITE
  // creation : CREATE_NEW, CREATE_ALWAYS, OPEN_EXISTING,
  //            OPEN_ALWAYS and TRUNCATE_EXISTING
  // random   : True for random access files
  bool open(const std::string& filename, DWORD access, bool append, DWORD creation, bool random);
  bool isOpen(){return (hFile!=NULL);};
  bool reopen();
  void close();
  void flush();
  void deleteFile();
  void useMemoryFile();
  DWORD size();
  DWORD read(void* buffer, DWORD length, DWORD filepointer);
  DWORD read(void* buffer, DWORD length){return read(buffer,length,dwFilepointer);};
  bool write(const void* buffer, DWORD length, DWORD filepointer);
  bool write(const void* buffer, DWORD length){return write(buffer,length,dwFilepointer);};
  bool setFilepointer(DWORD filepointer);
  // For textreading
  bool readLine(std::string& line);
  bool writeLine(const char* str){return writeLine(std::string(str));};
  bool writeLine(const std::string& line);
  void putBack(std::string& line);
  bool isMemoryFile(){return (MemoryFile?true:false);};
  bool exist(const std::string& filename);
  const std::string toString();
};

