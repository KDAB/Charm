Name:           charmtimetracker
Version:        1.9.0
Release:        0
Summary:        Time Tracking Application
Source:         %{name}-%{version}.tar.gz
Url:            https://github.com/KDAB/Charm
Group:          Productivity/Other
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

%if %{defined suse_version}
BuildRequires:  libqt4-devel cmake update-desktop-files
Requires:       libqt4-sql-sqlite
%endif

%if %{defined fedora}
BuildRequires:  gcc-c++ qt-devel cmake desktop-file-utils
Requires:       qt4-sqlite
%endif

%if %{defined rhel}
BuildRequires:  gcc-c++ qt-devel cmake desktop-file-utils
Requires:       qt4-sqlite
%endif

%description
Charm is a program for OS X, Linux and Windows that helps to keep
track of time. It is built around two major ideas - tasks, and
events. Tasks are the things time is spend on, repeatedly. For
example, ironing laundry is a task. The laundry done for two hours on
last Tuesday is an event in that task. When doing laundry multiple
times, the events will be accumulated, and can later be printed in
activity reports or weekly time sheets. So in case laundry would be
done for three hours on Wednesday again, the activity report for the
"Ironing Laundry" task would list the event on tuesday, the event on
wednesday and a total of five hours.

Authors:
--------
     Mirko Boehm <mirko.boehm@kdab.com>

%prep
%setup -T -c
%{__tar} -zxf %{SOURCE0} --strip-components=1

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DCharm_VERSION=%{version}
%__make %{?_smp_mflags}

%install
%make_install

%if %{defined suse_version}
%suse_update_desktop_file charmtimetracker Utility TimeUtility
%endif

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_prefix}/share/doc/charmtimetracker
%{_prefix}/share/icons/hicolor
%{_prefix}/share/applications/charmtimetracker.desktop
%{_prefix}/bin/charmtimetracker

%changelog
* Thu Mar 26 2015 Allen Winter <allen.winter@kdab.com> 1.9.0
- 1.9.0 release
* Tue Jul 02 2013 Kevin Ottens <kevin.ottens@kdab.com> 1.8.0
- 1.8.0 release
* Fri Jul 27 2012 Frank Osterfeld <frank.osterfeld@kdab.com> 1.7.0
- 1.7.0 release
* Thu Feb 23 2012 Mike McQuaid <mike.mcquaid@kdab.com> 1.6.0
- 1.6.0 release
* Wed Apr 20 2011 Mike McQuaid <mike.mcquaid@kdab.com> 1.5.2
- Initial setup of 1.5.2 version (based on Kevin Ottens work).
