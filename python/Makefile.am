cgi_SCRIPTS = vcommand
pythonlib_DATA = vmailmgr.py vmailmgr.pyc vmailmgr.pyo
EXTRA_DIST = vcommand.py vmailmgr.py.in
CLEANFILES = vcommand vmailmgr.py *.pyo *.pyc

SUFFIXES = .py .pyc .pyo
vcommand: vcommand.py
	sed -e "s|%PYTHON%|${PYTHON}|" vcommand.py >vcommand
	chmod +x vcommand
.py.pyc:
	python -c 'import $*'
.py.pyo:
	python -O -c 'import $*'

vmailmgr.py: vmailmgr.py.in
	sed -e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|" vmailmgr.py.in >$@

