#ifndef VMAILMGR__STAT_FNS__H__
#define VMAILMGR__STAT_FNS__H__

#include <sys/stat.h>
#include <unistd.h>

inline bool is_exist(const char* filename)
{
  struct stat buf;
  return !stat(filename, &buf);
}

inline bool is_dir(const char* filename)
{
  struct stat buf;
  return !stat(filename, &buf) && S_ISDIR(buf.st_mode);
}

inline bool is_symlink(const char* filename)
{
  struct stat buf;
  return !lstat(filename, &buf) && S_ISLNK(buf.st_mode);
}

inline bool is_file(const char* filename)
{
  struct stat buf;
  return !stat(filename, &buf) && S_ISREG(buf.st_mode);
}

#endif
