bin_PROGRAMS = listvdomain vadduser vaddusers vchattr vchforwards \
	vcheckquota vdeliver vdeluser vpasswd vpasswds vpopbull \
	vrehash vsetup vupgrade
bin_SCRIPTS = vaddalias vconf2dir
noinst_HEADERS = vcommand.h

noinst_PODS = \
	listvdomain.pod vadduser.pod vadduser.pod vchattr.pod \
	vchforwards.pod vcheckquota.pod vdeliver.pod vdeluser.pod \
	vpasswd.pod vpasswds.pod vpopbull.pod vrehash.pod vsetup.pod \
	vupgrade.pod
noinst_HTMLS = $(noinst_PODS:.pod=.html)
man_MANS = $(noinst_PODS:.pod=.1)

CLEANFILES = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) \
	pod2html-dircache pod2html-itemcache
EXTRA_DIST = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) $(bin_SCRIPTS)

EXTRA_SCRIPTS = vupgrade-all

all: docs

docs: $(noinst_HTMLS) $(man_MANS)

dist-hook:
	touch $(distdir)/*.html $(distdir)/*.[123456789]

%.pod: %.cc ../configure.in ../lib/cli++/cli++topod.pl
	perl ../lib/cli++/cli++topod.pl $< >$@
%.1: %.pod ../configure.in; $(POD2MAN) --section=1 $< >$@
%.html: %.pod; $(POD2HTML) $< >$@

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../lib -DBINDIR=\"@bindir@\"

listvdomain_SOURCES = listvdomain.cc vcommand.cc
listvdomain_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vadduser_SOURCES = vadduser.cc vcommand.cc
vadduser_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vaddusers_SOURCES = vaddusers.cc vcommand.cc
vaddusers_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vchattr_SOURCES = vchattr.cc vcommand.cc
vchattr_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vcheckquota_SOURCES = vcheckquota.cc
vcheckquota_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vchforwards_SOURCES = vchforwards.cc vcommand.cc
vchforwards_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vdeliver_SOURCES = vdeliver.cc vcommand.cc
vdeliver_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vdeluser_SOURCES = vdeluser.cc vcommand.cc
vdeluser_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vpasswd_SOURCES = vpasswd.cc vcommand.cc
vpasswd_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vpasswds_SOURCES = vpasswds.cc vcommand.cc
vpasswds_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vupgrade_SOURCES = vupgrade.cc vcommand.cc
vupgrade_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vpopbull_SOURCES = vpopbull.cc vcommand.cc
vpopbull_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vsetup_SOURCES = vsetup.cc vcommand.cc
vsetup_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vrehash_SOURCES = vrehash.cc vcommand.cc
vrehash_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a
