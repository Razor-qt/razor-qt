////////////////////////////////////////
//  File      : utils.cpp             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "utils.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// APPICON /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

Appicon::Appicon(QObject *parent) : QObject(parent)
{}

Appicon::~Appicon()
{}

QString Appicon::get_app_icon(const QString &icon) // select the application icon
{
    QString app_icon = icon;

    if (app_icon.startsWith('/')) // full icon path already set
    {
        return app_icon;
    }
    // remove the extension (.png/.xpm)
    if (app_icon.endsWith(".png"))
        app_icon.remove(".png");
    else if (app_icon.endsWith(".xpm"))
        app_icon.remove(".xpm");

    data_path = "/usr/share"; // search in default path directory
  
    QDirIterator pix_iter(data_path + "/pixmaps/", QDirIterator::Subdirectories);
    while (pix_iter.hasNext())
    {
        pix_iter.next(); // move to child directory
        QFileInfo pix_file(pix_iter.fileInfo());

        if (pix_file.baseName().compare(app_icon) == 0)
        {
            qDebug() << "File name:" << pix_file.fileName();
            app_icon = pix_file.absoluteFilePath();
            return app_icon;
        }
    }
    // else search in /icons directory
    QDirIterator icon_iter(data_path + "/icons/hicolor/32x32/apps/", QDirIterator::Subdirectories);
    while (icon_iter.hasNext())
    {
        icon_iter.next(); // move to child directory
        QFileInfo icon_file(icon_iter.fileInfo());

        if (icon_file.baseName().compare(app_icon) == 0)
        {
            qDebug() << "File name:" << icon_file.fileName();
            app_icon = icon_file.absoluteFilePath();
            return app_icon;
        }
    }

    // if not defined, set default application icon
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Other");
    app_icon = stl_path + style->value("application_pix").toString();
    style->endGroup(); // Other
    return app_icon; // if not defined, set default application icon
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// CATEGORYMENU ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

Categorymenu::Categorymenu(QWidget *parent) : QWidget(parent)
{
    read_settings();
    init();
}

Categorymenu::~Categorymenu()
{
    delete antico;
    delete audiovideo_menu;
    delete system_menu;
    delete development_menu;
    delete graphics_menu;
    delete network_menu;
    delete office_menu;
    delete utility_menu;
    delete favourite_menu;
}

void Categorymenu::init()
{
    audiovideo_menu = new QMenu(tr("AudioVideo"));
    audiovideo_menu->setIcon(QIcon(audiovideo_pix));
    system_menu = new QMenu(tr("System"));
    system_menu->setIcon(QIcon(system_pix));
    development_menu = new QMenu(tr("Development"));
    development_menu->setIcon(QIcon(devel_pix));
    graphics_menu = new QMenu(tr("Graphics"));
    graphics_menu->setIcon(QIcon(graphics_pix));
    network_menu = new QMenu(tr("Network"));
    network_menu->setIcon(QIcon(network_pix));
    office_menu = new QMenu(tr("Office"));
    office_menu->setIcon(QIcon(office_pix));
    utility_menu = new QMenu(tr("Utility"));
    utility_menu->setIcon(QIcon(utility_pix));
    favourite_menu=new QMenu(tr("Favourite"));
    favourite_menu->setIcon(QIcon(favourite_pix));
    

    connect(audiovideo_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(system_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(development_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(graphics_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(network_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(office_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(utility_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    connect(favourite_menu, SIGNAL(triggered(QAction *)), this, SLOT(run_menu(QAction *)));
    
}

void Categorymenu::read_settings()
{
    antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get category menu icons
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Launcher");
    utility_pix = stl_path + style->value("utility_pix").toString();
    office_pix = stl_path + style->value("office_pix").toString();
    network_pix = stl_path + style->value("network_pix").toString();
    graphics_pix = stl_path + style->value("graphics_pix").toString();
    devel_pix = stl_path + style->value("development_pix").toString();
    system_pix = stl_path + style->value("system_pix").toString();
    audiovideo_pix = stl_path + style->value("audiovideo_pix").toString();
    favourite_pix = stl_path + style->value("favourite_pix").toString();
    qDebug() << style->value("favourite_pix").toString();
    style->endGroup(); // Launcher
}

void Categorymenu::update_menu()
{
    cat_menu.insert("Office", office_menu); 
    cat_menu.insert("Network", network_menu);
    cat_menu.insert("Graphics", graphics_menu);
    cat_menu.insert("Development", development_menu);
    cat_menu.insert("System", system_menu);
    cat_menu.insert("AudioVideo", audiovideo_menu);
    cat_menu.insert("Utility", utility_menu);
    cat_menu.insert("Favourite", favourite_menu); //Do NOT want favourites in start menu too!
    
    // update Category menu
    utility_menu->clear();
    office_menu->clear();
    network_menu->clear();
    graphics_menu->clear();
    development_menu->clear();
    system_menu->clear();
    audiovideo_menu->clear();
    favourite_menu->clear();
    
    parse_desktop_file();

    /////////// SET BY USER ///////////
    antico->beginGroup("Launcher");
    antico->beginGroup("Category");

    QHashIterator<QString, QMenu *> cat_iter(cat_menu);
    while (cat_iter.hasNext())
    {
        cat_iter.next();
        antico->beginGroup(cat_iter.key()); // category name
        for (int i = 0; i < antico->childGroups().size(); i++)
        {
            antico->beginGroup(antico->childGroups().value(i)); // App name
            QString name = antico->value("name").toString();
            QString exec = antico->value("exec").toString();
            QString icon = antico->value("pix").toString();
            add_app(cat_iter.value(), name, exec, icon);
            antico->endGroup(); // App name
        }
        antico->endGroup();
    }
    antico->endGroup(); // Category
    antico->endGroup(); // Launcher

    /////////// READ FROM .DESKTOP FILE ///////////
    antico->beginGroup("Launcher-Desk");
    antico->beginGroup("Category");

    QHashIterator<QString, QMenu *> cat_desk_iter(cat_menu);
    while (cat_desk_iter.hasNext())
    {
        cat_desk_iter.next();
        antico->beginGroup(cat_desk_iter.key()); // category name
        for (int i = 0; i < antico->childGroups().size(); i++)
        {
            antico->beginGroup(antico->childGroups().value(i)); // App name
            QString name = antico->value("name").toString();
            QString exec = antico->value("exec").toString();
            QString icon = antico->value("pix").toString();
            add_app(cat_desk_iter.value(), name, exec, icon);
            antico->endGroup(); // App name
        }
        antico->endGroup();
    }
    antico->endGroup(); // Category
    antico->endGroup(); // Launcher-Desk
}

QList <QMenu *> Categorymenu::get_menus()
{
    QList <QMenu *> menu_list;
    menu_list.append(utility_menu);
    menu_list.append(office_menu);
    menu_list.append(network_menu);
    menu_list.append(graphics_menu);
    menu_list.append(development_menu);
    menu_list.append(system_menu);
    menu_list.append(audiovideo_menu);
    //uncomment the following if you want favourites in start menu and / or want to enable the deskbar
    //menu_list.append(favourite_menu);
    return menu_list;
}

void Categorymenu::add_app(QMenu *category_menu, const QString &name, const QString &exec, const QString &icon) // add application and icon on Category menu
{
    QAction *act = new QAction(QIcon(icon), name, category_menu);
    act->setData(exec);
    category_menu->addAction(act);
}

void Categorymenu::run_menu(QAction *act)
{
    QString cmd = act->data().toString(); // get Application name from menu
    qDebug() << "Command arguments on run:" << cmd_arguments;

    if (cmd_arguments.isEmpty())
        QProcess::startDetached(cmd); // start Application from Category menu
    else
        QProcess::startDetached(cmd, cmd_arguments); //start Application + arguments from Category menu

    clear_cmd_arguments();
}

void Categorymenu::set_cmd_arguments(const QString &arg) // add arguments after application name on run
{
    qDebug() << "Command arguments on run:" << arg;
    cmd_arguments << arg;
}

void Categorymenu::clear_cmd_arguments()
{
   cmd_arguments.clear();
}

void Categorymenu::update_style()
{
    read_settings();
    favourite_menu->setIcon(QIcon(favourite_pix));
    audiovideo_menu->setIcon(QIcon(audiovideo_pix));
    system_menu->setIcon(QIcon(system_pix));
    development_menu->setIcon(QIcon(devel_pix));
    graphics_menu->setIcon(QIcon(graphics_pix));
    network_menu->setIcon(QIcon(network_pix));
    office_menu->setIcon(QIcon(office_pix));
    utility_menu->setIcon(QIcon(utility_pix));
}

QMenu* Categorymenu::get_favmenu()
{
  return favourite_menu;
}

void Categorymenu::parse_desktop_file()
{
    antico->remove("Launcher-Desk"); // clear Launcher-Desk entry
    QList<QString> cat_list;
    cat_list.append("Office");
    cat_list.append("Network");
    cat_list.append("Graphics");
    cat_list.append("Development");
    cat_list.append("System");
    cat_list.append("AudioVideo");
    cat_list.append("Utility");
    cat_list.append("Favourites");

    QString lang = QLocale::system().name(); // (it_IT)
    QString country = lang.section('_', 0, 0); // (it_IT -> it)
    QString locale_name = QString("Name").append("[").append(country).append("]").append("="); // Name[it]
  
    data_path = "/usr/share"; // search in default path directory
  
    QDirIterator desktop_iter(data_path + "/applications/", QDirIterator::Subdirectories);

    while (desktop_iter.hasNext())
    {
        desktop_iter.next(); // move to child directory

        QFile desktop_file(desktop_iter.filePath());
        qDebug() << "Parsing file:" << desktop_iter.filePath();

        QString name;
        QString exec;
        QString icon_name;
        QString icon_path;
        QString categories;
        QString full_path;
        QString args;
        QStringList split_path;
        QString app_name;

        QString file_name = desktop_file.fileName();

        if (file_name == "." || file_name == "..")
            continue;

        if (!desktop_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Problem to read .desktop file:" << file_name;
            continue;
        }

        QTextStream stream(&desktop_file);
        QString line;

        ///////////// PARSE THE .DESKTOP FILE /////////////
        do
        {
            line = stream.readLine();

            if (! line.isNull() || ! line.isEmpty())
            {
                if (line.startsWith("Categories="))
                {
                    QString new_line = line.remove("Categories=");
                    QStringList split = new_line.split(";"); // get the single categories

                    for (int i = 0; i < split.size(); ++i)
                    {
                        if (cat_list.contains(split.at(i)))
                            categories = split.at(i); // get the category
                    }
                    qDebug() << "Categories:" << categories;
                }
                if (line.startsWith("Name="))
                {
                    name = line.remove("Name="); // default name
                    qDebug() << "Name:" << name;
                }
                if (line.startsWith(locale_name))
                {
                    name = line.remove(locale_name); // locale name
                    qDebug() << "Locale Name:" << name;
                }
                if (line.startsWith("Exec="))
                {
                    exec = line.remove("Exec=");
                    if (! exec.startsWith('/')) // full path not already set
                        exec.prepend("/usr/bin/");
                    qDebug() << "Exec:" << exec;
                }
                if (line.startsWith("Icon="))
                {
                    icon_name = line.remove("Icon=");
                    qDebug() << "Icon:" << icon_name;
                }
            }
        }
        while (!line.isNull());

        if (! exec.isEmpty() && ! categories.isEmpty() && ! name.isEmpty() && ! icon_name.isEmpty())
        {
            full_path = exec.section(' ', 0, 0); // remove eventually args after app name (/usr/bin/vlc %U -> /usr/bin/vlc)
            qDebug() << "full_path:" << full_path;
            args = exec.section(' ', 1, 1); // save args after app name with space (/usr/bin/vlc %U -> _%U)

            if (args.startsWith('-')) // leave only args i.e. for OpenOffice (soffice-writer)
                qDebug() << "args:" << args;
            else
                args.clear();

            split_path = full_path.split('/', QString::SkipEmptyParts); // (/usr/bin/vlc -> usr, bin, vlc)
            qDebug() << "split_path:" << split_path;
            app_name = split_path.last(); // (usr, bin, vlc -> vlc)
            qDebug() << "App_name:" << app_name;
            qDebug() << "---------------------------------------------";

            Appicon *app_ico = new Appicon(); // get application icon path
            icon_path = app_ico->get_app_icon(icon_name);

            if (! cat_list.contains(categories))
                categories = "Utility";

            antico->beginGroup("Launcher-Desk");
            antico->beginGroup("Category");
            antico->beginGroup(categories);
            antico->beginGroup(app_name + args); // for i.e. soffice-writer

            antico->setValue("name", name);
            antico->setValue("exec", full_path);
            antico->setValue("pix", icon_path);

            antico->endGroup(); // App name
            antico->endGroup(); // Category type
            antico->endGroup(); // Category group
            antico->endGroup(); // Launcher-Desk
            qDebug() << "---------------------------------------------";
        }
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILEICON ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

Fileicon::Fileicon() : QFileIconProvider()
{
    read_settings();
    // match the files extension with icons
    devel << "h" << "cpp" << "o" << "sh" << "py" << "rb" << "conf";
    graphics << "png" << "jpg" << "svg";
    system << "cfg" << "stl" << "pro" << "iso" << "img" << "bin" << "theme";
    office << "odt" << "odf" << "odp" << "od" << "txt" << "pdf";
    audiovideo << "wav" << "mp3" << "ogg" << "mpg" << "avi";
    network << "htm" << "html" << "xlm";
    utility << "gz" << "zip" << "bz2";
    cat_map.insert(devel_pix, devel);
    cat_map.insert(graphics_pix, graphics);
    cat_map.insert(system_pix, system);
    cat_map.insert(office_pix, office);
    cat_map.insert(audiovideo_pix, audiovideo);
    cat_map.insert(network_pix, network);
    cat_map.insert(utility_pix, utility);
    cat_map.insert(favourite_pix, favourite);
}

Fileicon::~Fileicon()
{
}

void Fileicon::read_settings()
{
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get category menu icons
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat);
    style->beginGroup("Launcher");
    utility_pix = stl_path + style->value("utility_pix").toString();
    office_pix = stl_path + style->value("office_pix").toString();
    network_pix = stl_path + style->value("network_pix").toString();
    graphics_pix = stl_path + style->value("graphics_pix").toString();
    devel_pix = stl_path + style->value("development_pix").toString();
    system_pix = stl_path + style->value("system_pix").toString();
    audiovideo_pix = stl_path + style->value("audiovideo_pix").toString();
    favourite_pix = stl_path + style->value("favourite_pix").toString();
    
    //favourite_pix = stl_path + style->value("favourite_pix").toString();
    style->endGroup(); // Launcher
    style->beginGroup("Deskfolder");
    d_folder_pix = stl_path + style->value("d_folder_pix").toString();
    style->endGroup(); // Deskfolder
    style->beginGroup("Other");
    application_pix = stl_path + style->value("application_pix").toString();
    style->endGroup(); // Other
}

QIcon Fileicon::icon(const QFileInfo &info) const
{
    QIcon default_ico(devel_pix); //default icon

    if (info.isDir())
    {
        QIcon ico(d_folder_pix);
        return ico;
    }

    if (info.isExecutable())
    {
        QIcon ico(application_pix);
        return ico;
    }

    QString suff = info.suffix(); // get the file extension
    QMapIterator<QString, QStringList> iter (cat_map); // (key = category pix path) (value = category file suffix list)

    while (iter.hasNext())
    {
        iter.next();
        if (iter.value().contains(suff, Qt::CaseInsensitive))
        {
            QString ico_name = iter.key();
            QIcon ico(ico_name);
            return ico;
        }
    }
    return default_ico; //default icon
}

QIcon Fileicon::icon() const
{
    QIcon ico(utility_pix); //default icon
    return ico;
}


QString Fileicon::type(const QFileInfo &info) const
{
    QString type;

    if (info.isDir())
    {
        type = "Directory";
        return type;
    }
    else if (info.isFile() && info.isExecutable())
    {
        type = "Executable";
        return type;
    }
    else if (info.isFile() && info.isSymLink())
    {
        type = "SymLink";
        return type;
    }
    else if (info.isFile())
    {
        type = "File";
        return type;
    }
    else
    {
        type = "Unknown";
        return type;
    }
}

QString Fileicon::icon_type(const QFileInfo &info) const
{
    QString suff = info.suffix(); // get the file extension
    QMapIterator<QString, QStringList> iter (cat_map); // (key = category pix path) (value = category file suffix list)

    while (iter.hasNext())
    {
        iter.next();
        if (iter.value().contains(suff, Qt::CaseInsensitive))
        {
            QString ico_name = iter.key();
            return ico_name;
        }
    }
    return system_pix; // default pix
}

