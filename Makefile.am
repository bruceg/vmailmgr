if WANTPYTHON
SUBDIRS = python php lib authenticate commands daemon cgi doc scripts
else
SUBDIRS = php lib authenticate commands daemon cgi doc scripts
endif

dist-hook:
	cp -dP --parents `find lib/ac -name CVS -prune -o \( -not -name '*~' -a -type f -o -type l \) -print` $(distdir)
	sed -e s/@VERSION\@/@PACKAGE_VERSION@/ <spec >$(distdir)/vmailmgr-@PACKAGE_VERSION@.spec

docs:
	$(MAKE) -C doc all
	$(MAKE) -C daemon docs
	$(MAKE) -C commands docs
