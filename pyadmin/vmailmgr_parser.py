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

import re
import string
import sys

###############################################################################
# Context management

def iif(cond, true, false):
    '''A functional equivalent to C's ?: operator.

    This function returns the value of 'true' if 'cond' evaluates to non-zero,
    or the value of 'false' otherwise.  Since it's a function, both 'true' and
    'false' are unconditionally evaluated (unlike 'cond?true:false' in C).
    '''
    if cond:
        return true
    else:
        return false

html_escapes = ( ('&', '&amp;'),
                 ('<', '&lt;'),
                 ('>', '&gr;'),
                 ('"', '&quot;') )

def escape(str, escapes = html_escapes):
    for (needle, replacement) in escapes:
        str = string.replace(str, needle, replacement)
    return str

global_context = {
    'iif': iif,
    'escape': escape,
    'math': __import__('math'),
    'random': __import__('random'),
    're': __import__('re'),
    'regsub': __import__('regsub'),
    'string': __import__('string'),
    'time': __import__('time'),
    }

class Context:
    def __init__(self, initdict = { }):
        self.stack = [ initdict ]
        self.pop()
    def pop(self):
        self.dict = self.stack.pop()
        self.get = self.dict.get
        self.has_key = self.dict.has_key
        self.items = self.dict.items
        self.keys = self.dict.keys
        self.values = self.dict.values
        self.update = self.dict.update
    def copy(self):
        return Context(self.dict.copy())
    def __getitem__(self, key):
        try:
            return self.dict[key]
        except KeyError:
            return eval(key, global_context, self.dict)
    def __getattr__(self, key):
        return self.dict[key]
    def __setitem__(self, key, val):
        self.dict[key] = val
    def push(self):
        self.stack.append(self.dict.copy())

def do_exec(body, context):
    exec(body, global_context, context.dict)

eval_cache = { }

def do_eval(body, context):
    '''Evaluate an expression, keeping a cache of results.'''
    if eval_cache.has_key(body):
        return eval_cache[body]
    result = eval(body, global_context, context.dict)
    eval_cache[body] = result
    return result

###############################################################################
# Node types

class Node:
    pass

def indent(level): return ' - ' * level

class Body(Node):
    def __init__(self):
        self.list = [ ]
    def append(self, node):
        self.list.append(node)
    def showtree(self, level):
        print indent(level), "Body"
        for item in self.list:
            item.showtree(level+1)
    def execute(self, context):
        for item in self.list:
            item.execute(context)

class Section(Node):
    '''Encapsulates an optionally formatted text section'''
    def __init__(self, string, formatted=0):
        self.section = string
    def showtree(self, level):
        print indent(level), "Section(%d bytes)" % len(self.section)
    def execute(self, context):
        return sys.stdout.write(self.section % context)

class End(Node):
    '''Ends a block node, like Start, Foreach, and If'''
    rx = re.compile(r'^end(\s+.*|)$')
    def __init__(self, groups):
        pass

class If(Node):
    rx = re.compile(r'^if\s+(.+)$')
    def __init__(self, groups):
        self.expr = groups[0]
        self.body = None
        self.else_node = None
        self.elseifs = [ ]
    def append_elseif(self, node):
        self.elseifs.append(node)
    def showtree(self, level):
        print indent(level), "If", self.expr
        self.body.showtree(level+1)
        for elseif in self.elseifs:
            elseif.showtree(level)
        if self.else_node:
            self.else_node.showtree(level)
    def execute(self, context):
        if do_eval(self.expr, context):
            return self.body.execute(context)
        for elseif in self.elseifs:
            if elseif.execute(context):
                return
        if self.else_node:
            return self.else_node.execute(context)

class Else(Node):
    '''Negates an If clause'''
    rx = re.compile(r'^else$')
    def __init__(self, groups):
        self.body = None
    def showtree(self, level):
        print indent(level), "Else"
        self.body.showtree(level+1)
    def execute(self, context):
        return self.body.execute(context)

class ElseIf(Node):
    '''Additional clauses to an If clause'''
    rx = re.compile(r'^else\s+if\s+(.+)$')
    def __init__(self, groups):
        self.expr = groups[0]
        self.body = None
    def showtree(self, level):
        print indent(level), "ElseIf", self.expr
        self.body.showtree(level+1)
    def execute(self, context):
        if do_eval(self.expr, context):
            self.body.execute(context)
            return 1
        return None

class Foreach(Node):
    '''Iterates over each item in a list'''
    rx = re.compile(r'^foreach\s+(.+)$')
    def __init__(self, groups):
        self.expr = groups[0]
        self.body = None
    def showtree(self, level):
        print indent(level), "Foreach", self.expr
        self.body.showtree(level + 1)
    def execute(self, context):
        context.push()
        list = do_eval(self.expr, context)
        for item in list:
            context.update(item)
            self.body.execute(context)
        context.pop()
    
