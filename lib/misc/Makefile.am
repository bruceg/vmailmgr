noinst_LIBRARIES = libmisc.a
INCLUDES = -I..
EXTRA_DIST = krbauth.c

if KRB5
KRB5_SRCS = krbauth.c
else
KRB5_SRCS =
endif

libmisc_a_SOURCES = $(KRB5_SRCS) \
	autodelete.h \
	crc32.h crc32.cc crc_hash.h \
	debug.h \
	exec.h exec.cc \
	itoa.h itoa.cc \
	keystrlist.cc \
	list_table.h list_table.cc \
	lookup.h \
	lookup_baseuser.cc \
	lookup_domain.cc \
	maildir.h maildir.cc \
	map_table.h map_table.cc \
	md5.h md5.c \
	md5-crypt.c \
	passwdfn.h passwdfn.cc \
	pwcrypt.h pwcrypt.cc \
	pwentry.h pwentry.cc \
	pwentry_auth.cc \
	pwentry_table.h pwentry_table.cc \
	response.h response_write.cc response_message.cc response_read.cc \
	server.h server.cc \
	stat_fns.h \
	strlist.h strlist.cc \
	strlist_read.cc \
	strlist_write.cc \
	un.h \
	utoa.h utoa.cc
