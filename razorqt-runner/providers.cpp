/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "providers.h"
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgmenu.h>
#include <qtxdg/xmlhelper.h>
#include <qtxdg/xdgdirs.h>

#include <QtCore/QProcess>
#include <QtCore/QtAlgorithms>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <razorqt-runner/providers.h>
#include <wordexp.h>

#define MAX_HISORTY 100


/************************************************

 ************************************************/
unsigned int CommandProviderItem::stringRank(const QString str, const QString pattern) const
{
    int n = str.indexOf(pattern, 0, Qt::CaseInsensitive);
    if (n<0)
        return 0;

    return MAX_RANK - ((str.length() - pattern.length()) + n * 5);
}


/************************************************

 ************************************************/
CommandProvider::CommandProvider():
    QObject(),
    QList<CommandProviderItem*>()
{
}


/************************************************

 ************************************************/
CommandProvider::~CommandProvider()
{
    qDeleteAll(*this);
}


/************************************************

 ************************************************/
AppLinkItem::AppLinkItem(const QDomElement &element):
        CommandProviderItem()
{
    mIconName = element.attribute("icon");
    mTitle = element.attribute("title");
    mComment = element.attribute("genericName");
    mToolTip = element.attribute("comment");
    mCommand = QFileInfo(element.attribute("exec")).baseName().section(" ", 0, 0);
    mDesktopFile = element.attribute("desktopFile");
}


/************************************************

 ************************************************/
void AppLinkItem::updateIcon()
{
    if (mIcon.isNull())
        mIcon = XdgIcon::fromTheme(mIconName);
}


/************************************************

 ************************************************/
void AppLinkItem::operator=(const AppLinkItem &other)
{
    mTitle = other.title();
    mComment = other.comment();
    mToolTip = other.toolTip();

    mCommand = other.mCommand;
    mDesktopFile = other.mDesktopFile;

    mIconName = other.mIconName;
    mIcon = other.icon();
}


/************************************************

 ************************************************/
unsigned int AppLinkItem::rank(const QString &pattern) const
{
    return qMax(stringRank(mCommand, pattern),
                stringRank(mTitle, pattern)
               );
}


/************************************************

 ************************************************/
bool AppLinkItem::run() const
{
    XdgDesktopFile *desktop = XdgDesktopFileCache::getFile(mDesktopFile);
    return desktop->startDetached();
}


/************************************************

 ************************************************/
bool AppLinkItem::compare(const QRegExp &regExp) const
{
    if (regExp.isEmpty())
        return false;

    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseInsensitive);
    return mCommand.contains(re) ||
           mTitle.contains(re) ;
}




/************************************************

 ************************************************/
AppLinkProvider::AppLinkProvider():
        CommandProvider(),
        mXdgMenu( new XdgMenu())
{
    mXdgMenu->setEnvironments("X-RAZOR");
    connect(mXdgMenu, SIGNAL(changed()), this, SLOT(update()));
    mXdgMenu->read(XdgMenu::getMenuFileName());
    update();
}


/************************************************

 ************************************************/
AppLinkProvider::~AppLinkProvider()
{
    delete mXdgMenu;
}


/************************************************

 ************************************************/
void doUpdate(const QDomElement &xml, QHash<QString, AppLinkItem*> &items)
{
    DomElementIterator it(xml, "");
    while (it.hasNext())
    {
        QDomElement e = it.next();

        // Build submenu ........................
        if (e.tagName() == "Menu")
            doUpdate(e, items);

        //Build application link ................
        else if (e.tagName() == "AppLink")
        {
            AppLinkItem *item = new AppLinkItem(e);
            items.insert(item->command(), item);
        }
    }
}


/************************************************

 ************************************************/
void AppLinkProvider::update()
{
    emit aboutToBeChanged();
    QHash<QString, AppLinkItem*> newItems;
    doUpdate(mXdgMenu->xml().documentElement(), newItems);

    {
        QMutableListIterator<CommandProviderItem*> i(*this);
        while (i.hasNext()) {
            AppLinkItem *item = static_cast<AppLinkItem*>(i.next());
            AppLinkItem *newItem = newItems.take(item->command());
            if (newItem)
            {
                *(item) = *newItem;  // Copy by value, not pointer!
                item->updateIcon();
                delete newItem;
            }
            else
            {
                i.remove();
                delete item;
            }
        }
    }

    {
        QHashIterator<QString, AppLinkItem*> i(newItems);
        while (i.hasNext()) {
            AppLinkItem *item = i.next().value();
            append(item);
            item->updateIcon();
        }
    }

    emit changed();
}