special_node_types = [ Else, ElseIf, End, Foreach, If ]

class Expr(Node):
    def __init__(self, expr):
        self.expr = expr
    def showtree(self, level):
        print indent(level), "Expr", self.expr
    def execute(self, context):
        return do_exec(self.expr, context)

###############################################################################
# Lexical analysis

_rx_begin_cmd = re.compile(r'<\?\s*')
_rx_end_cmd = re.compile(r'\s*\?>\s*\r?\n?')

class Lexer:
    def __init__(self, content):
        self.currpos = 0
        self.after_escape = 0
        self.content = content

    def parse_node(self):
        if self.currpos >= len(self.content):
            return None
        if self.after_escape:
            return self.parse_node_after()
        else:
            return self.parse_node_before()
    def parse_node_after(self):
        end = _rx_end_cmd.search(self.content, self.currpos)
        if not end:
            raise ValueError, "Unterminated command escape sequence"
        self.after_escape = 0
        cmd = self.content[self.currpos:end.start()]
        self.currpos = end.end()
        return self.parse_cmd(cmd)
    def parse_node_before(self):
        start = _rx_begin_cmd.search(self.content, self.currpos)
        if start:
            self.after_escape = 1
            result = Section(self.content[self.currpos:start.start()])
            self.currpos = start.end()
        else:
            result = Section(self.content[self.currpos:])
            self.currpos = len(self.content)
        return result
    def parse_cmd(self, cmdstr):
        for type in special_node_types:
            match = type.rx.match(cmdstr)
            if match:
                return type(match.groups())
        return Expr(cmdstr)

###############################################################################
# Parsing mechanism

def parse(content):
    '''Parse the content into nodes, and then order into a tree structure'''
    list = [ ]
    lexer = Lexer(content)
    node = lexer.parse_node()
    while node is not None:
        print node
        list.append(node)
        node = lexer.parse_node()
    (node,index) = parse_body(list, 0)
    if index < len(list):
        raise ValueError, "Parsing ended before end-of-file"
    return node

def parse_body(nodes, index):
    body = Body()
    (node,newindex) = parse_statement(nodes, index)
    while node:
        body.append(node)
        (node,newindex) = parse_statement(nodes, newindex)
    return (body,newindex)

def parse_statement(nodes, index):
    (node,newindex) = parse_text_section(nodes, index)
    if node:
        return (node,newindex)
    (node,newindex) = parse_if_section(nodes, index)
    if node:
        return (node,newindex)
    (node,newindex) = parse_foreach_section(nodes, index)
    if node:
        return (node,newindex)
    (node,newindex) = parse_other_section(nodes, index)
    if node:
        return (node,newindex)
    return (None, index)

def parse_text_section(nodes, index):
    if index < len(nodes) and isinstance(nodes[index], Section):
        return (nodes[index], index+1)
    return (None, index)

def parse_if_section(nodes, index):
    if index < len(nodes) and isinstance(nodes[index], If):
        top = nodes[index]
        (body, newindex) = parse_body(nodes, index+1)
        if body:
            top.body = body
            while newindex < len(nodes) and \
                  isinstance(nodes[newindex], ElseIf):
                (body, tmpindex) = parse_body(nodes, newindex+1)
                if body:
                    nodes[newindex].body = body
                    top.append_elseif(nodes[newindex])
                    newindex = tmpindex
            if newindex < len(nodes) and isinstance(nodes[newindex], Else):
                (body, tmpindex) = parse_body(nodes, newindex+1)
                if body:
                    nodes[newindex].body = body
                    top.else_node = nodes[newindex]
                    newindex = tmpindex
            if newindex < len(nodes) and isinstance(nodes[newindex], End):
                return (top, newindex+1)
    return (None, index)

def parse_foreach_section(nodes, index):
    if index < len(nodes) and isinstance(nodes[index], Foreach):
        top = nodes[index]
        (body, endindex) = parse_body(nodes, index+1)
        if body and \
           endindex < len(nodes) and isinstance(nodes[endindex], End):
            top.body = body
            return (top, endindex+1)
    return (None, index)

def parse_other_section(nodes, index):
    if index < len(nodes) and isinstance(nodes[index], Expr):
        return (nodes[index], index+1)
    return (None, index)

def format(content, context):
    if not isinstance(context, Context):
        context = Context(context)
    tree = parse(content)
    return tree.execute(context)

if __name__ == '__main__':
    body = sys.stdin.read()
    tree = parse(body)
    tree.showtree(0)
    context = Context({'username':'nobody', 'domain':'testdomain.org'})
    tree.execute(context)
    print context.dict
