bin_PROGRAMS = authvmailmgr checkvpw vauthenticate

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = $(CIINCLUDES) -I../lib

checkvpw_SOURCES = checkvpw.cc authvlib.cc authvlib.h
checkvpw_LDADD = ../lib/libvmailmgr.a

authvmailmgr_SOURCES = authvmailmgr.cc authvlib.cc authvlib.h
authvmailmgr_LDADD = ../lib/libvmailmgr.a -L../lib/courier-authlib -lauthmod

vauthenticate_SOURCES = vauthenticate.cc authvlib.cc authvlib.h
vauthenticate_LDADD = ../lib/libvmailmgr.a

