if COURIER_IMAP
CIPROGS = authvmailmgr
CILIBS = -L@COURIER_IMAP_DIR@/authlib -lauthmod -lauth
CIINCLUDES = -I@COURIER_IMAP_DIR@/authlib
else
CIPROGS =
endif
EXTRA_DIST = authvmailmgr.cc

bin_PROGRAMS = $(CIPROGS) checkvpw vauthenticate

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = $(CIINCLUDES) -I../lib -I../lib/config -I../lib/fdbuf -I../lib/misc -I../lib/mystring -I../lib/vpw

checkvpw_SOURCES = checkvpw.cc authvlib.cc authvlib.h
checkvpw_LDADD = ../lib/libvmailmgr.a

authvmailmgr_SOURCES = authvmailmgr.cc authvlib.cc authvlib.h
authvmailmgr_LDADD = ../lib/libvmailmgr.a $(CILIBS)

vauthenticate_SOURCES = vauthenticate.cc authvlib.cc authvlib.h
vauthenticate_LDADD = ../lib/libvmailmgr.a

