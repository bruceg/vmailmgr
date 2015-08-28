sbin_PROGRAMS = vmailmgrd

noinst_PODS = vmailmgrd.pod
noinst_HTMLS = $(noinst_PODS:.pod=.html)
man_MANS = $(noinst_PODS:.pod=.8)

CLEANFILES = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) \
	pod2html-dircache pod2html-itemcache
EXTRA_DIST = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS)

all: docs

docs: $(noinst_HTMLS) $(noinst_TXTS)

dist-hook:
	touch $(distdir)/*.html $(distdir)/*.[123456789]

vmailmgrd.pod: main.cc ../configure.ac ../lib/cli++/cli++topod.pl
	perl ../lib/cli++/cli++topod.pl $< >$@
.pod.8: ../configure.ac
	$(POD2MAN) --section=8 $< >$@
.pod.html: ../configure.ac
	$(POD2HTML) $< >$@

AM_CPPFLAGS = -I../lib

vmailmgrd_SOURCES = daemon.h \
	adduser.cc \
	autoresponse.cc \
	chattr.cc \
	check.cc \
	command.h command.cc \
	deluser.cc \
	dispatch.cc \
	listdomain.cc \
	log.h log.cc \
	lookup.cc \
	main.cc \
	stat.cc \
	state.h state.cc
vmailmgrd_LDADD = ../lib/libvmailmgr.a ../lib/cli++/libcli++.a
