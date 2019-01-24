#include "WinFile.h"
#include "Utility.h"

using namespace std;

WinFile::WinFile()
{
  hFile=NULL;
  dwAccess=0;
  dwFilepointer=0;
  flags=FILE_ATTRIBUTE_NORMAL;
  MemoryFile=NULL;
}

WinFile::~WinFile()
{
  close();
}

void WinFile::flush()
{
  if (!MemoryFile)
    return;
}

bool WinFile::open(const std::string& filename, DWORD access, bool append, DWORD creation, bool random)
{
  if (hFile)
    close();
  sFilename=trim(filename);
  if (sFilename.length()==0)
    return false;
  dwAccess=access;
  if (dwAccess==GENERIC_WRITE)
    dwAccess|=GENERIC_READ;

  if (random)
    flags|=FILE_FLAG_RANDOM_ACCESS;
  else
    flags|=FILE_FLAG_SEQUENTIAL_SCAN;
  hFile=CreateFileA(sFilename.c_str(),dwAccess,FILE_SHARE_READ,NULL,creation,flags,NULL);
  if (hFile==INVALID_HANDLE_VALUE)
  {
    hFile=NULL;
    return false;
  }
  if (append)
    dwFilepointer=SetFilePointer(hFile,0,NULL,FILE_END);
  else
    dwFilepointer=0;
  return true;
}

bool WinFile::reopen()
{
  if (hFile)
    close();
  if (!sFilename.length())
    return false;
  hFile=CreateFileA(sFilename.c_str(),dwAccess,FILE_SHARE_READ,NULL,OPEN_EXISTING,flags,NULL);
  if (hFile==INVALID_HANDLE_VALUE)
  {
    hFile=NULL;
    return false;
  }
  dwFilepointer=0;
  return true;
}

void WinFile::close()
{
  lineBuffer="";
  if (hFile)
    CloseHandle(hFile);
  hFile=NULL;
  dwFilepointer=0;
}

void WinFile::deleteFile()
{
  if (hFile)
    close();
  if (sFilename.length())
    DeleteFileA(sFilename.c_str());
}

DWORD WinFile::size()
{
  DWORD dwSize=0;
  if (hFile)
  {
    dwSize=GetFileSize(hFile,NULL);
  }else
  {
    if (reopen())
    {
      dwSize=GetFileSize(hFile,NULL);
      close();
    }
  }
  if (dwSize==0xFFFFFFFF)
    dwSize=0;
  return dwSize;
}

DWORD WinFile::read(void* buffer, DWORD length, DWORD filepointer)
{
  DWORD dwRead;
  if (!hFile)
    return false;
  if (!setFilepointer(filepointer))
    return 0;
  if (!ReadFile(hFile,buffer,length,&dwRead,NULL))
    return 0;
  if (!dwRead)
    return 0;
  dwFilepointer+=dwRead;
  return dwRead;
}

bool WinFile::readLine(std::string& line)
{
  DWORD dwRead;
  char c;
  if (lineBuffer.length())
  {
    line=lineBuffer;
    lineBuffer="";
    return true;
  }
  line="";
  while (1)
  {
    if (!ReadFile(hFile,&c,1,&dwRead,NULL))
    {
      if (line.length())
        return true;
      return false;
    }
    if (dwRead==0)
    {
      if (line.length())
        return true;
      return false;
    }
    dwFilepointer++;
    if (c!='\r')
    {
      if (c=='\n')
        return true;
      line+=c;
    }
  }
}

void WinFile::putBack(std::string& line)
{
  lineBuffer=line;
}

bool WinFile::write(const void* buffer, DWORD length, DWORD filepointer)
{
  DWORD dwWrite;
  if (!hFile)
    return false;
  if (!setFilepointer(filepointer))
    return false;
  if (!WriteFile(hFile,buffer,length,&dwWrite,NULL))
    return false;
  dwFilepointer+=dwWrite;
  return true;
}

bool WinFile::setFilepointer(DWORD filepointer)
{
  if (!hFile)
    return false;
  lineBuffer="";
  if (dwFilepointer==filepointer)
    return true;
  dwFilepointer=SetFilePointer(hFile,filepointer,NULL,FILE_BEGIN);
  if (dwFilepointer==0xFFFFFFFF)
  {
    dwFilepointer=0;
    return false;
  }
  return true;
}

bool WinFile::writeLine(const std::string& line)
{
  string s=line;
  int i;
  if (s.length()<1)
    s="\n";
  // Replace all \r\n strings so we only have \n (new line).
  i=s.find("\r\n");
  while (i!=string::npos)
  {
    s.replace(i,2,"\n");
    i=s.find("\r\n",i+1);
  }
  // Now replace back again so we get it to Windows format.
  i=s.find("\n");
  while (i!=string::npos)
  {
    s.replace(i,1,"\r\n");
    i=s.find("\n",i+2);
  }
  // Add a newline at the end if it not exist.
  if (s.at(s.length()-1)!='\n')
    s+="\r\n";
  return write(s.c_str(),s.length(),dwFilepointer);
}

bool WinFile::exist(const std::string& filename)
{
  HANDLE hFind;
  WIN32_FIND_DATAA wfd;
  if (filename.length()>MAX_PATH)
    return false;
  hFind=FindFirstFileA(filename.c_str(),&wfd);
  if (hFind==INVALID_HANDLE_VALUE)
    return false;
  FindClose(hFind);
  return true;
}

const std::string WinFile::toString()
{
  char sz[256];
  DWORD dw;
  string s;
  setFilepointer(0);
  while ((dw=read(sz,255))>0)
  {
    sz[dw]='\0';
    s+=sz;
  }
  return s;
}