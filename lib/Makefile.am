SUBDIRS = misc vdomain vpwentry vpwtable cdb++ cgi cli fdbuf mystring config \
	courier-authlib
noinst_LIBRARIES = libvmailmgr.a libcgi.a libcli.a
EXTRA_DIST = mergelib.sh

libvmailmgr_a_SOURCES=
libvmailmgr.a: mergelib.sh cdb++/libcdb++.a misc/libmisc.a \
	config/libconfig.a vpwentry/libvpwentry.a vpwtable/libvpwtable.a \
	vdomain/libvdomain.a fdbuf/libfdbuf.a mystring/libmystring.a
	$(RM) -f $@
	sh mergelib.sh $@ cdb++/libcdb++.a misc/libmisc.a \
	config/libconfig.a vpwentry/libvpwentry.a vpwtable/libvpwtable.a \
	vdomain/libvdomain.a fdbuf/libfdbuf.a mystring/libmystring.a
	
libcli_a_SOURCES =
libcli.a: cli/libcli.a mergelib.sh
	$(RM) -f $@
	sh mergelib.sh $@ cli/libcli.a

libcgi_a_SOURCES =
libcgi.a: cgi/libcgi.a mergelib.sh
	$(RM) -f $@
	sh mergelib.sh $@ cgi/libcgi.a
