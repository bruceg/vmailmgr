noinst_LIBRARIES = libvpwtable.a
INCLUDES = -I..
EXTRA_DIST = vpwtable.h \
	cdb_vpwtable.cc \
	cdb_del.cc \
	cdb_exists.cc \
	cdb_get.cc \
	cdb_getbyname.cc \
	cdb_put.cc \
	gdbm_vpwtable.cc \
	gdbm_del.cc \
	gdbm_exists.cc \
	gdbm_get.cc \
	gdbm_getbyname.cc \
	gdbm_put.cc

if VPWTABLE_GDBM
libvpwtable_a_SOURCES = \
	gdbm_vpwtable.cc \
	gdbm_del.cc \
	gdbm_exists.cc \
	gdbm_get.cc \
	gdbm_getbyname.cc \
	gdbm_put.cc
endif
if VPWTABLE_CDB
libvpwtable_a_SOURCES = \
	cdb_vpwtable.cc \
	cdb_del.cc \
	cdb_exists.cc \
	cdb_get.cc \
	cdb_getbyname.cc \
	cdb_put.cc
endif
