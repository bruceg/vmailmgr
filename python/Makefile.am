pythonlib_DATA = \
	__init__.py __init__.pyc __init__.pyo \
	commands.py commands.pyc commands.pyo \
	config.py config.pyc config.pyo \
	constants.py constants.pyc constants.pyo \
	daemon.py daemon.pyc daemon.pyo \
	local.py local.pyc local.pyo \
	types.py types.pyc types.pyo
EXTRA_DIST = __init__.py commands.py config.py constants.py daemon.py types.py
CLEANFILES = local.py *.pyc *.pyo

local.py: Makefile
	echo "sysconfdir = '${sysconfdir}'" >$@

config.pyc: config.py local.py
config.pyo: config.py local.py

.py.pyc:
	python -c 'import $*'
.py.pyo:
	python -O -c 'import $*'
