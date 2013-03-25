[LightDM](http://www.freedesktop.org/wiki/Software/LightDM) is new 
[Display Manager](http://en.wikipedia.org/wiki/X_display_manager_%28program_type%29) (DM) which seems to be widely adopted across distributions. It is not required to use LightDM at all Razor-qt will work out of the box from most displaymanagers

### How to enable razor's lightdm greeter

Update or insert in(to) your `/etc/lightdm/lightdm.conf`, in the `SeatDefaults` section, this line:

    greeter-session=lightdm-razor-greeter

### Configure lightdm-razor-greeter

The file `/etc/lightdm/lightdm-razor-greeter.conf` allows for a few configurations of lightdm-razor-greeter (background-image, positioning of loginform). The configuration options are documented in that file.

### OpenSUSE Specific Notes

To enable LightDM as a DM instead of kdm/gdm/xdm set appropriate value in the `DISPLAYMANAGER` variable with _YaST_ for it (module _Sysconfig Editor_) or edit `/etc/sysconfig/displaymanager` manually.

    DISPLAYMANAGER="lightdm"

Then restart the `rcxdm` service (or your computer).

### Arch Linux specific notes

To install lightdm, do

    sudo pacman -S lightdm

To set lightdm as the displaymanager do (assuming you use the default init system, systemd):

    sudo systemctl enable lightdm.service

If you already have an displaymanager installed, you will need to disable that first, e.g:

    sudo systemctl disable xdm