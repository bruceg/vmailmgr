noinst_LIBRARIES = libvpwtable.a
AM_CPPFLAGS = -I..
EXTRA_DIST = vpwtable.h \
	cdb_vpwtable.cc \
	cdb_del.cc \
	cdb_exists.cc \
	cdb_getbyname.cc \
	cdb_put.cc \
	cdb_read.cc \
	cdb_write.cc \
	gdbm_vpwtable.cc \
	gdbm_del.cc \
	gdbm_exists.cc \
	gdbm_getbyname.cc \
	gdbm_put.cc \
	gdbm_read.cc \
	gdbm_write.cc

if VPWTABLE_CDB
CDBSOURCES = \
	cdb_vpwtable.cc \
	cdb_del.cc \
	cdb_exists.cc \
	cdb_getbyname.cc \
	cdb_put.cc \
	cdb_read.cc \
	cdb_write.cc
else
CDBSOURCES =
endif

if VPWTABLE_GDBM
GDBMSOURCES = \
	gdbm_vpwtable.cc \
	gdbm_del.cc \
	gdbm_exists.cc \
	gdbm_getbyname.cc \
	gdbm_put.cc \
	gdbm_read.cc \
	gdbm_write.cc
else
GDBMSOURCES =
endif

libvpwtable_a_SOURCES = $(CDBSOURCES) $(GDBMSOURCES) \
	reader.cc writer.cc
