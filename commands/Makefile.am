if VPWTABLE_CDB
CDBPROGS = vpasswd2cdb vrehash
CDBSCRIPTS = allvpasswd2cdb
CDBMANS = vpasswd2cdb.1
CDBPODS = vpasswd2cdb.pod
CDBHTMLS = vpasswd2cdb.html
else
CDBPROGS =
CDBSCRIPTS =
CDBMANS =
CDBPODS =
CDBHTMLS =
endif

bin_PROGRAMS = $(CDBPROGS) \
	listvdomain vadduser vaddusers vchattr vchforwards vcheckquota \
	vdeliver vdeluser vpasswd vpasswd2db vpasswds vpopbull vsetup
bin_SCRIPTS = $(CDBSCRIPTS) vaddalias vconf2dir
noinst_HEADERS = vcommand.h

man_MANS = $(noinst_PODS:.pod=.1)
noinst_HTMLS = $(noinst_PODS:.pod=.html)
noinst_PODS = $(bin_PROGRAMS:=.pod)

CLEANFILES = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) \
	pod2html-dircache pod2html-itemcache
EXTRA_DIST = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) $(bin_SCRIPTS) \
	vpasswd2cdb.cc vrehash.cc

all: docs

docs: $(noinst_HTMLS) $(noinst_TXTS)

dist-hook:
	touch $(distdir)/*.html
	touch $(distdir)/*.txt
	touch $(distdir)/*.[123456789]

%.pod: %.cc ../configure.in ../lib/cli/cli2pod.pl
	perl ../lib/cli/cli2pod.pl $< >$@
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

vpasswd2cdb_SOURCES = vpasswd2cdb.cc vcommand.cc
vpasswd2cdb_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vpasswd2db_SOURCES = vpasswd2db.cc vcommand.cc
vpasswd2db_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vpopbull_SOURCES = vpopbull.cc vcommand.cc
vpopbull_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vrehash_SOURCES = vrehash.cc vcommand.cc
vrehash_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a

vsetup_SOURCES = vsetup.cc vcommand.cc
vsetup_LDADD = ../lib/libcli.a ../lib/libvmailmgr.a