/************************************************

 ************************************************/
HistoryItem::HistoryItem(const QString &command):
        CommandProviderItem()
{
    mIcon = XdgIcon::defaultApplicationIcon();
    mTitle = command;
    mComment = QObject::tr("History");
    mCommand = command;
}


/************************************************

 ************************************************/
bool HistoryItem::run() const
{
    return QProcess::startDetached(mCommand);
}


/************************************************

 ************************************************/
bool HistoryItem::compare(const QRegExp &regExp) const
{
    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseSensitive);
    return mCommand.contains(re);
}


/************************************************

 ************************************************/
unsigned int HistoryItem::rank(const QString &pattern) const
{
    return stringRank(mCommand, pattern);
}


/************************************************

 ************************************************/
HistoryProvider::HistoryProvider():
        CommandProvider()
{
    QString fileName = (XdgDirs::cacheHome() + "/razor-runner.history");
    mHistoryFile = new QSettings(fileName, QSettings::IniFormat);
    mHistoryFile->beginGroup("commands");
    for (uint i=0; i<MAX_HISORTY; ++i)
    {
        QString key = QString("%1").arg(i, 3, 10, QChar('0'));
        if (mHistoryFile->contains(key))
        {
            HistoryItem *item = new HistoryItem(mHistoryFile->value(key).toString());
            append(item);
        }
    }
}


/************************************************

 ************************************************/
HistoryProvider::~HistoryProvider()
{
    delete mHistoryFile;
}


/************************************************

 ************************************************/
void HistoryProvider::AddCommand(const QString &command)
{
    HistoryItem *item = new HistoryItem(command);
    insert(0, item);

    mHistoryFile->clear();
    for (int i=0; i<qMin(length(), MAX_HISORTY); ++i)
    {
        QString key = QString("%1").arg(i, 3, 10, QChar('0'));
        mHistoryFile->setValue(key, static_cast<HistoryItem*>(at(i))->command());
    }
}

void HistoryProvider::clearHistory()
{
    clear();
    mHistoryFile->clear();
}

/************************************************

 ************************************************/
CustomCommandItem::CustomCommandItem():
    CommandProviderItem()
{
    mIcon = XdgIcon::fromTheme("utilities-terminal");
}


/************************************************

 ************************************************/
QString which(const QString &progName)
{
    if (progName.isEmpty())
        return "";

    if (progName.startsWith(QDir::separator()))
    {
        if (QFileInfo(progName).isExecutable())
            QFileInfo(progName).absoluteFilePath();
    }

    QStringList dirs = QString(getenv("PATH")).split(":");

    foreach (QString dir, dirs)
    {
        if (QFileInfo(QDir(dir), progName).isExecutable())
            return QFileInfo(QDir(dir), progName).absoluteFilePath();
    }

    return "";
}


/************************************************

 ************************************************/
QString expandCommand(const QString &command, QStringList *arguments=0)
{
    QString program;
    wordexp_t words;

    if (wordexp(command.toLocal8Bit().data(), &words, 0) != 0)
        return "";

    char **w;
    w = words.we_wordv;
    program = QString::fromLocal8Bit(w[0]);

    if (arguments)
    {
        for (size_t i = 1; i < words.we_wordc; i++)
            *arguments << w[i];
    }

    wordfree(&words);
    return program;
}


/************************************************

 ************************************************/
void CustomCommandItem::setCommand(const QString &command)
{
    mCommand = command;
    mTitle = mCommand;

    QString program  = which(expandCommand(command));

    if (!program.isEmpty())
        mComment = QString("%1 %2").arg(program, command.section(' ', 1));
    else
        mComment = "";

}


/************************************************

 ************************************************/
bool CustomCommandItem::run() const
{
    QStringList args;
    QString program  = expandCommand(mCommand, &args);
    if (program.isEmpty())
        return false;

    return QProcess::startDetached(program, args);
}


/************************************************

 ************************************************/
bool CustomCommandItem::compare(const QRegExp &regExp) const
{
    return !mComment.isEmpty();
}


/************************************************

 ************************************************/
unsigned int CustomCommandItem::rank(const QString &pattern) const
{
    return 0;
}


/************************************************

 ************************************************/
CustomCommandProvider::CustomCommandProvider():
        CommandProvider()
{
    mItem = new CustomCommandItem();
    append(mItem);
}

