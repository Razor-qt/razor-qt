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

class DomElementIterator
{
public:
    explicit DomElementIterator(const QDomNode& parentNode, const QString& tagName="")
    {
        mTagName = tagName;
        mParent = parentNode;
        toFront();
    }

    void toFront()
    {
        mNext = mParent.firstChildElement(mTagName);
    }

    bool hasNext()
    {
        return (!mNext.isNull());
    }

    const QDomElement& next()
    {
        mCur = mNext;
        mNext = mNext.nextSiblingElement(mTagName);
        return mCur;
    }


    void toBack()
    {
        mNext = mParent.lastChildElement(mTagName);
    }


    bool hasPrevious()
    {
        return (!mNext.isNull());
    }

    const QDomElement& previous()
    {
        mCur = mNext;
        mNext = mNext.previousSiblingElement(mTagName);
        return mCur;
    }

    const QDomElement& current() const
    {
        return mCur;
    }


private:
    QString  mTagName;
    QDomNode mParent;
    QDomElement mCur;
    QDomElement mNext;
};

class MutableDomElementIterator
{
public:
    explicit MutableDomElementIterator(QDomNode& parentNode, const QString& tagName="")
    {
        mTagName = tagName;
        mParent = parentNode;
        toFront();
    }

    void toFront()
    {
        mNext = mParent.firstChildElement(mTagName);
    }

    bool hasNext()
    {
        return (!mNext.isNull());
    }

    QDomElement& next()
    {
        mCur = mNext;
        mNext = mNext.nextSiblingElement(mTagName);
        return mCur;
    }


    void toBack()
    {
        mNext = mParent.lastChildElement(mTagName);
    }


    bool hasPrevious()
    {
        return (!mNext.isNull());
    }

    QDomElement& previous()
    {
        mCur = mNext;
        mNext = mNext.previousSiblingElement(mTagName);
        return mCur;
    }

    QDomElement& current()
    {
        return mCur;
    }


private:
    QString  mTagName;
    QDomNode mParent;
    QDomElement mCur;
    QDomElement mNext;
};





QDebug operator<<(QDebug dbg, const QDomElement &el);


#endif // DOMHELPER_H
