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

import anydbm
import cgi
import os
import random
import re
import socket
import string
import sys
import time

import vmailmgr_parser
import vmailmgr_lib

template_dir = sys.argv[0] + '-templates'
default_page = 'index'
sessiond_socket = '/tmp/.vmailmgr-sessiond'

PAGE = intern('PAGE')
FIELD = intern('FIELD')
SESSION = intern('SESSION')
MESSAGE = intern('MESSAGE')
DOMAIN = intern('DOMAIN')
USERNAME = intern('USERNAME')

###############################################################################
# Support routines

def require(form, *fields):
    list = []
    for field in fields:
        try:
            list.append(form[field])
        except KeyError:
            form[FIELD] = field
            format_page('missing-field', form)
            sys.exit(0)
    return list

def optional(form, *fields):
    list = []
    for field in fields:
        try:
            value = form[field]
        except KeyError:
            value = ''
        list.append(value)
    return list

def redirect(path):
    try:
        hostname = os.environ['HTTP_HOST']
    except KeyError:
        hostname = ''
    if not hostname:
        hostname = os.environ['SERVER_NAME']
    fullpath = 'http://%s%s' % ( hostname,
                                 os.path.join('/', os.environ['SCRIPT_NAME'],
                                              path) )
    sys.stdout.write('Location: %s\n\n'
                     'Redirected <a href="%s">here</a>\n' % (
        fullpath, fullpath ) )
    sys.exit(0)

def set_cookie(var,val):
    sys.stdout.write("Set-Cookie: %s=%s\n" % (var, val))

def trap_vmailmgr_call(fn, args):
    try:
        return (1, apply(fn, args))
    except vmailmgr_lib.Error, msg:
        return (0, msg)
    except vmailmgr_lib.Econn, msg:
        return (0, msg)
    except vmailmgr_lib.Bad, msg:
        return (0, msg)

def load_file(filename):
    fullpath = os.path.join(template_dir, filename)
    return open(fullpath, 'r').read()

def format_page(basename, form):
    basename = string.replace(basename, '.', ':')
    httpname = basename + '.vhttp'
    htmlname = basename + '.vhtml'
    try:
        vmailmgr_parser.format(load_file(httpname), form)
    except IOError:
        pass
    sys.stdout.write("\n")
    vmailmgr_parser.format(load_file(htmlname), form)

def auto_convert(str):
    try: return int(str)
    except ValueError: pass

    try: return float(str)
    except ValueError: pass

    return str

###############################################################################
# Session management

NUL = intern(chr(0))

def split_dict(data):
    dict = { }
    for part in string.split(data, NUL):
        (key,val) = string.split(part, '=', 1)
        dict[key] = auto_convert(val)
    return dict

def join_dict(dict):
    return string.join(map(lambda x,y:"%s=%s"%(x,y),
                           dict.keys(), dict.values()),
                       NUL)

def sessiond_cmd(cmd):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(sessiond_socket)
    s = s.makefile('r+')
    s.write("%s\n" % cmd)
    return s

def load_session(key):
    s = sessiond_cmd("get %s" % key)
    try:
        len = int(s.readline())
        return split_dict(s.read(len))
    except:
        return None

def save_session(key, dict):
    data = join_dict(dict)
    s = sessiond_cmd("set %s %d" % (key, len(data)))
    s.write(data)
    return s.close()

def new_session(dict):
    data = join_dict(dict)
    s = sessiond_cmd("new %d" % len(data))
    s.write(data)
    return string.strip(s.readline())

def del_session(key):
    #s = sessiond_cmd("del %s" % key)
    pass

###############################################################################
# Login/out actions

def do_login_failed(form, reason):
    if reason:
        form['reason'] = reason
        format_page('login-failed', form)
    else:
        format_page('login-form', form)

def do_login(form):
    (domain,password) = require(form, 'domain', 'password')
    (username,) = optional(form, 'username')
    try:
        vmailmgr_lib.check(domain, username, password)
    except vmailmgr_lib.Error, msg:
        return do_login_failed(form, "Login failed: %s" % msg)
    session = { }
    session['domain'] = domain
    session['username'] = username
    session['password'] = password
    key = new_session(session)
    set_cookie(SESSION, key)
    form[SESSION] = key
    return session

def do_logout(form):
    if form.has_key(SESSION):
        set_cookie(SESSION, None)
        del_session(form[SESSION])
    redirect('')

###############################################################################
# vmailmgr function wrapper class

def trap_vmailmgr_call(fn, args):
    try:
        return (1, apply(fn, args))
    except vmailmgr_lib.Error, msg:
        return (0, msg)
    except vmailmgr_lib.Econn, msg:
        return (0, msg)
    except vmailmgr_lib.Bad, msg:
        return (0, msg)

