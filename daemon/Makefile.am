sbin_PROGRAMS = vmailmgrd

noinst_PODS = $(sbin_PROGRAMS:=.pod)
noinst_HTMLS = $(noinst_PODS:.pod=.html)
man_MANS = $(noinst_PODS:.pod=.8)

CLEANFILES = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS) \
	pod2html-dircache pod2html-itemcache
EXTRA_DIST = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS)

all: docs

docs: $(noinst_HTMLS) $(noinst_TXTS)

dist-hook:
	touch $(distdir)/*.html
	touch $(distdir)/*.txt
	touch $(distdir)/*.[123456789]

vmailmgrd.pod: main.cc ../configure.in ../lib/cli/cli2pod.pl
	perl ../lib/cli/cli2pod.pl $< >$@
%.8: %.pod ../configure.in; $(POD2MAN) --section=8 $< >$@
%.html: %.pod; $(POD2HTML) $< >$@

CXXLINK = $(CC) $(CXXFLAGS) $(LDFLAGS) -o $@
INCLUDES = -I../lib

vmailmgrd_SOURCES = daemon.h \
	adduser2.cc \
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
	state.h state.cc
vmailmgrd_LDADD = ../lib/libvmailmgr.a ../lib/cli/libcli.a
