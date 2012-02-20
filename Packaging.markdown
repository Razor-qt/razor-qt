**Note:** Package names should follow target distro guidelines. This documentation provides coordination frame to keep package naming as unified as possible.

**Note:** Package names in this document are marked as italic text. Example: _razorqt-desktop_

## Links to packaking environments

* RPM: [QtDesktop build service repository](https://build.opensuse.org/package/files?package=razor&project=home%3ATI_Eugene%3AQtDesktop)
* DEB: in progress
* eBuild: n/a

## Meta packages

* _razorqt_ - a "meta" package providing easy installation of all "end user" components.

## "end user" packages

Packages with appropriate source code repository content.

**Note:** in Open Build Service are dependencies for libraries calculated automatically so it's not required to list library dependencies for those packages (in OBS).

* _razorqt-appswitcher_
* _razorqt-config_
* _razorqt-desktop_
* _razorqt-panel_ 
* _razorqt-policykit-agent_
* _razorqt-power_
* _razorqt-runner_
* _razorqt-session_ - should use openbox as a dependency
* _razorqt-config_

## Libraries and various data files

* _razorqt-session_: including /etc/xdg/autostart/razor*.desktop

* _razorqt-data_: themes, graphics from razorqt-resources subdir. It should be used as a dependency for all packages in "end user" category
* _razorqt-devel_: includes and devel libs for _razorqt-libraries_ and razor-x11info binary
* _razorqt-libraries_: librazorqt, librazorqxt, librazormount, etc.

* libqtxdg - this library is packed in its special package because it's shared and used by 3rd party apps. So suggested names are something like:
 * _libqtxdg0_ (suse, debian)
 * _libqtxdg_ (fedora, mandriva)
* _libqtxdg-devel_ - development files for libqtxdg
