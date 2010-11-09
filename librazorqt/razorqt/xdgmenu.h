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


class Xdgmenu : public QObject
{
    Q_OBJECT
public:
    Xdgmenu(Xdgenv* _env, Xdgiconthememanager* _iconman);
    Xdgmenu();
    ~Xdgmenu();
    QVector<Xdgdesktopfile*>* get_xdgfilelist();
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
    Xdgmenucross mainMenu;
    void parseFiles(QString _dir);
    Xdgreader* menuReader;
    QVector<Xdgdesktopfile*> filelist;
    QMap <QString, QString> dirMap;
    QVector<Xdgdesktopfile*> dirList;
    Xdgenv* menuEnv;
    Xdgiconthememanager* menuIcon;
    QStringList pathList;
    QStringList badNames;

    //QStringList fileList;
};



#endif
