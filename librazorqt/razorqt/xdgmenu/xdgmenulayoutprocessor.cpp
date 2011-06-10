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


#include "xdgmenulayoutprocessor.h"

#include <QDebug>

/*class LayoutProps
{
public:
    LayoutProps(const LayoutProps& other)
    {
        mShowEmpty = other.mShowEmpty;
        mIsInline = other.mIsInline;
        mInlineLimit = other.mInlineLimit;
        mInlineHeader = other.mInlineHeader;
        mInlineAlias = other.mInlineAlias;
    }

private:
    bool mShowEmpty;
    bool mIsInline;
    bool mInlineLimit;
    bool mInlineHeader;
    bool mInlineAlias;
};
*/




/************************************************
 If no default-layout has been specified then the layout as specified by
 the following elements should be assumed:
 <DefaultLayout
     show_empty="false"
     inline="false"
     inline_limit="4"
     inline_header="true"
     inline_alias="false">
     <Merge type="menus"/>
     <Merge type="files"/>
 </DefaultLayout>
 ************************************************/
XdgMenuLayoutProcessor::XdgMenuLayoutProcessor()
{
    /*
    mDefaultLayout.setShowEmpty(false);
    mDefaultLayout.setInline(false);
    mDefaultLayout.setInlineLimit(4);
    mDefaultLayout.setInlineHeader(true);
    mDefaultLayout.setInlineAlias(false);
*/
/*
    mDefaultLayout.mShowEmpty = false;
    mDefaultLayout.mInline = false;
    mDefaultLayout.mInlineLimit = 4;
    mDefaultLayout.mInlineHeader = true;
    mDefaultLayout.mInlineAlias = false;


    LayoutItem item;
    item.type = LayoutItem::MergeMenus;
    mDefaultLayout << item;

    item.type = LayoutItem::MergeFiles;
    mDefaultLayout << item;
*/
}


/************************************************

 ************************************************/
/*XdgMenuLayoutProcessor::XdgMenuLayoutProcessor(const Layout& other)
{
    mDefaultLayout = other;
}
*/

/************************************************

 ************************************************/
QDomElement findLastElementByTag(const QDomElement element, const QString tagName)
{
//    QDomNodeList l = element.elementsByTagName(tagName);
//    if (l.isEmpty())
//        return QDomElement();

    //return l.at(l.length()-1);
}


/************************************************
 If no default-layout has been specified then the layout as specified by
 the following elements should be assumed:
 <DefaultLayout
     show_empty="false"
     inline="false"
     inline_limit="4"
     inline_header="true"
     inline_alias="false">
     <Merge type="menus"/>
     <Merge type="files"/>
 </DefaultLayout>
 ************************************************/
void XdgMenuLayoutProcessor::run(QDomElement& element)
{
 /*   qDebug() << "RUN::::::::::::::::::::::::::::::";
    mDefaultLayout = findLastElementByTag(element, "DefaultLayout");

    if (mDefaultLayout.isNull())
    {
        // Create node ..........................
        QDomDocument doc = element.ownerDocument();
        mDefaultLayout = doc.createElement("DefaultLayout");
        QDomElement menus = doc.createElement("Merge");
        menus.setAttribute("type", "menus");

        QDomElement files = doc.createElement("Merge");
        files.setAttribute("type", "files");

        mDefaultLayout.appendChild(menus);
        mDefaultLayout.appendChild(files);

        element.appendChild(mDefaultLayout);

        mDefaultParams.mShowEmpty = false;
        mDefaultParams.mInline = false;
        mDefaultParams.mInlineLimit = 4;
        mDefaultParams.mInlineHeader = true;
        mDefaultParams.mInlineAlias = false;
    }
    else
    {
        mDefaultParams.mShowEmpty = mDefaultLayout.attribute("show_empty", "false");
        mDefaultParams.mInline = mDefaultLayout.attribute("inline", "false");
        mDefaultParams.mInlineLimit = mDefaultLayout.attribute("inline_limit", "4");
        mDefaultParams.mInlineHeader = mDefaultLayout.attribute("inline_header", "true");
        mDefaultParams.mInlineAlias = mDefaultLayout.attribute("inline_alias", "false");
    }

    run(element, this);
*/
}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::run(QDomElement &element, XdgMenuLayoutProcessor *parent)
{
    //mDefaultLayout = findLastElementByTag(element, "DefaultLayout");

/*    // DefaultLayout ............................
    {
        QDomNodeList l = element.elementsByTagName("DefaultLayout");
        if (!l.isEmpty())
        {
            QDomElement e = l.at(l.length()-1);
            mDefaultLayout.mShowEmpty =  e.attribute("show_empty", mDefaultLayout.mShowEmpty);
        }
    }
*/
}
