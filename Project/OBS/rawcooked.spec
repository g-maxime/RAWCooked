# Copyright (c) 2018 info@mediaarea.net
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.

# norootforbuild

%global rawcooked_version		26.11

%if 0%{?suse_version} > 1599 || 0%{?mageia} || 0%{?rhel} > 9 || 0%{?fedora_version}
%global qt6_gui 1
%else
%global qt6_gui 0
%endif

# The Qt GUI is not built for RHEL-based distributions older than RHEL 9.
%if 0%{?rhel} && 0%{?rhel} < 9
%global build_gui 0
%else
%global build_gui 1
%endif

Name:			rawcooked
Version:		%rawcooked_version
Release:		1
Summary:		Encodes RAW A/V data while permitting reversibility (CLI)
Group:			Productivity/Multimedia/Other
License:		BSD-2-Clause
URL:			https://mediaarea.net/RAWcooked
Packager:		Jerome Martinez <info@mediaarea.net>
Source0:		rawcooked_%{version}.tar.xz
Prefix:			%{_prefix}
BuildRoot:		%{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:		gcc-c++
BuildRequires:		pkgconfig
BuildRequires:		automake
BuildRequires:		autoconf
BuildRequires:		libtool
BuildRequires:		dos2unix

%if 0%{?rhel} >= 8
BuildRequires:		alternatives
%endif

%description
RAWcooked CLI (Command Line Interface)

RAWcooked provides this service:

Encodes RAW audio-visual data while permitting reversibility

%if %{build_gui}
%package gui
Summary:		Encodes RAW A/V data while permitting reversibility (GUI)
Group:			Productivity/Multimedia/Other

%if %{qt6_gui}
BuildRequires:		pkgconfig(Qt6Widgets)
BuildRequires:		pkgconfig(Qt6Svg)
%else
BuildRequires:		pkgconfig(Qt5Widgets)
BuildRequires:		pkgconfig(Qt5Svg)
%endif

%if 0%{?suse_version}
BuildRequires:		update-desktop-files
%endif

%description gui
RAWcooked GUI (Graphical User Interface)

RAWcooked provides this service:

Encodes RAW audio-visual data while permitting reversibility

This package contains the graphical user interface.
%endif

%prep
%setup -q -n rawcooked
dos2unix	*.txt
%__chmod 644	*.html *.txt

%build
export CFLAGS="-g $RPM_OPT_FLAGS"
export CXXFLAGS="-g $RPM_OPT_FLAGS"

# build CLI
pushd Project/GNU/CLI
	%__chmod +x autogen autogen.sh
	./autogen.sh
	%configure
	make %{?jobs:-j%{jobs}}
popd

%if %{build_gui}
# build GUI
pushd Project/Qt
	sh prepare rawcooked-gui.pro
	make %{?jobs:-j%{jobs}}
popd
%endif

%install
pushd Project/GNU/CLI
	make install DESTDIR=%{buildroot}
popd

%if %{build_gui}
pushd Project/Qt
	make install INSTALL_ROOT=%{buildroot}
popd

# icon
%__install -dm 755 %{buildroot}%{_datadir}/icons/hicolor/128x128/apps
%__install -m 644 Source/Resource/Image/Icon.png \
	%{buildroot}%{_datadir}/icons/hicolor/128x128/apps/rawcooked.png
%__install -dm 755 %{buildroot}%{_datadir}/pixmaps
%__install -m 644 Source/Resource/Image/Icon.png \
	%{buildroot}%{_datadir}/pixmaps/rawcooked.png

# menu entry
%__install -dm 755 %{buildroot}%{_datadir}/applications
%__install -m 644 Project/Qt/rawcooked-gui.desktop \
	%{buildroot}%{_datadir}/applications
%if 0%{?suse_version}
	%suse_update_desktop_file -n rawcooked-gui AudioVideo AudioVideoEditing
%endif

%__install -dm 755 %{buildroot}%{_datadir}/apps/konqueror/servicemenus
%__install -m 644 Project/Qt/rawcooked-gui.kde3.desktop \
	%{buildroot}%{_datadir}/apps/konqueror/servicemenus/rawcooked-gui.desktop

%__install -dm 755 %{buildroot}%{_datadir}/kde4/services/ServiceMenus
%__install -m 644 Project/Qt/rawcooked-gui.kde4.desktop \
	%{buildroot}%{_datadir}/kde4/services/ServiceMenus/rawcooked-gui.desktop

%__install -dm 755 %{buildroot}%{_datadir}/kservices5/ServiceMenus
%__install -m 644 Project/Qt/rawcooked-gui.kde4.desktop \
	%{buildroot}%{_datadir}/kservices5/ServiceMenus/rawcooked-gui.desktop

%if 0%{?fedora_version}
%__install -dm 755 %{buildroot}%{_datadir}/metainfo
%__install -m 644 Project/Qt/rawcooked-gui.metainfo.xml \
	%{buildroot}%{_datadir}/metainfo/rawcooked-gui.metainfo.xml
%else
%__install -dm 755 %{buildroot}%{_datadir}/appdata
%__install -m 644 Project/Qt/rawcooked-gui.metainfo.xml \
	%{buildroot}%{_datadir}/appdata/rawcooked-gui.appdata.xml
%endif
%endif

%clean
[ -d "%{buildroot}" -a "%{buildroot}" != "" ] && %__rm -rf "%{buildroot}"

%files
%defattr(-,root,root,-)
%doc License.html History_CLI.txt
%{_bindir}/rawcooked
%{_mandir}/man1/rawcooked.*

%if %{build_gui}
%files gui
%defattr(-,root,root,-)
%doc License.html History_CLI.txt
%{_bindir}/rawcooked-gui
%{_datadir}/applications/*.desktop
%{_datadir}/pixmaps/*.png
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/128x128
%dir %{_datadir}/icons/hicolor/128x128/apps
%{_datadir}/icons/hicolor/128x128/apps/*.png
%dir %{_datadir}/apps
%dir %{_datadir}/apps/konqueror
%dir %{_datadir}/apps/konqueror/servicemenus
%{_datadir}/apps/konqueror/servicemenus/*.desktop
%dir %{_datadir}/kde4
%dir %{_datadir}/kde4/services
%dir %{_datadir}/kde4/services/ServiceMenus
%{_datadir}/kde4/services/ServiceMenus/*.desktop
%dir %{_datadir}/kservices5
%dir %{_datadir}/kservices5/ServiceMenus
%{_datadir}/kservices5/ServiceMenus/*.desktop
%if 0%{?fedora_version}
%dir %{_datadir}/metainfo
%{_datadir}/metainfo/*.xml
%else
%dir %{_datadir}/appdata
%{_datadir}/appdata/*.xml
%endif
%endif

%changelog
