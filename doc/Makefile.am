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

CL2HTML	= perl changelog2html.pl
NEWS2HTML = perl news2html.pl
FIG2PDF = fig2dev -L pdf -c
FIG2PS = fig2dev -L ps -c
TEXI2HTML = texi2any --html --no-split
TEXI2PDF = texi2pdf --clean --quiet
TEXI2INFO = texi2any --info --no-split --number-sections
TEXI2TXT = texi2any --plaintext

all: $(noinst_HTMLS) $(noinst_PDFS) $(noinst_TXTS) $(noinst_MISC)

dist-hook:
	touch $(distdir)/*.html
	touch $(distdir)/*.txt
	touch $(distdir)/*.[123456789]

ChangeLog.html: ../ChangeLog
	$(CL2HTML) $< >$@

NEWS.html: ../NEWS
	$(NEWS2HTML) $< >$@

.pod.1: ../configure.ac; $(POD2MAN) --section=1 $< >$@
.pod.5: ../configure.ac; $(POD2MAN) --section=5 $< >$@
.pod.7: ../configure.ac; $(POD2MAN) --section=7 $< >$@
.pod.8: ../configure.ac; $(POD2MAN) --section=8 $< >$@
.pod.html: ../configure.ac; $(POD2HTML) $< >$@
.texi.html: ../configure.ac; $(TEXI2HTML) $< >$@
.texi.txt: ../configure.ac; $(TEXI2TXT) $< >$@
.texi.pdf: ../configure.ac; $(TEXI2PDF) $<
.texi.info: ../configure.ac; $(TEXI2INFO) $<

.fig.pdf: ../configure.ac; $(FIG2PDF) $< $@
.fig.ps: ../configure.ac; $(FIG2PS) $< $@
