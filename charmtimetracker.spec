Name:           charmtimetracker
Version:        1.12.0rc2
Release:        0
Summary:        Time Tracking Application
Source:         %{name}-%{version}.tar.gz
Url:            https://github.com/KDAB/Charm
Group:          Productivity/Other
License:        GPL-2.0+
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake extra-cmake-modules desktop-file-utils

%if %{defined suse_version}
BuildRequires:  update-desktop-files libqt5-qtbase-devel qtkeychain-qt5-devel
%endif

%if %{defined fedora}
BuildRequires:  qt5-qtbase-devel qtkeychain-qt5-devel
%endif

%if %{defined rhel}
BuildRequires:  qt5-qtbase-devel qtkeychain-qt5-devel
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
     Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>


%define debug_package %{nil}
%global __debug_install_post %{nil}

%prep
%setup -T -c
%{__tar} -zxf %{SOURCE0} --strip-components=1

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DCharm_VERSION=%{version}
%__make %{?_smp_mflags}

%install
%make_install

%if %{defined suse_version}
%suse_update_desktop_file -r charmtimetracker Utility DesktopUtility
%endif

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%dir %{_prefix}/share/doc/HTML
%dir %{_prefix}/share/doc/HTML/charmtimetracker
%{_prefix}/share/doc/HTML/charmtimetracker/*
%{_prefix}/share/icons/hicolor
%{_prefix}/share/applications/charmtimetracker.desktop
%{_prefix}/bin/charmtimetracker

%changelog
* Sat Jul 07 2018 Allen Winter <allen.winter@kdab.com> 1.12.0rc2
- 1.12.0 RC2
* Tue Nov 01 2016 Steffen Hansen <steffen.hansen@kdab.com> 1.11.4
- 1.11.4 release
* Tue Nov 01 2016 Steffen Hansen <steffen.hansen@kdab.com> 1.11.3
- 1.11.3 release
* Sat Apr 30 2016 Allen Winter <allen.winter@kdab.com> 1.11.1
- 1.11.1 release
* Mon Apr 25 2016 Allen Winter <allen.winter@kdab.com> 1.11.0
- 1.11.0 release
* Mon Oct 05 2015 Allen Winter <allen.winter@kdab.com> 1.10.0
- 1.10.0 release
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
