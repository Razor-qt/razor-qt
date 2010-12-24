/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

#include "razormainmenu.h"
#include "razor.h"
#include <QDebug>
#include <razorqt/xdgdesktopfile.h>


/************************************************

 ************************************************/
RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorMainMenu * ret = new RazorMainMenu(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


/************************************************

 ************************************************/
RazorMainMenu::RazorMainMenu(RazorBar * panel, QWidget * parent, const QString & name):
    RazorPlugin(panel, parent, name)
{
    mainLayout()->addWidget(&mButton);
    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    mButton.setText("Razor");
}


/************************************************

 ************************************************/
RazorMainMenu::~RazorMainMenu()
{
}


/************************************************

 ************************************************/
void RazorMainMenu::showMenu()
{
    XdgDesktopFile::execute("konsole");
}

