sbin_PROGRAMS = vmailmgrd

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../lib

vmailmgrd_SOURCES = daemon.h \
	adduser2.cc \
	autoresponse.cc \
	chattr.cc \
	check.cc \
	command.h command.cc \
	deluser.cc \
	dispatch.cc \
	listdomain.cc \
	log.h log.cc \
	lookup.cc \
	main.cc \
	state.h state.cc
vmailmgrd_LDADD = ../lib/libvmailmgr.a ../lib/cli/libcli.a

