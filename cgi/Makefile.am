cgi_PROGRAMS = vpasswd vadduser vdeluser vaddalias vchattr listvdomain

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../include

vpasswd_SOURCES = vpasswd.cc
vpasswd_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

vadduser_SOURCES = vadduser.cc
vadduser_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

vdeluser_SOURCES = vdeluser.cc
vdeluser_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

vaddalias_SOURCES = vaddalias.cc
vaddalias_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

vchattr_SOURCES = vchattr.cc
vchattr_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

listvdomain_SOURCES = listvdomain.cc
listvdomain_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

