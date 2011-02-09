#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include "appswitcher.h"


namespace RazorAppSwitcher
{

/*! \brief This inherits QApplication and therefore does our X11 event-handling
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int & argc, char **argv);
    ~Application();

protected:
    bool x11EventFilter(XEvent * e);

private:
    RazorAppSwitcher::AppSwitcher * m_as;
};

}; // namespace


#endif
