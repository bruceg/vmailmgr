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

global_context = {
    'iif': iif,
    'math': __import__('math'),
    'random': __import__('random'),
    're': __import__('re'),
    'regsub': __import__('regsub'),
    'string': __import__('string'),
    'time': __import__('time'),
    }

class Context:
    def __init__(self, initdict = { }):
        #self.stack = [ initdict ]
        #self.pop()
        #def pop(self):
        #self.dict = self.stack.pop()
        self.dict = initdict
        for method in dir(self.dict):
            setattr(self, method, getattr(self.dict, method))
    def __getitem__(self, key):
        try:
            return self.dict[key]
        except KeyError:
            return eval(key, global_context, self.dict)
    def __getattr__(self, key):
        return self.dict[key]
    def __setitem__(self, key, val):
        self.dict[key] = val
    #def push(self):
    #    self.stack.append(self.dict.copy())

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
# Parsing mechanism

class Stack:
    def __init__(self):
        self.stack = [ ]
        self.fake_depth = 0
    def push(self, token):
        self.stack.append(token)
    def fake_push(self):
        self.fake_depth = self.fake_depth + 1
    def pop(self):
        if self.fake_depth:
            self.fake_depth = self.fake_depth - 1
            return None
        return self.stack.pop()
    def top(self):
        return self.stack[-1]
    def empty(self):
        return len(self.stack) == 0
    def append_or_print(self, section, context):
        frame = len(self.stack)
        while section and frame > 0:
            frame = frame - 1
            section = self.stack[frame].append(section)
        if section:
            section.format(context)
            sys.stdout.write(str(section))

class Token:
    '''Token placeholder class

    All tokens must define the following methods:
    * apply(stack, context) -- used to "execute" the token.
    Syntax tokens manipulate the stack and context here.
    
    Tokens on the stack must define the following methods:
    * do_end(stack, context) -- called *after* the token is popped off
    the stack by an end token.
    * append(section) -- either appends the section to the current token
    and returns None, or returns the section (potentially formatting it
    in the process).
    * is_true() -- Returns true if tokens are to be pushed onto the stack.
    
    Tokens that handle an "else" clause must define:
    * do_else()
    '''
    
    pass

class Start(Token):
    '''Is pushed onto the stack initially to provide for a non-empty stack'''
    def __init__(self):
        pass
    def apply(self, stack, context):
        stack.push(self)
    def do_end(self, stack, context):
        pass
    def append(self, section):
        return section
    def is_true(self):
        return 1

class Section(Token):
    '''Encapsulates an optionally formatted text section'''
    def __init__(self, string, formatted=0):
        self.section = string
        self.formatted = formatted
    def apply(self, stack, context):
        stack.append_or_print(self, context)
    def format(self, context):
        if not self.formatted:
            self.section = self.section % context
            self.formatted = 1
    def __str__(self):
        return self.section
    def __repr__(self):
        return "Section('%s', %s)" % (self.section, self.formatted)
    def copy(self):
        return Section(self.section, self.formatted)

class End(Token):
    '''Ends a block token, like Start, Foreach, and If'''
    rx = re.compile(r'^end(\s+.*|)$')
    def __init__(self, groups):
        pass
    def apply(self, stack, context):
        top = stack.pop()
        if top:
            return top.do_end(stack, context)
        return None

class If(Token):
    rx = re.compile(r'^if\s+(.+)$')
    def __init__(self, groups):
        self.expr = groups[0]
    def apply(self, stack, context):
        if stack.top().is_true():
            self.truth = do_eval(self.expr, context)
            self.depth = 0
            stack.push(self)
        else:
            stack.fake_push()
    def do_end(self, stack, context):
        pass
    def do_else(self):
        self.truth = not self.truth
    def append(self, section):
        if not self.truth:
            section = None
        return section
    def is_true(self):
        return self.truth

class Else(Token):
    '''Negates an If clause'''
    rx = re.compile(r'^else$')
    def __init__(self, groups):
        pass
    def apply(self, stack, context):
        stack.top().do_else()

class Foreach(Token):
    '''
    rx = re.compile(r'^foreach\s+(.+)$')
    def __init__(self, groups):
        self.expr = groups[0]
        self.sections = [ ]
    def apply(self, stack, context):
        if stack.top().is_true():
            self.saved_context = context
            self.results = do_eval(self.expr, context)
            stack.push(self)
        else:
            stack.fake_push()
    def copy_sections(self):
        copy = range(len(self.sections))
        for i in copy:
            copy[i] = self.sections[i].copy()
        return copy
    def do_end(self, stack, context):
        context = context.copy()
        for item in self.results:
            context.update(item)
            for section in self.copy_sections():
                section.format(context)
                stack.append_or_print(section, context)
    def append(self, section):
        self.sections.append(section)
        return None
    def is_true(self):
        return 1

special_token_types = [ Else, End, Foreach, If ]

class Expr(Token):
    def __init__(self, expr):
        self.expr = expr
    def apply(self, stack, context):
        if stack.top().is_true():
            do_exec(self.expr, context)

_rx_begin_cmd = re.compile(r'<\?\s*')
_rx_end_cmd = re.compile(r'\s*\?>\s*\r?\n?')

class Lexer:
    def __init__(self, content):
        self.currpos = 0
        self.after_escape = 0
        self.content = content
    
    def get_token(self):
        if self.currpos >= len(self.content):
            return None
        if self.after_escape:
            return self.get_token_after()
        else:
            return self.get_token_before()
    def get_token_after(self):
        end = _rx_end_cmd.search(self.content, self.currpos)
        if not end:
            raise ValueError, "Unterminated command escape sequence"
        self.after_escape = 0
        cmd = self.content[self.currpos:end.start()]
        self.currpos = end.end()
        return self.parse_cmd(cmd)
    def get_token_before(self):
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
        for type in special_token_types:
            match = type.rx.match(cmdstr)
            if match:
                return type(match.groups())
        return Expr(cmdstr)

def format(content, context):
    if not isinstance(context, Context):
        context = Context(context)
    
    stack = Stack()
    Start().apply(stack, context)

    lex = Lexer(content)
    token = lex.get_token()
    while token:
        token.apply(stack, context)
        token = lex.get_token()

    while not stack.empty():
        End(None).apply(stack, context)

if __name__ == '__main__':
    format(sys.stdin.read(),
           { 'domain':'testdomain.org', 'username':'nobody' })
