/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "xdgaction.h"
#include <razorqt/xdgicon.h>


/************************************************

 ************************************************/
XdgAction::XdgAction(QObject *parent):
    QAction(parent)
{
}


/************************************************

 ************************************************/
XdgAction::XdgAction(const XdgDesktopFile& desktopFile, QObject *parent):
    QAction(parent)
{
    load(desktopFile);
}


/************************************************

 ************************************************/
XdgAction::XdgAction(const XdgDesktopFile* desktopFile, QObject *parent):
    QAction(parent)
{
    load(*desktopFile);
}


/************************************************

 ************************************************/
XdgAction::XdgAction(const QString& desktopFileName, QObject *parent):
    QAction(parent)
{
    XdgDesktopFile df(desktopFileName);
    load(df);
}


/************************************************

 ************************************************/
XdgAction::XdgAction(const XdgAction& other, QObject *parent):
    QAction(parent)
{
    load(other.mDesktopFile);
}


/************************************************

 ************************************************/
XdgAction::~XdgAction()
{
}


/************************************************

 ************************************************/
XdgAction& XdgAction::operator=(const XdgAction& other)
{
    load(other.mDesktopFile);
     return *this;
}


/************************************************

 ************************************************/
bool XdgAction::isValid() const
{
    return mDesktopFile.isValid();
}


/************************************************

 ************************************************/
void XdgAction::load(const XdgDesktopFile& desktopFile)
{
    mDesktopFile = desktopFile;
    if (mDesktopFile.isValid())
    {
        setText(mDesktopFile.name());
        setToolTip(mDesktopFile.comment());

        setIcon(desktopFile.icon());
        if (icon().isNull())
            setIcon(XdgIcon::fromTheme("application-x-executable"));

        connect(this, SIGNAL(triggered()), this, SLOT(runConmmand()));
    }
    else
    {
        setText("");
        setToolTip("");
        setIcon(QIcon());
    }
}


/************************************************

 ************************************************/
void XdgAction::runConmmand() const
{
    if (mDesktopFile.isValid())
        mDesktopFile.startDetached();
}
