# Copyright (C) 1999,2000 Bruce Guenter <bruceg@em.ca>
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

import os
import string
import time

import vmailmgr_local

def encode_int(i):
    return chr((i/256)%256) + chr(i%256)

def encode_str(str):
    return encode_int(len(str)) + str

def read_config(name, default):
    try:
        file = open(os.path.join(vmailmgr_local.sysconfdir, name))
        result = string.strip(file.readline())
        file.close()
    except:
        result = default
    return result

class Limit:
    unlimited = -1
    def __init__(self, str):
        if str[0] == '-' or string.lower(str) == 'unlimited':
            str = self.unlimited
        self.value = int(str)
    def __str__(self):
        if self.value == self.unlimited:
            return 'unlimited'
        return str(self.value)
    def __repr__(self):
        if self.value == self.unlimited:
            val = '-'
        else:
            val = str(self.value)
        return "Limit('%s')" % val

class Flag:
    def __init__(self, value):
        self.value = value
    def __str__(self):
        if self.value:
            return 'true'
        return 'false'
    def __repr__(self):
        return "Flag(%s)" % repr(self.value)
    def html(self, fieldname):
        if self.value:
            strue = ' selected'
            sfalse = ''
        else:
            strue = ''
            sfalse = ' selected'
        return ("<select name='%s'>"
                "<option value=1%s>True"
                "<option value=0%s>False"
                "</select>" % ( fieldname, strue, sfalse ) )

class Time:
    def __init__(self, value):
        self.time = int(value)
    def __str__(self):
        return time.asctime(time.localtime(self.time))
    def __repr__(self):
        return 'Time(%d)' % self.time

ATTR_MAILBOX_ENABLED = 8

class VUser:
    # Default values
    mailbox_enabled = 1
    password = ''
    mailbox = ''
    forwards = []
    personal = ''
    hard_quota = -1
    soft_quota = -1
    message_size_limit = -1
    message_count_limit = -1
    creation_time = 0
    expiry_time = -1
    
    def __init__(self, bin=None):
        if bin:
            self.from_binary(bin)

    def from_binary(self, bin):
        if bin[0] <> chr(2):
            raise ValueError, "Virtual user data has invalid format number"
        i = 1
        while bin[i] <> chr(0):
            flag = ord(bin[i])
            val = ord(bin[i+1])
            i = i + 2
            if flag == ATTR_MAILBOX_ENABLED:
                self.mailbox_enabled = Flag(val)
            else:
                raise ValueError, "Invalid flag number %d in vuser data" % flag
        bin = string.split(bin[i+1:], '\0')
        self.password = bin[0]
        self.mailbox = bin[1]
        self.forwards = []
        i = 2
        while bin[i]:
            self.forwards.append(bin[i])
            i = i + 1
        self.personal = bin[i+1]
        self.hard_quota = Limit(bin[i+2])
        self.soft_quota = Limit(bin[i+3])
        self.message_size_limit = Limit(bin[i+4])
        self.message_count_limit = Limit(bin[i+5])
        self.creation_time = Time(bin[i+6])
        self.expiry_time = Limit(bin[i+7])

    def __getitem__(self, key):
        """Allow subscript access to the attributes of this object.

        This method allows the data in the virtual user to be
        accessed as though it were a dictionary.  This allows the use of
        such expressions as '%(mailbox)s'%user."""
        return getattr(self, key)

    def keys(self): return self.__dict__.keys()
    def values(self): return self.__dict__.values()
    def items(self): return self.__dict__.items()
    def dict(self): return self.__dict__

class NamedVUser:
    def __init__(self, username, vuser):
        self.username = username
        self.vuser = vuser
    def __cmp__(self, other):
        return cmp(self.username, other.username)
    def __getitem__(self, key):
        if key == 'username':
            return self.username
        else:
            return self.vuser[key]
    def dict(self):
        d = self.vuser.dict()
        d['username'] = self.username
        return d


ok = 0
bad = 1
err = 2
econn = 3

class Bad(Exception):   pass
class Error(Exception): pass
class Econn(Exception): pass

class Command:
    def __init__(self, name, args):
        self.name = name
        self.args = args
    
    def encode(self):
        argcount = len(self.args)
        cmd = chr(argcount) + encode_str(self.name)
        for i in range(argcount):
            cmd = cmd + encode_str(self.args[i])
        cmd = chr(2) + encode_int(len(cmd)) + cmd
        return cmd

class Daemon:
    def __init__(self):
        self.socket_file = read_config('socket-file', '/tmp/.vmailmgrd')
        self.socket = None

    def connect(self):
        import socket
        self.socket = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.socket.connect(self.socket_file)

    def read_response_noraise(self):
        try:
            reply = self.socket.recv(3)
            code = ord(reply[0])
            msglen = ord(reply[1])*256 + ord(reply[2])
            message = self.socket.recv(msglen)
        except:
            return (econn, 'Server aborted the connection')
        return (code, message)
        
    def read_response(self):
        (code, message) = self.read_response_noraise()
        if code == err:
            raise Error, message
        if code == bad:
            raise Bad, message
        if code == econn:
            raise Econn, message
        return message
        
    def execute(self, command):
        try:
            self.connect()
        except:
            raise Econn, "Unable to connect to the server"
        try:
            self.socket.send(command.encode())
        except:
            raise Econn, "Server aborted the connection"
        # Don't close the socket here -- it will be closed
        # as soon as it gets garbage collected, and remains
        # usable for other commands like listdomain.
        return self.read_response()

def execute(command, args):
    return Daemon().execute(Command(command, args))

def check(domain, username, password):
    return Daemon().execute(Command('check', (domain, username, password)))

def listdomain(domain, password):
    daemon = Daemon()
    response = daemon.execute(Command('listdomain', (domain, password)))
    users = []
    while 1:
        r = daemon.read_response()
        if not r:
            break
        (username,data) = string.split(r, '\0', 1)
        users.append(NamedVUser(username, VUser(data)))
    return users

def adduser(domain, newuser, password, newpass, maildir=None, forwards=[]):
    return Daemon().execute(Command('adduser2',
                                    (domain, newuser, password, newpass,
                                     maildir or newuser) + tuple(forwards)))

def lookup(domain, username, password):
    data = Daemon().execute(Command('lookup', (domain, username, password)))
    return VUser(data)

def autoresponse(domain, user, password, action, message=None):
    cmd = [ domain, user, password, action ]
    if message:
        cmd.append(message)
    return Daemon().execute(Command('autoresponse', cmd))

def deluser(domain, username, password):
    return execute('deluser', (domain, username, password))

def chattr(domain, username, password, attribute, *value):
    return execute('chattr', (domain,username,password,attribute)+value)
