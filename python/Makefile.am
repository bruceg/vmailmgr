cgi_SCRIPTS = vautoresponse vcommand
pythonlib_DATA = vmailmgr.py vmailmgr.pyc vmailmgr.pyo \
	vmailmgr_cgi.py vmailmgr_cgi.pyc vmailmgr_cgi.pyo
EXTRA_DIST = vautoresponse.in vcommand.in vmailmgr.py.in vmailmgr_cgi.py.in
CLEANFILES = $(cgi_SCRIPTS) $(pythonlib_DATA)

SUFFIXES = .py .pyc .pyo .in .py.in
%: %.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"
%.py: %.py.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"

.py.pyc:
	python -c 'import $*'
.py.pyo:
	python -O -c 'import $*'


