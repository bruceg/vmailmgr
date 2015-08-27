noinst_LIBRARIES = libconfig.a
bin_PROGRAMS = showvconfig
EXTRA_DIST = configrc.in build-config.sh \
	configrc.h.in Makefile.am.in configvar.in showvconfig.cc.in

AM_CPPFLAGS = -I..

libconfig_a_SOURCES = \
	_autoresponse_dir.cc \
	_autoresponse_file.cc \
	_bulletin_dir.cc \
	_default_expiry.cc \
	_default_maildir.cc \
	_default_msgcount.cc \
	_default_msgsize.cc \
	_default_hardquota.cc \
	_default_softquota.cc \
	_default_username.cc \
	_error_maildir.cc \
	_global_bulletin_dir.cc \
	_maildir_arg_str.cc \
	_password_file.cc \
	_postmaster_aliases.cc \
	_postmaster_email.cc \
	_qmail_root.cc \
	_separators.cc \
	_socket_file.cc \
	_user_dir.cc \
	_user_dir_bits.cc \
	_user_dir_slices.cc \
	configio.h config_read.cc \
	configrc.h configrc.cc \
	read_dir.cc read_list.cc read_str.cc read_uns.cc

showvconfig_SOURCES = showvconfig.cc
showvconfig_LDADD = libconfig.a ../fdbuf/libfdbuf.a ../misc/libmisc.a \
	../mystring/libmystring.a ../vdomain/libvdomain.a \
	../vpwtable/libvpwtable.a
