man_MANS = vdeliver.1 vmailmgrd.8 checkvpw.1 vmailmgr.7 vpasswd.5 \
	vpasswd.1 vadduser.1 vdeluser.1 vauthenticate.1 vchattr.1 \
	vaddusers.1 vpasswds.1 vsetup.1 vrehash.1 vpopbull.1
noinst_SGMLS = FAQ.sgml HOWTO.sgml
noinst_TEXINFOS = configuration.texi vmailmgr-cgi.texi
noinst_HTMLS = vdeliver.html vmailmgrd.html checkvpw.html vmailmgr.html \
	vpasswd5.html vpasswd.html vadduser.html vdeluser.html \
	vaddusers.html vpasswds.html vauthenticate.html vchattr.html \
	vsetup.html vrehash.html vpopbull.html \
	HOWTO.html FAQ.html NEWS.html \
	configuration.html ChangeLog.html vmailmgr-cgi.html
noinst_TXTS = HOWTO.txt FAQ.txt \
	configuration.txt vmailmgr-cgi.txt protocol.txt \
	record-format.txt
noinst_PODS = vdeliver.pod vmailmgrd.pod checkvpw.pod \
	vmailmgr.pod vpasswd5.pod vpasswd.pod \
	vadduser.pod vdeluser.pod vauthenticate.pod vchattr.pod \
	vaddusers.pod vpasswds.pod vsetup.pod vrehash.pod vpopbull.pod
noinst_MISC = translation.ps
EXTRA_DIST = ChangeLog-pre-vmailmgr ChangeLog-pre-0.70 \
	changelog2html.pl news2html.pl \
	TODO YEAR2000 translation.fig translation.ps \
	$(man_MANS) \
	$(noinst_PODS) \
	$(noinst_SGMLS) \
	$(noinst_TEXINFOS) \
	$(noinst_HTMLS) \
	$(noinst_TXTS)
CLEANFILES = $(noinst_HTMLS) $(man_MANS) \
	HOWTO.txt FAQ.txt configuration.txt vmailmgr-cgi.txt

POD2MAN	= pod2man --release="vmailmgr ${VERSION}" \
	--center="VMailMgr Tools Documentation"
POD2HTML= pod2html --noindex
TEXI2HTML= texi2html -monolithic -number
CL2HTML	= perl changelog2html.pl
NEWS2HTML = perl news2html.pl
FIG2PS = fig2dev -L ps -c
SGML2HTML = sgml2html --split=0
SGML2TXT = sgml2txt

SUFFIXES = .1 .5 .7 .8 .pod .html .texi .txt .ps .fig .sgml

all: $(noinst_HTMLS) $(noinst_TXTS) $(noinst_MISC)

dist-hook:
	touch $(distdir)/*.html
	touch $(distdir)/*.txt
	touch $(distdir)/*.[123456789]

vpasswd.5: vpasswd5.pod ../configure.in
	$(POD2MAN) --section=5 $< >$@

ChangeLog.html: ../ChangeLog
	$(CL2HTML) $< >$@

NEWS.html: ../NEWS
	$(NEWS2HTML) $< >$@

%.1: %.pod ../configure.in; $(POD2MAN) --section=1 $< >$@
%.5: %.pod ../configure.in; $(POD2MAN) --section=5 $< >$@
%.7: %.pod ../configure.in; $(POD2MAN) --section=7 $< >$@
%.8: %.pod ../configure.in; $(POD2MAN) --section=8 $< >$@
%.html: %.sgml; $(SGML2HTML) $<
%.txt: %.sgml; $(SGML2TXT) $<
%.html: %.pod; $(POD2HTML) $< >$@
%.html: %.texi; $(TEXI2HTML) $< >$@
%.txt: %.html; $(HTML2TXT) $< >$@

%.ps: %.fig ../configure.in; $(FIG2PS) $< $@
