#ifndef VMAILMGR__UN__H__
#define VMAILMGR__UN__H__

#include <sys/socket.h>
#include <sys/un.h>

#ifndef SUN_LEN
/* Evaluate to actual length of the `sockaddr_un' structure.  */
#define SUN_LEN(ptr) ((size_t) (((struct sockaddr_un *) 0)->sun_path)	      \
		      + strlen ((ptr)->sun_path))
#endif

#endif
