#ifndef VMAILMGRD__DAEMON__LOG__H__
#define VMAILMGRD__DAEMON__LOG__H__

#include "fdbuf/fdbuf.h"
#include "command.h"
#include "misc/response.h"

extern fdobuf& log();
extern void log(const char*);
extern void logcommand(const command&);
extern void logresponse(const response&);

extern const mystring LOG_PASSWORD;
extern const mystring LOG_NEWPASS;
extern const mystring LOG_ADMINPASS;
extern const mystring LOG_MESSAGE;

#endif // VMAILMGRD__DAEMON__LOG__H__
