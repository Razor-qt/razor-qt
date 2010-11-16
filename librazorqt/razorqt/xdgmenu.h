#ifndef XDGMENU_H
#define XDGMENU_H


#include "defs.h"
#include "xdgreader.h"
#include "xdgdesktopfile.h"
#include "xdgenv.h"
#include "xdgmenucross.h"
#include "xdgiconthememanager.h"


//2010 Chris "VdoP" Regali


/**
 * @file xdgmenu.h
 * @brief Declares the class "Xdgmenu"
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */

/**
 * @brief Handles the menu on an abstract lvl
*/


class XdgMenu : public QObject
{
    Q_OBJECT
public:
    XdgMenu(XdgEnv* _env, XdgIconThemeManager* _iconman);
    XdgMenu();
    ~XdgMenu();
    QVector<XdgDesktopFile*>* get_xdgfilelist();
    void cleanList();

    //these should be called in this order!
    bool readMenufile();
    void getFilelist();
    void feedTree();
    QMenu* get_QMenus();
    void assignIcons();
    void getDirlist();
public slots:
    void runCmd(QAction* _action);

private:

    QMenu* qMenu;
    QList<QMenu*> qMenulist;
    XdgMenuCross mainMenu;
    void parseFiles(QString _dir);
    XdgReader* menuReader;
    QVector<XdgDesktopFile*> filelist;
    QMap <QString, QString> dirMap;
    QVector<XdgDesktopFile*> dirList;
    XdgEnv* menuEnv;
    XdgIconThemeManager* menuIcon;
    QStringList pathList;
    QStringList badNames;

    //QStringList fileList;
};



#endif
