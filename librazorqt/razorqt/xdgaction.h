/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#ifndef XDGACTION_H
#define XDGACTION_H

#include <QtGui/QAction>
#include "xdgdesktopfile.h"

class XdgAction : public QAction
{
    Q_OBJECT
public:
    explicit XdgAction(QObject *parent=0);
    explicit XdgAction(const XdgDesktopFile& desktopFile, QObject *parent=0);
    explicit XdgAction(const XdgDesktopFile* desktopFile, QObject *parent=0);
    explicit XdgAction(const XdgAction& other, QObject *parent=0);
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
