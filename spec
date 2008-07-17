Name: vmailmgr
Version: @VERSION@
Release: 1
Group: Utilities/System
URL: http://www.vmailmgr.org/
License: GPL
Source: http://www.vmailmgr.org/archive/%{PACKAGE_VERSION}/vmailmgr-%{PACKAGE_VERSION}.tar.gz
Summary: Simple virtualizing POP3 password interface
Packager: Bruce Guenter <bruce@untroubled.org>
Buildroot: %{_tmppath}/vmailmgr
Obsoletes: checkvpw

%description
Vmailmgr provides a virtualizing password-checking interface to
qmail-pop3d as well as both a delivery agent to automatically delivery
mail within a virtual domain and a set of tools to manage such a domain.

%package cgi
Summary: CGI applications for vmailmgr
Group: Utilities/System
Requires: vmailmgr-daemon = %{PACKAGE_VERSION}
%description cgi
This package contains CGI applications to allow web-based administration
of vmailmgr systems.

%package courier-imap
Summary: Vmailmgr authentication module for Courier IMAP
Requires: courier-imap
Group: Utilities/System
%description courier-imap
This package contains the vmailmgr authentication module to be used by
Courier IMAP.

%package daemon
Summary: Vmailmgr daemon for CGIs
Requires: supervise-scripts >= 2.2
Requires: ucspi-unix
Group: Utilities/System
%description daemon
This package contains the vmailmgrd daemon that provides virtual domain
manipulation services to support unprivileged clients like CGIs.

%package php
Summary: PHP include files
Group: Development/Libraries
%description php
This package contains the include files necessary to call VMailMgr
functions from PHP.

%package python
Summary: Python library for accessing VMailMgr
Group: Development/Libraries
Requires: python >= 1.5
%description python
This package contains the Python library code necessary to call VMailMgr

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" \
CXXFLAGS="$RPM_OPT_FLAGS" \
LDFLAGS="-s" \
./configure --prefix=/usr

%build
make all
make all

%install
rm -rf $RPM_BUILD_ROOT
for dir in var/service/vmailmgrd/log var/log/vmailmgrd \
	etc/rc.d/init.d etc/rc.d/rc{0,1,2,3,4,5,6}.d etc/vmailmgr
do
	mkdir -p $RPM_BUILD_ROOT/$dir
done
make	prefix=$RPM_BUILD_ROOT/usr \
	cgidir=$RPM_BUILD_ROOT/home/httpd/cgi-bin \
	pythonlibdir=$RPM_BUILD_ROOT/usr/lib/python1.5/vmailmgr \
	phpdir=$RPM_BUILD_ROOT/home/httpd/php \
	install-strip
install -m 755 scripts/vmailmgrd.init $RPM_BUILD_ROOT/etc/rc.d/init.d/vmailmgrd
install -m 755 scripts/vmailmgrd.run $RPM_BUILD_ROOT/var/service/vmailmgrd/run
install -m 755 scripts/vmailmgrd-log.run $RPM_BUILD_ROOT/var/service/vmailmgrd/log/run
pushd $RPM_BUILD_ROOT/etc/rc.d
ln -s ../init.d/vmailmgrd rc0.d/K35vmailmgrd
ln -s ../init.d/vmailmgrd rc1.d/K35vmailmgrd
ln -s ../init.d/vmailmgrd rc2.d/S65vmailmgrd
ln -s ../init.d/vmailmgrd rc3.d/S65vmailmgrd
ln -s ../init.d/vmailmgrd rc4.d/S65vmailmgrd
ln -s ../init.d/vmailmgrd rc5.d/S65vmailmgrd
ln -s ../init.d/vmailmgrd rc6.d/K35vmailmgrd
popd
pushd $RPM_BUILD_ROOT/etc/vmailmgr
echo users >user-dir
echo passwd >password-file
echo ./Maildir/ >default-maildir
echo maildir >maildir-arg-str
echo /var/service/vmailmgrd/socket >socket-file

%clean
rm -rf $RPM_BUILD_ROOT

%post
if [ $# -eq 2 -a -f /etc/vmailmgr.conf ]; then
  vconf2dir /etc/vmailmgr.conf /etc/vmailmgr
fi

%files
%defattr(-,root,root)
%doc AUTHORS ChangeLog COPYING NEWS TODO doc/ChangeLog* doc/YEAR2000
%doc doc/*.txt doc/*.html commands/*.html authenticate/*.html daemon/*.html
%doc scripts/autoresponder.sh
%dir /etc/vmailmgr
%config(missingok,noreplace) %verify(user,group,mode) /etc/vmailmgr/*
/usr/bin/*
/usr/man/man1/*
/usr/man/man7/*
/usr/man/man8/*

%files cgi
%defattr(-,root,root)
%doc cgi/*.html
/home/httpd/cgi-bin/listvdomain
/home/httpd/cgi-bin/vaddalias
/home/httpd/cgi-bin/vadduser
/home/httpd/cgi-bin/vchattr
/home/httpd/cgi-bin/vchforwards
/home/httpd/cgi-bin/vdeluser
/home/httpd/cgi-bin/vpasswd

%files courier-imap
%defattr(-,root,root)
/usr/lib/courier-imap/libexec/authlib/*

%files daemon
%defattr(-,root,root)
%config /etc/rc.d/init.d/vmailmgrd
%config /etc/rc.d/rc?.d/*vmailmgrd
/usr/sbin/vmailmgrd
%attr(1755,root,root) %dir /var/service/vmailmgrd
%dir /var/service/vmailmgrd/log
/var/service/vmailmgrd/log/run
/var/service/vmailmgrd/run
%attr(0700,root,root) /var/log/vmailmgrd

%files php
%defattr(-,root,root)
%doc php/vmail.features
/home/httpd/php/*

%files python
%defattr(-,root,root)
/usr/lib/python1.5/vmailmgr
