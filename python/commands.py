# Copyright (C) 1999,2000 Bruce Guenter <bruce@untroubled.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import daemon
import string
import types

def execute(command, *args):
    return daemon.execute(command, args)

def check(domain, username, password):
    return execute('check', domain, username, password)

def listdomain(domain, password):
    conn = daemon.Daemon()
    response = conn.execute(daemon.Command('listdomain', (domain, password)))
    users = []
    while 1:
        r = conn.read_response()
        if not r:
            break
        (username,data) = string.split(r, '\0', 1)
        users.append(types.NamedVUser(username, types.VUser(data)))
    return users

def adduser(domain, newuser, password, newpass,
            maildir=None, has_mailbox=None, forwards=[]):
    return daemon.execute('adduser3',
                          (domain, newuser, password, newpass,
                           maildir or '', has_mailbox or '') + tuple(forwards))

def lookup(domain, username, password):
    return types.VUser(execute('lookup', domain, username, password))

def autoresponse(domain, user, password, action, message=None):
    cmd = [ domain, user, password, action ]
    if message:
        cmd.append(message)
    return daemon.execute('autoresponse', cmd)

def deluser(domain, username, password):
    return execute('deluser', domain, username, password)

def chattr(domain, username, password, attribute, *value):
    return daemon.execute('chattr', (domain,username,password,attribute)+value)
