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

import string
import time

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
