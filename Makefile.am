SUBDIRS = python lib authenticate commands daemon cgi doc

dist-hook:
	cp -dP `find php3 -not -name '*~' -a -type f -o -type l` $(distdir)
	sed -e s/@VERSION\@/@VERSION@/ <spec >$(distdir)/vmailmgr-@VERSION@.spec

docs:
	$(MAKE) -C doc all
