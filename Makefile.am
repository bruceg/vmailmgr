if WANTPYTHON
SUBDIRS = python php lib authenticate commands daemon cgi doc scripts
else
SUBDIRS = php lib authenticate commands daemon cgi doc scripts
endif

dist-hook:
	cp -dP `find lib/ac -name CVS -prune -o \( -not -name '*~' -a -type f -o -type l \) -print` $(distdir)
	sed -e s/@VERSION\@/@VERSION@/ <spec >$(distdir)/vmailmgr-@VERSION@.spec

docs:
	$(MAKE) -C doc all
