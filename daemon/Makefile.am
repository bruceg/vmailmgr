sbin_PROGRAMS = vmailmgrd
EXTRA_DIST = init run-log run-svc

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../lib -I../lib/config -I../lib/fdbuf -I../lib/misc -I../lib/mystring -I../lib/vpw

vmailmgrd_SOURCES = daemon.h \
	addalias.cc \
	addaliasp.cc \
	adduser.cc \
	chattr.cc \
	check.cc \
	command.h command.cc \
	deluser.cc \
	dispatch.cc \
	fork.cc \
	listdomain.cc \
	log.h log.cc \
	lookup.cc \
	main.cc \
	state.h state.cc
vmailmgrd_LDADD = ../lib/libvmailmgr.a

