if COURIER_IMAP
CIPROGS = authvmailmgr
else
CIPROGS =
endif
EXTRA_DIST = authvmailmgr.cc

bin_PROGRAMS = $(CIPROGS) checkvpw vauthenticate

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../include

checkvpw_SOURCES = checkvpw.cc authvlib.cc authvlib.h
checkvpw_LDADD = ../lib/libvmailmgr.a

authvmailmgr_SOURCES = authvmailmgr.cc authvlib.cc authvlib.h
authvmailmgr_LDADD = ../lib/libvmailmgr.a

vauthenticate_SOURCES = vauthenticate.cc authvlib.cc authvlib.h
vauthenticate_LDADD = ../lib/libvmailmgr.a

