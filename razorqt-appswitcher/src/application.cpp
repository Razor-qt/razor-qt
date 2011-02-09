#include "razorqt/readsettings.h"
#include "application.h"


RazorAppSwitcher::Application::Application(int & argc, char ** argv)
    : QApplication(argc, argv),
      m_as(0)
{
    ReadSettings cfg("razor");
    ReadTheme theme(cfg.settings()->value("theme", "").toString());
    setStyleSheet(theme.qss());

    m_as = new RazorAppSwitcher::AppSwitcher();
}

RazorAppSwitcher::Application::~Application()
{
    delete m_as;
    m_as = 0;
}

bool RazorAppSwitcher::Application::x11EventFilter(XEvent * e)
{
    return m_as->handleEvent(e);
}