#ifdef VBOX_ENABLED
VirtualBoxItem::VirtualBoxItem(const QString & MachineName , const QIcon & Icon):
        CommandProviderItem()
{
    mTitle = MachineName;
    mIcon = Icon;
}

bool VirtualBoxItem::run() const
{
    QStringList arguments;
    arguments << "startvm" << title();
    return QProcess::startDetached ("VBoxManage" , arguments);
}

bool VirtualBoxItem::compare(const QRegExp &regExp) const
{
    QRegExp re(regExp);
    re.setCaseSensitivity(Qt::CaseInsensitive);
    return (! regExp.isEmpty() && -1 != re.indexIn (title ()));
}

unsigned int VirtualBoxItem::rank(const QString &pattern) const
{
    return stringRank(mTitle, pattern);
}

///////
VirtualBoxProvider::VirtualBoxProvider():
        virtualBoxConfig ( QDesktopServices::storageLocation (QDesktopServices::HomeLocation)
                           + "/.VirtualBox/VirtualBox.xml")
{
    fp.setFileName (virtualBoxConfig);

    static const char *kOSTypeIcons [][2] =
    {
        {"Other",           ":/vbox-icons/os_other.png"},
        {"DOS",             ":/vbox-icons/os_dos.png"},
        {"Netware",         ":/vbox-icons/os_netware.png"},
        {"L4",              ":/vbox-icons/os_l4.png"},
        {"Windows31",       ":/vbox-icons/os_win31.png"},
        {"Windows95",       ":/vbox-icons/os_win95.png"},
        {"Windows98",       ":/vbox-icons/os_win98.png"},
        {"WindowsMe",       ":/vbox-icons/os_winme.png"},
        {"WindowsNT4",      ":/vbox-icons/os_winnt4.png"},
        {"Windows2000",     ":/vbox-icons/os_win2k.png"},
        {"WindowsXP",       ":/vbox-icons/os_winxp.png"},
        {"WindowsXP_64",    ":/vbox-icons/os_winxp_64.png"},
        {"Windows2003",     ":/vbox-icons/os_win2k3.png"},
        {"Windows2003_64",  ":/vbox-icons/os_win2k3_64.png"},
        {"WindowsVista",    ":/vbox-icons/os_winvista.png"},
        {"WindowsVista_64", ":/vbox-icons/os_winvista_64.png"},
        {"Windows2008",     ":/vbox-icons/os_win2k8.png"},
        {"Windows2008_64",  ":/vbox-icons/os_win2k8_64.png"},
        {"Windows7",        ":/vbox-icons/os_win7.png"},
        {"Windows7_64",     ":/vbox-icons/os_win7_64.png"},
        {"WindowsNT",       ":/vbox-icons/os_win_other.png"},
        {"OS2Warp3",        ":/vbox-icons/os_os2warp3.png"},
        {"OS2Warp4",        ":/vbox-icons/os_os2warp4.png"},
        {"OS2Warp45",       ":/vbox-icons/os_os2warp45.png"},
        {"OS2eCS",          ":/vbox-icons/os_os2ecs.png"},
        {"OS2",             ":/vbox-icons/os_os2_other.png"},
        {"Linux22",         ":/vbox-icons/os_linux22.png"},
        {"Linux24",         ":/vbox-icons/os_linux24.png"},
        {"Linux24_64",      ":/vbox-icons/os_linux24_64.png"},
        {"Linux26",         ":/vbox-icons/os_linux26.png"},
        {"Linux26_64",      ":/vbox-icons/os_linux26_64.png"},
        {"ArchLinux",       ":/vbox-icons/os_archlinux.png"},
        {"ArchLinux_64",    ":/vbox-icons/os_archlinux_64.png"},
        {"Debian",          ":/vbox-icons/os_debian.png"},
        {"Debian_64",       ":/vbox-icons/os_debian_64.png"},
        {"OpenSUSE",        ":/vbox-icons/os_opensuse.png"},
        {"OpenSUSE_64",     ":/vbox-icons/os_opensuse_64.png"},
        {"Fedora",          ":/vbox-icons/os_fedora.png"},
        {"Fedora_64",       ":/vbox-icons/os_fedora_64.png"},
        {"Gentoo",          ":/vbox-icons/os_gentoo.png"},
        {"Gentoo_64",       ":/vbox-icons/os_gentoo_64.png"},
        {"Mandriva",        ":/vbox-icons/os_mandriva.png"},
        {"Mandriva_64",     ":/vbox-icons/os_mandriva_64.png"},
        {"RedHat",          ":/vbox-icons/os_redhat.png"},
        {"RedHat_64",       ":/vbox-icons/os_redhat_64.png"},
        {"Ubuntu",          ":/vbox-icons/os_ubuntu.png"},
        {"Ubuntu_64",       ":/vbox-icons/os_ubuntu_64.png"},
        {"Xandros",         ":/vbox-icons/os_xandros.png"},
        {"Xandros_64",      ":/vbox-icons/os_xandros_64.png"},
        {"Linux",           ":/vbox-icons/os_linux_other.png"},
        {"FreeBSD",         ":/vbox-icons/os_freebsd.png"},
        {"FreeBSD_64",      ":/vbox-icons/os_freebsd_64.png"},
        {"OpenBSD",         ":/vbox-icons/os_openbsd.png"},
        {"OpenBSD_64",      ":/vbox-icons/os_openbsd_64.png"},
        {"NetBSD",          ":/vbox-icons/os_netbsd.png"},
        {"NetBSD_64",       ":/vbox-icons/os_netbsd_64.png"},
        {"Solaris",         ":/vbox-icons/os_solaris.png"},
        {"Solaris_64",      ":/vbox-icons/os_solaris_64.png"},
        {"OpenSolaris",     ":/vbox-icons/os_opensolaris.png"},
        {"OpenSolaris_64",  ":/vbox-icons/os_opensolaris_64.png"},
        {"QNX",             ":/vbox-icons/os_qnx.png"},
    };

    for (size_t n = 0; n < sizeof (kOSTypeIcons) / sizeof(kOSTypeIcons[0]); ++ n)
    {
        osIcons.insert (kOSTypeIcons [n][0], (kOSTypeIcons [n][1]));
    }
}

