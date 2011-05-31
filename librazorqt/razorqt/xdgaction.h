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


#ifndef XDGACTION_H
#define XDGACTION_H

#include <QtGui/QAction>
#include <QtCore/QString>
#include "xdgdesktopfile.h"

/*******************************************************************/ /*!
  @brief The XdgAction class provides an QAction object based on XdgDesktopFile.

  The following properties of the action are set based on the desktopFile.
    Text    - XdgDesktopFile localizeValue("Name")
    Icon    - XdgDesktopFile icon()
    ToolTip - XdgDesktopFile localizeValue("Comment")

  Internally this function will create a copy of the desktopFile, so you
  can delete original XdgDesktopFile object.

  When an action is activated by the user; for example, when the user clicks
  a menu option, toolbar button or when trigger() was called, XdgAction start
  the application defined in XdgDesktopFile. @sa XdgDesktopFile::startDetached.
****************************************/
//*******************************************************************
class XdgAction : public QAction
{
    Q_OBJECT
public:
    explicit XdgAction(QObject *parent=0);
    explicit XdgAction(const XdgDesktopFile& desktopFile, QObject *parent=0);
    explicit XdgAction(const XdgDesktopFile* desktopFile, QObject *parent=0);
    explicit XdgAction(const QString& desktopFileName, QObject *parent=0);
    // Constructs a XdgAction that is a copy of the given XdgAction.
    explicit XdgAction(const XdgAction& other, QObject *parent=0);

    /// Destroys the object and frees allocated resources.
    virtual ~XdgAction();
    XdgAction& operator=(const XdgAction& other);

    //! Returns true if the XdgAction is valid; otherwise returns false.
    bool isValid() const;

    const XdgDesktopFile& desktopFile() const { return mDesktopFile; }

private slots:
    void runConmmand() const;

private:
    void load(const XdgDesktopFile& desktopFile);

    XdgDesktopFile mDesktopFile;
};

#endif // XDGACTION_H
