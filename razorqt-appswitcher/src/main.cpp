#include <QApplication>
#include "application.h"


/*! \mainpage razor-appswitcher, lightweight "alt+tab" application switcher

Some Window Managers (WM) do not provide well known feature how to change
active window with for example Alt+Tab key combination. In this case you
can use this Razor application (part of RazorQt project).

Using this Razor module is optional and it is not required in most cases
eg. in Openbox etc.
But you can find it useful in other minimalistic WMs
*/
int main(int argc, char **argv)
{
    RazorAppSwitcher::Application a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    return a.exec();
}
