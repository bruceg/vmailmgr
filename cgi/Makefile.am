EXTRA_DIST = listvdomain.html vaddalias.html vadduser.html vchalias.html \
	vchattr.html vchforwards.html vdeluser.html vpasswd.html
cgi_PROGRAMS = vpasswd vadduser vdeluser vaddalias vchattr vchforwards \
	listvdomain

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../lib

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

vchforwards_SOURCES = vchforwards.cc
vchforwards_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a

listvdomain_SOURCES = listvdomain.cc
listvdomain_LDADD = ../lib/libcgi.a ../lib/libvmailmgr.a
