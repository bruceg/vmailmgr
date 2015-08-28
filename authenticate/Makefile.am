bin_PROGRAMS = checkvpw vauthenticate
authlib_PROGRAMS = authvmailmgr
authlibdir = ${prefix}/lib/courier-imap/libexec/authlib/

noinst_PODS = checkvpw.pod vauthenticate.pod
noinst_HTMLS = $(noinst_PODS:.pod=.html)
man_MANS = $(noinst_PODS:.pod=.1)

CLEANFILES = $(man_MANS) $(noinst_HTMLS) \
	pod2html-dircache pod2html-itemcache
EXTRA_DIST = $(man_MANS) $(noinst_HTMLS) $(noinst_PODS)

all: docs

docs: $(noinst_HTMLS) $(man_MANS)

dist-hook:
	touch $(distdir)/*.html $(distdir)/*.[123456789]

.pod.1: ../configure.ac
	$(POD2MAN) --section=1 $< >$@
.pod.html: ../configure.ac
	$(POD2HTML) $< >$@

AM_CPPFLAGS = -I../lib

checkvpw_SOURCES = checkvpw.cc authvlib.cc authvlib.h
checkvpw_LDADD = ../lib/libvmailmgr.a

authvmailmgr_SOURCES = authvmailmgr.cc authvlib.cc authvlib.h
authvmailmgr_LDADD = ../lib/libvmailmgr.a -L../lib/courier-authlib -lauthmod

vauthenticate_SOURCES = vauthenticate.cc authvlib.cc authvlib.h
vauthenticate_LDADD = ../lib/libvmailmgr.a
