cgi_SCRIPTS = vautoresponse vcommand
pythonlib_DATA = vmailmgr.py vmailmgr.pyc vmailmgr.pyo \
	vmailmgr_cgi.py vmailmgr_cgi.pyc vmailmgr_cgi.pyo
EXTRA_DIST = vautoresponse.in vcommand.in vmailmgr.py.in vmailmgr_cgi.py.in \
	vautoresponse.html vautoresponse-error.html vautoresponse-success.html
CLEANFILES = $(cgi_SCRIPTS) $(pythonlib_DATA)

SUFFIXES = .py .pyc .pyo .in .py.in

vautoresponse: vautoresponse.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"
	chmod +x $@

vcommand: vcommand.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"
	chmod +x $@

vmailmgr.py: vmailmgr.py.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"
	chmod +x $@

vmailmgr_cgi.py: vmailmgr_cgi.py.in Makefile
	sed <$< >$@ \
		-e "s|%PYTHON%|${PYTHON}|"  \
		-e "s|%SYSCONFDIR%|${sysconfdir}/vmailmgr|"
	chmod +x $@

.py.pyc:
	python -c 'import $*'
.py.pyo:
	python -O -c 'import $*'
