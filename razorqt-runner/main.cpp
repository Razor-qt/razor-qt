/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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


#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

#include "dialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("%1/razor-runner_%2.qm").arg(TRANSLATIONS_DIR, locale));
    a.installTranslator(&translator);

    QWidget *hiddenPreviewParent = new QWidget(0, Qt::Tool);
    Dialog d(hiddenPreviewParent);
    //Dialog d;
    //d.show();

    return a.exec();

}
