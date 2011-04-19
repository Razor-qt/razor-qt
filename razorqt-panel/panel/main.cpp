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
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>
#include <QDebug>

#include "razorpanelapplication.h"
#include "razorpanel.h"

/*! The razor-panel is the panel of Razor-qt.
  Usage: razor-panel [CONFIG_ID]
    CONFIG_ID      Section name in config file ~/.razor/panel.conf
                   (default main)
 */

int main(int argc, char *argv[])
{
    RazorPanelApplication a(argc, argv);

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("%1/razor-panel_%3.qm").arg(TRANSLATIONS_DIR, locale));
    a.installTranslator(&translator);

    RazorPanel window;
    window.show();

    return a.exec();

}
