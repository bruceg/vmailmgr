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
import random
import signal
import socket
import string
import time

class Session:
    def __init__(self, data):
        self.data = data
        self.touch()
    def touch(self):
        self.timestamp = time.time()

def make_socket(socket_file):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    os.unlink(socket_file)
    s.bind(socket_file)
    os.chmod(socket_file, 0777)
    s.listen(256)
    return s

def age_sessions(sessions, age_threshold):
    now = time.time()
    for key in sessions.keys():
        if now - sessions[key].timestamp > age_threshold:
            print "Expired session '%s'" % key
            del sessions[key]

def trap_alarm(signal, frame):
    raise IOError, "Caught alarm!"

def new_key():
    return "%f.%d" % (time.time(),
                      random.uniform(0, 1000000000))

def main(socket_file, age_threshold):
    sessions = { }
    sock = make_socket(socket_file)
    while 1:
        ( conn, address ) = sock.accept()
        try:
            age_sessions(sessions, age_threshold)
            conn = conn.makefile("r+")
            signal.signal(signal.SIGALRM, trap_alarm)
            signal.alarm(1)
            line = string.split(string.strip(conn.readline()))
            fn = line[0]
            key = line[1]
            if fn == 'get':
                print "Get '%s'" % key,
                if sessions.has_key(key):
                    sess = sessions[key]
                    sess.touch()
                    conn.write("%s\n%s" % (len(sess.data), sess.data))
                    print "OK, %d bytes" % len(sess.data)
                else:
                    print "NO"
            elif fn == 'set':
                datalen = int(line[2])
                print "Set '%s' (%d bytes)" % (key, datalen)
                data = conn.read(datalen)
                sess = Session(data)
                sessions[key] = sess
            elif fn == 'new':
                datalen = int(line[1])
                print "New (%d bytes)" % datalen,
                key = new_key()
                data = conn.read(datalen)
                sess = Session(data)
                sessions[key] = sess
                conn.write("%s\n" % key)
                print "OK '%s'" % key
            else:
                print "Unknown command '%s' from client" % fn
        except IOError, msg:
            print "Caught I/O error:", msg
        except:
            print "Caught untrapped exception!"
        signal.alarm(0)
        conn.close()
