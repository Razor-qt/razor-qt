# norootforbuild

%define have_polkit 0

%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version}
	%define breq qt4-devel >= 4.6.0, libXcomposite-devel
%if 0%{?fedora}
BuildRequires:	polkit-qt-devel
%define have_polkit 1
%endif
	%define qmake /usr/bin/qmake-qt4
	%define lrelease /usr/bin/lrelease-qt4
	%define uic /usr/bin/uic-qt4
	%define qtxdglib libqtxdg
	%define qtxdglibdevel libqtxdg-devel
%endif
%if 0%{?mdkversion}
	%define breq libqt4-devel >= 4.6.0, qt4-linguist, libxcomposite-devel
	%define qmake /usr/lib/qt4/bin/qmake
	%define lrelease /usr/lib/qt4/bin/lrelease
	%define uic /usr/lib/qt4/bin/uic
	%define qtxdglib libqtxdg
	%define qtxdglibdevel libqtxdg-devel
%endif
%if 0%{?suse_version}
	%define breq libqt4-devel >= 4.6.0, update-desktop-files, fdupes
	%define qmake /usr/bin/qmake
	%define lrelease /usr/bin/lrelease
	%define uic /usr/bin/uic
	%define qtxdglib libqtxdg0
	%define qtxdglibdevel libqtxdg-devel
%if 0%{?sles_version} == 0 && 0%{suse_version} >= 1210
BuildRequires:  libpolkit-qt-1-devel
%define have_polkit 1
%endif
%endif


Name:		razorqt
Version:	0.4.1
Release:	0
License:	GPL
Source:		razor-qt-%{version}.tar.bz2
Group:		System/GUI/Other
Summary:	Razor a lightweight desktop toolbox
Vendor:		Razor-qt team
Url:		http://razor-qt.org
BuildRequires:	gcc-c++, cmake, make, file-devel, %{breq}, doxygen
BuildRoot:	%{_tmppath}/%{name}-%{version}-build
Requires:	%{name}-desktop = %{version}, %{name}-panel = %{version}, %{name}-session = %{version}
Requires:	%{name}-appswitcher = %{version}, %{name}-runner = %{version}
Requires:	%{name}-config = %{version}, %{name}-data = %{version}
Requires:	%{name}-power = %{version}
Requires:   %{qtxdglib} = %{version}
%if 0%{?have_polkit} == 1
Requires:	%{name}-policykit-agent = %{version}
%endif
Prefix:		/usr


#Recommends:	qterminal, juffed, ptbatterysystemtray, qlipper, qxkb, qasmixer, screengrab


%package	devel
Summary:	RazorQt development package
Url:            http://razor-qt.org
Group:		Development/Libraries/C and C++
Requires:	%{name}-libraries
Obsoletes:	razorqt-x11info <= %{version}

%package	libraries
Summary:	RazorQt shared library
Url:            http://razor-qt.org
Group:		System/Libraries
Requires:	upower
# it's quite a big dependency - but there are manu bugreports with (I cannot see any
# icons...) and at least one icon theme has to be installed
Requires:	oxygen-icon-theme
# names before 0.4
Obsoletes:	razorqt-libs <= %{version}, librazorqt0 <= %{version}

%package	-n %{qtxdglib}
Url:            http://razor-qt.org
Summary:	QtXdg library
Group:		System/Libraries

%package	-n %{qtxdglibdevel}
Url:            http://razor-qt.org
Summary:	Development files for QtXdg library
Group:		Development/Libraries/C and C++
Requires:	%{qtxdglib} = %{version}

%package	appswitcher
Url:            http://razor-qt.org
Summary:	RazorQt application switcher
Group:		System/X11/Utilities
Requires:	%{name}-data

%package	desktop
Url:            http://razor-qt.org
Summary:	RazorQt desktop
Group:		System/GUI/Other
Requires:	%{name}-data

%package	panel
Url:            http://razor-qt.org
Summary:	RazorQt panel
Group:		System/GUI/Other
Requires:	%{name}-data xscreensaver

%if 0%{?have_polkit} == 1
%package	policykit-agent
Url:		http://razor-qt.org
Summary:	RazorQt policykit agent
Group:          System/GUI/Other
%endif

%package	power
Url:		http://razor-qt.org
Summary:	RazorQt power management apps
Group:		System/GUI/Other
Requires:	%{name}-data

%package	data
Url:            http://razor-qt.org
Summary:        RazorQt resources and shared data
Group:          System/GUI/Other
Obsoletes:	%{name}-resources <= %{version}

%package	runner
Url:            http://razor-qt.org
Summary:        RazorQt runner application
Group:          System/GUI/Other
Requires:	%{name}-data

%package	session
Url:            http://razor-qt.org
Summary:	RazorQt session
Group:		System/GUI/Other
Requires:	%{name}-data, openbox
Obsoletes:	%{name}-openbox <= %{version}, %{name}-wm <= %{version}

%package	config
Url:            http://razor-qt.org
Summary:	RazorQt config tools
Group:		System/GUI/Other

%description
Description:	A metapackage for Razor-qt DE. It will install all components.

%description	libraries
Description:	Base runtime libraries for Razor-qt DE,

%description	-n %{qtxdglib}
Description:	Implementation of XDG standards in Qt.

%description    -n %{qtxdglibdevel}
Description:	A development environment for qtxdg.

%description	devel
Description:	A development environment for Razor-qt.

%description	appswitcher
Description:	A alt+tab appliaction switcher for window managers where it is not available natively.

%description	desktop
Description:	Razor-qt desktop implementation.

%description	panel
Description:	Razor-qt panel and its plugins.

%if 0%{?have_polkit} == 1
%description	policykit-agent
Description:	A lightweight PolicyKit agent primarily writen for Razor-qt DE. But it can be used standalone as well.
%endif

