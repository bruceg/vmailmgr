#ifndef VMAILMGR__CONFIG_RC__H__
#define VMAILMGR__CONFIG_RC__H__

#ifndef GLOBAL_CONFIG_DIR
#define GLOBAL_CONFIG_DIR "/etc/vmailmgr"
#endif

#ifndef LOCAL_CONFIG_DIR
#define LOCAL_CONFIG_DIR ".vmailmgr"
#endif

#include "mystring/mystring.h"
#include "misc/strlist.h"

template<class T>
struct config_cache
{
  T* value;
  config_cache() : value(0) { }
  ~config_cache() { delete value; }
};

class configuration 
{
public:
  configuration();
  configuration(const configuration* prev, const mystring& dir);
  ~configuration();

  configuration const * parent;

  const mystring directory;

public: mystring bulletin_dir() const;
private: config_cache<mystring> bulletin_dir_cache;
public: unsigned default_expiry() const;
private: config_cache<unsigned> default_expiry_cache;
public: mystring default_maildir() const;
private: config_cache<mystring> default_maildir_cache;
public: unsigned default_msgcount() const;
private: config_cache<unsigned> default_msgcount_cache;
public: unsigned default_msgsize() const;
private: config_cache<unsigned> default_msgsize_cache;
public: unsigned default_hardquota() const;
private: config_cache<unsigned> default_hardquota_cache;
public: unsigned default_softquota() const;
private: config_cache<unsigned> default_softquota_cache;
public: mystring default_username() const;
private: config_cache<mystring> default_username_cache;
public: mystring error_maildir() const;
private: config_cache<mystring> error_maildir_cache;
public: mystring global_bulletin_dir() const;
private: config_cache<mystring> global_bulletin_dir_cache;
public: mystring maildir_arg_str() const;
private: config_cache<mystring> maildir_arg_str_cache;
public: unsigned max_fork() const;
private: config_cache<unsigned> max_fork_cache;
public: mystring password_file() const;
private: config_cache<mystring> password_file_cache;
public: strlist postmaster_aliases() const;
private: config_cache<strlist> postmaster_aliases_cache;
public: mystring postmaster_email() const;
private: config_cache<mystring> postmaster_email_cache;
public: mystring qmail_root() const;
private: config_cache<mystring> qmail_root_cache;
public: mystring separators() const;
private: config_cache<mystring> separators_cache;
public: mystring socket_file() const;
private: config_cache<mystring> socket_file_cache;
public: mystring user_dir() const;
private: config_cache<mystring> user_dir_cache;
public: unsigned user_dir_bits() const;
private: config_cache<unsigned> user_dir_bits_cache;
public: unsigned user_dir_slices() const;
private: config_cache<unsigned> user_dir_slices_cache;

private:
  mystring read_str(const mystring& name, const mystring& def,
                    config_cache<mystring>&) const;
  mystring read_dir(const mystring& name, const mystring& def,
                    config_cache<mystring>&) const;
  unsigned read_uns(const mystring& name, unsigned def,
                    config_cache<unsigned>&) const;
  strlist read_list(const mystring& name, const strlist& def,
                    config_cache<strlist>&) const;
};

extern const configuration* config;

#endif