def trap_vmailmgr_response(context, fn, args):
    (ok,resp) = trap_vmailmgr_call(fn, args)
    context['response'] = resp
    return ok

class VmailmgrWrapper:
    def __init__(self, session, context):
        self.session = session
        self.context = context
        self.domain = session['domain']
        self.password = session['password']
        username = self.session['username'] or \
                   context.get('username', None)
        context[USERNAME] = username
        self.username = username

    def autoresponse_status(self):
        return vmailmgr_lib.autoresponse(self.domain, self.username,
                                         self.password, 'status')

    def autoresponse_read(self):
        return vmailmgr_lib.autoresponse(self.domain, self.username,
                                         self.password, 'read')

    def autoresponse_enable(self):
        return trap_vmailmgr_response(self.context,
                                      vmailmgr_lib.autoresponse, (
            self.domain, self.username, self.password, 'enable' ) )

    def autoresponse_disable(self):
        return trap_vmailmgr_response(self.context,
                                      vmailmgr_lib.autoresponse, (
            self.domain, self.username, self.password, 'disable' ) )

    def autoresponse_delete(self):
        return trap_vmailmgr_response(self.context,
                                      vmailmgr_lib.autoresponse, (
            self.domain, self.username, self.password, 'delete' ) )

    def autoresponse_write(self, body):
        return trap_vmailmgr_response(self.context,
                                      vmailmgr_lib.autoresponse, (
            self.domain, self.username, self.password, 'write', body ) )

    def lookup_user(self):
        vuser = vmailmgr_lib.lookup(self.domain, self.username, self.password)
        self.context.update(vuser.dict())
        return 1

    def list_domain(self, sortkey = 'username'):
        key = intern(sortkey)
        list = map(lambda x:x.dict(),
                   vmailmgr_lib.listdomain(self.domain, self.password))
        list.sort(lambda x,y,key=key:cmp(x[key],y[key]))
        return list

    def deluser(self):
        return trap_vmailmgr_response(self.context, vmailmgr_lib.deluser, (
            self.domain, self.username, self.password ) )

    def adduser(self, newpass, forwards):
        return trap_vmailmgr_response(self.context, vmailmgr_lib.adduser, (
            self.domain, self.username, self.password, newpass,
            None, forwards ) )

    def chattr(self, attribute, *value):
        return trap_vmailmgr_response(self.context, vmailmgr_lib.adduser, (
            self.domain, self.username, self.password ) + tuple(value) )

###############################################################################
# The main dispatch routine

def handle_login(page, form):
    if not form.has_key('domain') and \
       not form.has_key('password'):
        return do_login_failed(form, '')
    if not form.has_key('domain'):
        return do_login_failed(form, 'Form was missing the domain name')
    if not form.has_key('password'):
        return do_login_failed(form, 'Form was missing the pass phrase')
    return do_login(form)

def set_session(form):
    session = None
    if form.has_key(SESSION):
        session = load_session(form[SESSION])
    return session
    
def cgi_main(form):
    try:
        path = os.environ['PATH_INFO']
    except KeyError:
        return redirect('')
    page = string.split(path, '/')[-1]
    form[PAGE] = page

    if page == 'logout':
        return do_logout(form)

    try:
        session = set_session(form)
        if not session:
            if page:
                return redirect('')
            else:
                session = handle_login(page, form)
        if session:
            form[DOMAIN] = session['domain']
            form[USERNAME] = session['username']
            context = vmailmgr_parser.Context(form)
            vmailmgr_fns = VmailmgrWrapper(session, context)
            vmailmgr_parser.global_context['vmailmgr'] = vmailmgr_fns
            format_page(page or default_page, context)
            #save_session(form[SESSION], session)
    except vmailmgr_lib.Econn, message:
        form[MESSAGE] = message
        format_page('connection-failed', form)
    except vmailmgr_lib.Bad, message:
        form[MESSAGE] = message
        format_page('BAD', form)

###############################################################################
# Startup code

def load_cookies():
    dict = { }
    try:
        for cookie in string.split(os.environ['HTTP_COOKIE'], '; '):
            try:
                (key,val) = string.split(cookie, '=', 1)
            except ValueError:
                continue
            dict[key] = auto_convert(val)
    except KeyError:
        pass
    return dict

def load_form():
    dict = { }
    form = cgi.FieldStorage()
    for item in form.list:
        if item.file:
            continue
        dict[item.name] = auto_convert(item.value)
    return dict

def load_data():
    result = load_cookies()
    result.update(load_form())
    return result

def main():
    if not os.environ.has_key('REQUEST_METHOD'):
        sys.stdout.write("This program must be run as a CGI\n")
        sys.exit(1)
    
    sys.stdout.write("Content-Type: text/html\n")

    try:
        form = load_data() # VFieldStorage()
        cgi_main(form)
    except SystemExit:
        pass
    except:
        print
        cgi.print_exception()
