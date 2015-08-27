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

import config

ok = 0
bad = 1
err = 2
econn = 3

class Bad(Exception):   pass
class Error(Exception): pass
class Econn(Exception): pass

def encode_int(i):
    return chr((i/256)%256) + chr(i%256)

def encode_str(s):
    s = str(s)
    return encode_int(len(s)) + s

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
        self.socket_file = config.read('socket-file', '/tmp/.vmailmgrd')
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
            if msglen:
                message = self.socket.recv(msglen)
            else:
                message = ""
        except:
            return (econn, 'Server aborted the connection')
        return (code, message)
        
    def read_response(self):
        (code, message) = self.read_response_noraise()
        if code == err:
            raise Error(message)
        if code == bad:
            raise Bad(message)
        if code == econn:
            raise Econn(message)
        return message
        
    def execute(self, command):
        try:
            self.connect()
        except:
            raise Econn("Unable to connect to the server")
        data = command.encode()
        try:
            self.socket.send(data)
        except:
            raise Econn("Server aborted the connection")
        # Don't close the socket here -- it will be closed
        # as soon as it gets garbage collected, and remains
        # usable for other commands like listdomain.
        return self.read_response()

def execute(command, args):
    return Daemon().execute(Command(command, args))
