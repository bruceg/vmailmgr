cgi_SCRIPTS = vcommand
pythonlib_DATA = vmailmgr.py vmailmgr.pyc vmailmgr.pyo
EXTRA_DIST = vcommand.in vmailmgr.py.in
CLEANFILES = vcommand vmailmgr.py *.pyo *.pyc

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


