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

__init__.pyc: __init__.py local.py
__init__.pyo: __init__.py local.py
commands.pyc: commands.py local.py
commands.pyo: commands.py local.py
config.pyc: config.py local.py
config.pyo: config.py local.py
constants.pyc: constants.py local.py
constants.pyo: constants.py local.py
daemon.pyc: daemon.py local.py
daemon.pyo: daemon.py local.py
types.pyc: types.py local.py
types.pyo: types.py local.py

.py.pyc:
	python -c 'import $*' && touch $@
.py.pyo:
	python -O -c 'import $*' && touch $@
