// Copyright (C) 1999,2000 Bruce Guenter <bruceg@em.ca>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <config.h>
#include <stdlib.h>
#include "cgi/cgi-base.h"
#include "misc/server.h"
#include "vpwentry/vpwentry.h"
#include "fdbuf/fdbuf.h"

static mystring domain;
static mystring userlink;

static mystring cell_pre;
static mystring cell_post;

static mystring p_pre;

static mystring table_align;

mystring do_subst(mystring link, const mystring& ref)
{
  int pos;
  int start = 0;
  unsigned advance = ref.length();
  
  while((pos = link.find_first('%', start)) >= 0) {
    link = link.left(pos) + ref + link.right(pos+1);
    start = pos + advance;
  }
  return link;
}

struct user
{
  mystring name;
  mystring code;
  user* next;
  user(const mystring& n, const mystring& c);
};

user::user(const mystring& n, const mystring& c)
  : name(n), code(c), next(0)
{
}

void show_user(const user* user)
{
  vpwentry vpw;
  vpw.from_record(user->name, user->code);
  
  mystring link;
  mystring elink;

  if(!!userlink) {
    link = "<a href=\"" + do_subst(userlink, vpw.name) + "\">";
    elink = "</a>";
  }
  
  fout << "<tr>"
       << cell_pre << link << vpw.name << elink << cell_post
       << cell_pre << vpw.mailbox << cell_post;
  for(mystring_iter iter = vpw.forwards; iter; ++iter)
    fout << cell_pre << *iter << cell_post;
  fout << "</tr>\n";
}

mystring load_users(int fd, unsigned& count, user**& array)
{
  user* tail = 0;
  user* head = 0;
  for(count = 0; ; count++) {
    response r(response::read(fd));
    if(!r)
      return r.message();
    if(!r.msg)
      break;
    mystring name(r.msg.c_str());
    mystring code(r.msg.c_str() + name.length()+1,
		  r.msg.length() - name.length()-1);
    user* node = new user(name, code);
    if(!head)
      head = node;
    else
      tail->next = node;
    tail = node;
  }
  if(count) {
    array = new user*[count];
    unsigned i = 0;
    for(user* node = head; node; node = node->next)
      array[i++] = node;
  }
  return "";
}

static int user_cmp(const void* ptra, const void* ptrb)
{
  const user* a = *(const user**)ptra;
  const user* b = *(const user**)ptrb;
  int res = a->name != b->name;
  return res;
}

void show_domain(int fd)
{
  unsigned count = 0;
  user** users;
  mystring msg = load_users(fd, count, users);
  if(count == 0) {
    fout << p_pre << "This domain contains no users.</p>\n";
    return;
  }

  if(!!msg) {
    fout << p_pre << "<samp>" << msg << "</samp></p>\n";
    return;
  }

  qsort(users, count, sizeof users[0], user_cmp);
  
  fout << p_pre << "This domain contains " << count << " users:</p>\n";
  fout << "<table border=1 align=\"" << table_align << "\">\n"
    "<tr><td><u>Username</u></td>"
    "<td><u>Mailbox</u></td>"
    "<td><u>Forwards</u></td></tr>\n";
  for(unsigned i = 0; i < count; i++)
    show_user(users[i]);
  fout << "</table>\n";
}

void setup_format(const CGIArgs& args)
{
  p_pre = "<p align=\"" + args.get("p-align", "left") + "\">";

  cell_pre = args["cell-pre"];
  cell_post = args["cell-post"];
  
  cell_pre = "<td align=\"" + args.get("cell-align", "left") + "\">"
    + cell_pre;
  cell_post = cell_post + "</td>";

  userlink = args["userlink"];

  table_align = args.get("table-align", "left");
}

CGI_MAIN 
{
  mystring body_flags = args["body-flags"];
  mystring title_pre = args.get("title-pre", "<h1>");
  mystring title_post = args.get("title-post", "</h1>");

  setup_format(args);

  domain = vdomain;
  server_call call("listdomain", vdomain, password);
  response resp = call.call();
  if(!resp)
    error(resp.msg);
  else {
    content_type("text/html");
    fout << "<html>"
      "<head>"
      "<title>Listing of Virtual Domain '" << vdomain << "'</title>"
      "</head>\n"
      "<body " << body_flags << ">\n" <<
      p_pre << title_pre << "Listing of Virtual Domain "
      "'" << vdomain << "'" << title_post << "</p>"
      "<hr>\n";
    show_domain(call.socket());
    const mystring referer = getenv("HTTP_REFERER");
    if(!!referer)
      fout << p_pre << "<a href=\"" << referer << "\">Back</a></p>\n";
    fout << "</body>\n"
      "</html>\n";
  }
}
