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


#ifndef DOMHELPER_H
#define DOMHELPER_H

#include <QtCore/QDebug>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>


/************************************************

 ************************************************/
QDebug operator<<(QDebug dbg, const QDomElement &el)
{
    QDomNamedNodeMap map = el.attributes();

    QString args;
    for (int i=0; i<map.count(); ++i)
        args += " " + map.item(i).nodeName() + "='" + map.item(i).nodeValue() + "'";

    dbg.nospace() << QString("<%1%2>%3</%1>").arg(el.tagName()).arg(args).arg(el.text());
    return dbg.space();
}



#endif // DOMHELPER_H