%description    power
Description:	Power management apps for Razor-qt DE

%description	data
Description:	Shared data for Razor-qt. For example: themes, images, etc.

%description	runner
Description:	Quick launch/runner application for Razor-qt.

%description	config
Description:	Razor-qt configuration GUI tools.

%description	session
Description:	Razor-qt session environment.

%prep
%setup -q -n razor-qt-%{version}

%build
_libsuffix=$(echo %{_lib} | cut -b4-)
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLIB_SUFFIX=$_libsuffix
make

%install
%{__rm} -rf %{buildroot}
%{makeinstall} DESTDIR=%{buildroot}

%if 0%{?suse_version}
%suse_update_desktop_file -r razor-autosuspend Utility DesktopUtility
%fdupes -s "%{buildroot}" 
%endif


%clean
%{__rm} -rf %{buildroot}

%post	libraries
ldconfig

%post   -n %{qtxdglib}
ldconfig

%postun	libraries
ldconfig

%postun -n %{qtxdglib}
ldconfig

%files
%defattr(-,root,root,-)
%doc README

%files	libraries
%defattr(-,root,root,-)
%{_libdir}/librazor*.so.*
%{_datadir}/librazorqt

%files  -n %{qtxdglib}
%defattr(-,root,root,-)
%{_libdir}/libqtxdg.so.*
%{_datadir}/qtxdg

%files  -n %{qtxdglibdevel}
%defattr(-,root,root,-)
%{_libdir}/libqtxdg.so
%{_libdir}/pkgconfig/qtxdg.pc
%{_includedir}/qtxdg/

%files	devel
%defattr(-,root,root,-)
%{_libdir}/librazor*.so
%{_libdir}/pkgconfig/razor*.pc
%{_includedir}/razor*/
%{_bindir}/razor-x11info

%files	appswitcher
%defattr(-,root,root,-)
%{_bindir}/razor-appswitcher

%files	desktop
%defattr(-,root,root,-)
%{_bindir}/razor-desktop
%{_bindir}/razor-config-desktop
%{_libdir}/razor-desktop
%{_datadir}/applications/razor-config-desktop.desktop
%dir %{_datadir}/razor
%{_datadir}/razor/desktop.conf
%{_datadir}/razor/razor-desktop/

%files	panel
%defattr(-,root,root,-)
%{_bindir}/razor-panel
%{_libdir}/razor-panel/
%{_datadir}/razor/razor-panel/

%if 0%{?have_polkit} == 1
%files	policykit-agent
%defattr(-,root,root,-)
%{_bindir}/razor-policykit-agent
%endif

%files	power
%defattr(-,root,root,-)
%{_bindir}/razor-power
%{_bindir}/razor-autosuspend
%{_datadir}/razor/razor-power/
%{_datadir}/applications/razor-autosuspend.desktop
%{_datadir}/applications/razor-power.desktop
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps/
%{_datadir}/icons/hicolor/scalable/apps/razor-autosuspend.svg

%files	runner
%defattr(-,root,root,-)
%{_bindir}/razor-runner
%{_datadir}/razor/razor-runner/

%files  config
%defattr(-,root,root,-)
%{_bindir}/razor-config
%{_bindir}/razor-config-mouse
%{_bindir}/razor-config-appearance
%{_datadir}/applications/razor-config.desktop
%{_datadir}/applications/razor-config-mouse.desktop
%{_datadir}/applications/razor-config-appearance.desktop
%{_datadir}/razor/razor-config/

%files	session
%defattr(-,root,root,-)
%{_bindir}/razor-session
%{_bindir}/razor-config-session
%{_bindir}/startrazor
%{_datadir}/xsessions/razor*.desktop
%dir %{_datadir}/apps/
%dir %{_datadir}/apps/kdm
%dir %{_datadir}/apps/kdm/sessions
%{_datadir}/apps/kdm/sessions/razor*.desktop
%{_datadir}/applications/razor-config-session.desktop
%{_datadir}/razor/session*.conf
%{_datadir}/razor/razor-session/
/etc/xdg/autostart/razor*.desktop

%files	data
%defattr(-,root,root,-)
%{_datadir}/razor/razor.conf
%{_datadir}/razor/graphics/
%{_datadir}/razor/themes/
%config /etc/xdg/menus/razor-applications.menu
%dir /etc/xdg/menus
%{_datadir}/desktop-directories/razor*
%dir %{_datadir}/desktop-directories
# temp files - it will be removed when it becomes part of upstream
%{_libdir}/razor-xdg-tools


%changelog
* Wed Jan 26 2012 Petr Vanek <petr@scribus.info> 0.4.1
- preparing 0.4.1 release

* Thu Sep 15 2011 Petr Vanek <petr@scribus.info> 0.4.0
- version bump: 0.4.0

* Fri Jul 22 2011 TI_Eugene <ti.eugene@gmail.com> 0.3.99
- Next version
- razor-runner added
- razor-x11info added

* Fri May 12 2011 Petr Vanek <petr@scribus.info> 0.3.0
- version bump. New packages structure

* Tue Feb 15 2011 TI_Eugene <ti.eugene@gmail.com> 0.2-206
- cleaning up spec
- openbox and eggwm session subpackages

* Mon Feb 07 2011 Petr Vanek <petr@scribus.info> 0.2-206
- suse fixes. Resources are split into library and resources package

* Sat Jan 29 2011 TI_Eugene <ti.eugene@gmail.com> 0.2-206
- appswitcher added

* Thu Jan 06 2011 TI_Eugene <ti.eugene@gmail.com> 0.2-190
- Next build

* Wed Mar 04 2009 TI_Eugene <ti.eugene@gmail.com> 0.1
- Initital build in OBS
