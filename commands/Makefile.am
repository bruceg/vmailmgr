if VPWTABLE_CDB
CDBPROGS = vpasswd2cdb vrehash
else
CDBPROGS =
endif

bin_PROGRAMS = listvdomain vadduser vaddusers vchattr vchforwards \
	vcheckquota vdeliver vdeluser vpasswd vpasswd2db vpasswds \
	vpopbull vsetup $(CDBPROGS)
bin_SCRIPTS = vaddalias vconf2dir
noinst_HEADERS = vcommand.h
EXTRA_DIST = vaddalias vconf2dir \
	vpasswd2cdb.cc vrehash.cc

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