void VirtualBoxProvider::rebuild()
{
    QDomDocument d;
    if ( !d.setContent( &fp ) )
    {
        qDebug() << "Unable to parse: " << fp.fileName();
        return;
    }

    QDomNodeList _dnlist = d.elementsByTagName( "MachineEntry" );
    for ( int i = 0; i < _dnlist.count(); i++ )
    {
        QDomNode node = _dnlist.at( i );
        QString ref = node.toElement().attribute( "src" );
        if ( ref.isEmpty() )
        {
            qDebug() << "MachineEntry with no src attribute";
            continue;
        }

        QFile m( ref );

        QDomDocument mspec;
        if ( !mspec.setContent( &m ) )
        {
            qDebug() << "Could not parse machine file " << m.fileName();
            continue;
        }

        QDomNodeList _mlist = mspec.elementsByTagName( "Machine" );
        for ( int j = 0; j < _mlist.count(); j++ )
        {
            QDomNode mnode = _mlist.at( j );
            QString type = mnode.toElement().attribute( "OSType" );

            VirtualBoxItem *virtualBoxItem = new VirtualBoxItem
            (
                mnode.toElement().attribute( "name" ) ,
		QIcon ( osIcons.value (type , ":/:/vbox-icons/os_other.png") )
            );

            append ( virtualBoxItem );
        }
    }

    timeStamp = QDateTime::currentDateTime();

}

bool VirtualBoxProvider::isOutDated() const
{
    return fp.exists() && ( timeStamp < QFileInfo ( virtualBoxConfig ).lastModified () );
}
#endif


#ifdef MATH_ENABLED
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

/************************************************

 ************************************************/
MathItem::MathItem():
        CommandProviderItem()
{
    mToolTip =QObject::tr("Mathematics");
    mIcon = XdgIcon::fromTheme("accessories-calculator");
}


/************************************************

 ************************************************/
bool MathItem::run() const
{
    return false;
}


/************************************************

 ************************************************/
bool MathItem::compare(const QRegExp &regExp) const
{
    QString s = regExp.pattern().trimmed();

    if (s.endsWith("="))
    {
        s.chop(1);
        QScriptEngine myEngine;
        QScriptValue res = myEngine.evaluate(s);
        if (res.isNumber())
        {
            MathItem *self=const_cast<MathItem*>(this);
            self->mTitle = s + " = " + res.toString();
            return true;
        }
    }

    return false;
}

/************************************************

 ************************************************/
unsigned int MathItem::rank(const QString &pattern) const
{
    return stringRank(mTitle, pattern);
}


/************************************************

 ************************************************/
MathProvider::MathProvider()
{
    append(new MathItem());
}

#endif
