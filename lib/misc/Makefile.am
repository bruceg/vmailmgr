noinst_LIBRARIES = libmisc.a
INCLUDES = -I..
EXTRA_DIST = autodelete.h debug.h hash_table.h krbauth.c stat_fns.h

if KRB5
KRB5_SRCS = krbauth.c
else
KRB5_SRCS =
endif

libmisc_a_SOURCES = $(KRB5_SRCS) \
	crc32.h crc32.cc crc_hash.h \
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
	pwentry_export_env.cc \
	pwentry_table.h pwentry_table.cc \
	response.h response_write.cc response_message.cc response_read.cc \
	server.h server.cc \
	server_call0.cc \
	server_call1.cc \
	server_call2.cc \
	server_call3.cc \
	server_call4.cc \
	server_call5.cc \
	server_call6.cc \
	strlist.h strlist.cc \
	strlist_read.cc \
	strlist_write.cc \
	strtou.h strtou.c \
	un.h \
	utoa.h utoa.cc
