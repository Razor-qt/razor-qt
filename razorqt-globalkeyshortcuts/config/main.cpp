#include <razorqt/razorapplication.h>
#include "shortcuteditor.h"

int main(int argc, char *argv[])
{
    RazorApplication a(argc, argv);
    ShortcutEditor w;
    w.show();

    return a.exec();
}
