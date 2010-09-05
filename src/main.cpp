////////////////////////////////////////
//  File      : main.cpp              //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "defs.h"
#include "antico.h"

////////////////////////////////////////
 void myMessageOutput(QtMsgType type, const char *msg)
 {
   QFile logfile ("/var/log/antico.log");
   QTextStream stream(&logfile);
     switch (type) {
     case QtDebugMsg:
         stream <<"Debug: "<< msg << "\n";
	 
         break;
     case QtWarningMsg:
         stream <<"Warning: "<< msg << "\n";
	 break;
     case QtCriticalMsg:
         stream <<"Critical: "<< msg << "\n";
	 break;
     case QtFatalMsg:
          stream <<"FUUUU: "<< msg << "\n";
	abort();
     }
     logfile.close();
 }


int main(int argc, char **argv)
{
    //qInstallMsgHandler(myMessageOutput);

    Razor a(argc, argv);
    XSelectInput(QX11Info::display(), QX11Info::appRootWindow(QX11Info::appScreen()), KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                 KeymapStateMask | ButtonMotionMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask | VisibilityChangeMask |
                 ExposureMask | StructureNotifyMask | SubstructureRedirectMask | SubstructureNotifyMask);

    XClearWindow(QX11Info::display(), QX11Info::appRootWindow(QX11Info::appScreen()));
    XSync(QX11Info::display(), False);
    
    QTranslator translator;
    qDebug() << "Language:" << QLocale::system().name();
    translator.load(QLocale::system().name(), QCoreApplication::applicationDirPath() + "/language/");
    a.installTranslator(&translator);
    
    qDebug() << "GUI creation...";
    a.create_gui();
    
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}

