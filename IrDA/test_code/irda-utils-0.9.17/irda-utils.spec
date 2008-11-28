Name: irda-utils
Summary: Utilities for infrared communication between devices
Version: 0.9.14
Release: 1mdk
Source0: ftp://irda.sourceforge.net/pub/irda/irda-utils/%{name}-%{version}.tar.bz2
Group: System/Servers
URL: http://irda.sourceforge.net/
BuildRoot: %{_tmppath}/%{name}-buildroot
Copyright: GPL
Prefix: %{_prefix}

%description
IrDA(TM) (Infrared Data Association) is an industry standard for
wireless, infrared communication between devices. IrDA speeds range
from 9600 bps to 4 Mbps, and IrDA can be used by many modern devices
including laptops, LAN adapters, PDAs, printers, and mobile phones.

The Linux-IrDA project is a GPL'd implementation, written from
scratch, of the IrDA protocols. Supported IrDA protocols include
IrLAP, IrLMP, IrIAP, IrTTP, IrLPT, IrLAN, IrCOMM and IrOBEX.

The irda-utils package contains a collection of programs that enable
the use of IrDA protocols. Most IrDA features are implemented in the
kernel, so IrDA support must be enabled in the kernel before any IrDA
tools or programs can be used. Some configuration outside the kernel
is required, however, and some IrDA features, like IrOBEX, are
actually implemented outside the kernel.

%prep
rm -rf $RPM_BUILD_ROOT

%setup -q

%build
make all RPM_BUILD_ROOT="$RPM_BUILD_ROOT" RPM_OPT_FLAGS="$RPM_OPT_FLAGS" ROOT="$RPM_BUILD_ROOT"

%install
make install RPM_OPT_FLAGS="$RPM_OPT_FLAGS" ROOT="$RPM_BUILD_ROOT"

for dir in irattach irdadump irdaping tekram; do
    cp $dir/README $dir/README.$dir
done

%clean
rm -rf $RPM_BUILD_ROOT $RPM_BUILD_DIR/file.list.%{name}

%files
%defattr(-,root,root,0755)
%doc README
%doc irattach/README.irattach
%doc irdadump/README.irdadump
%doc irdaping/README.irdaping
%doc tekram/README.tekram
/usr/sbin/irattach
/usr/sbin/irdaping
/usr/sbin/dongle_attach
/usr/sbin/findchip
/usr/bin/irdadump
/usr/bin/irpsion5
/usr/bin/irkbd
%config(noreplace) /etc/rc.d/init.d/irda
%config(noreplace) /etc/sysconfig/irda
%config(noreplace) /etc/sysconfig/network-scripts/ifcfg-irlan0

%post
/sbin/chkconfig --add irda

%preun

if [ $1 = 0 ]; then
    /sbin/chkconfig --del irda
fi

exit 0

%changelog
* Sun Nov 19 2000 Dag Brattli <dag@brattli.net> 0.9.13-1mdk
- 0.9.13
- Changed config scripts (now that irmanager is gone)
- Removed irmanager (not needed anymore)

* Wed Jan 19 2000 Ian Soboroff <ian@cs.umbc.edu>
- 0.9.8
- Added findchip to package
- Added READMEs for the various utilities to the doc directory

* Wed Nov 10 1999 Dag Brattli <dagb@cs.uit.no>
- 0.9.5
- Some fixes to irattach, so it works with the latest kernels and patches
- Removed OBEX which will now become its own distribution
- Removed irdadump-X11 which will be replaced with a GNOME version

* Wed Sep 8 1999 Bernhard Rosenkraenzer <bero@linux-mandrake.com>
- 0.9.4
- include new stuff (palm3, psion, obex_tcp, ...)
- various fixes

* Tue Sep 7 1999 Bernhard Rosenkraenzer <bero@linux-mandrake.com>
- Fix .spec bug

* Tue Sep 7 1999 Bernhard Rosenkraenzer <bero@linux-mandrake.com>
- add README to %doc
- compile gnobex, now in irda-utils-X11

* Tue Sep 7 1999 Bernhard Rosenkraenzer <bero@linux-mandrake.com>
- initial RPM:
  - handle RPM_OPT_FLAGS and RPM_BUILD_ROOT
  - fix build
  - split in normal and X11 packages

