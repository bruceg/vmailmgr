cgi_SCRIPTS = vmailmgr-pyadmin
sbin_SCRIPTS = vmailmgr-sessiond
pythonlib_DATA = \
	vmailmgr_lib.py vmailmgr_lib.pyc vmailmgr_lib.pyo \
	vmailmgr_local.py vmailmgr_local.pyc vmailmgr_local.pyo \
	vmailmgr_parser.py vmailmgr_parser.pyc vmailmgr_parser.pyo \
	vmailmgr_pyadmin.py vmailmgr_pyadmin.pyc vmailmgr_pyadmin.pyo \
	vmailmgr_sessiond.py vmailmgr_sessiond.pyc vmailmgr_sessiond.pyo
EXTRA_DIST = vmailmgr_lib.py \
	vmailmgr_parser.py \
	vmailmgr_pyadmin.py \
	vmailmgr_sessiond.py
CLEANFILES = $(cgi_SCRIPTS) $(sbin_SCRIPTS) \
	make-pymain vmailmgr_local.py *.pyc *.pyo

SUFFIXES = .py .pyc .pyo

vmailmgr_local.py: Makefile
	echo "sysconfdir = '${sysconfdir}'" >$@

make-pymain: Makefile
	{ echo '#!/bin/sh' ;\
	  echo "echo '#! ${PYTHON}'" ;\
	  echo 'echo "import $$1"' ;\
	  echo 'echo "$$1.main($$2)"' } >$@

vmailmgr-sessiond: make-pymain
	./make-pymain vmailmgr_sessiond "'/tmp/.vmailmgr-sessiond', 900" >$@

vmailmgr-pyadmin: make-pymain
	./make-pymain vmailmgr_pyadmin >$@

.py.pyc:
	python -c 'import $*'
.py.pyo:
	python -O -c 'import $*'
