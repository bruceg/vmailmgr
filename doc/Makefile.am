man_MANS = vmailmgr.7
noinst_TEXINFOS = FAQ.texi HOWTO.texi configuration.texi vmailmgr-cgi.texi
noinst_HTMLS = vmailmgr.html HOWTO.html FAQ.html NEWS.html \
	configuration.html ChangeLog.html vmailmgr-cgi.html
noinst_TXTS = HOWTO.txt FAQ.txt \
	configuration.txt vmailmgr-cgi.txt protocol.txt \
	record-format.txt
noinst_PODS = vmailmgr.pod
noinst_MISC = FAQ.info FAQ.pdf HOWTO.info HOWTO.pdf \
	configuration.info translation.pdf translation.ps
EXTRA_DIST = ChangeLog-pre-vmailmgr ChangeLog-pre-0.70 \
	changelog2html.pl news2html.pl \
	YEAR2000 translation.fig \
	$(man_MANS) \
	$(noinst_PODS) \
	$(noinst_TEXINFOS) \
	$(noinst_HTMLS) \
	$(noinst_MISC) \
	$(noinst_TXTS)
CLEANFILES = $(noinst_HTMLS) $(man_MANS) $(noinst_MISC) \
	HOWTO.txt FAQ.txt configuration.txt vmailmgr-cgi.txt \
	pod2htmd.tmp pod2htmi.tmp

TEXI2HTML = texi2html -monolithic -number
CL2HTML	= perl changelog2html.pl
NEWS2HTML = perl news2html.pl
FIG2PDF = fig2dev -L pdf -c
FIG2PS = fig2dev -L ps -c
TEXI2PDF = texi2pdf --clean --quiet
TEXI2INFO = makeinfo --no-split --number-sections

all: $(noinst_HTMLS) $(noinst_PDFS) $(noinst_TXTS) $(noinst_MISC)

dist-hook:
	touch $(distdir)/*.html
	touch $(distdir)/*.txt
	touch $(distdir)/*.[123456789]

ChangeLog.html: ../ChangeLog
	$(CL2HTML) $< >$@

NEWS.html: ../NEWS
	$(NEWS2HTML) $< >$@

%.1: %.pod ../configure.in; $(POD2MAN) --section=1 $< >$@
%.5: %.pod ../configure.in; $(POD2MAN) --section=5 $< >$@
%.7: %.pod ../configure.in; $(POD2MAN) --section=7 $< >$@
%.8: %.pod ../configure.in; $(POD2MAN) --section=8 $< >$@
%.html: %.pod; $(POD2HTML) $< >$@
%.html: %.texi; $(TEXI2HTML) $< >$@
%.txt: %.html; $(HTML2TXT) $< >$@
%.pdf: %.texi; $(TEXI2PDF) $<
%.info: %.texi; $(TEXI2INFO) $<

%.pdf: %.fig ../configure.in; $(FIG2PDF) $< $@
%.ps: %.fig ../configure.in; $(FIG2PS) $< $@
