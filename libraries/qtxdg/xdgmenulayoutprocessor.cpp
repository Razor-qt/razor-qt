/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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


#include "xdgmenulayoutprocessor.h"
#include "xmlhelper.h"
#include <QDebug>
#include <QtCore/QMap>


/************************************************

 ************************************************/
QDomElement findLastElementByTag(const QDomElement element, const QString tagName)
{
    QDomNodeList l = element.elementsByTagName(tagName);
    if (l.isEmpty())
        return QDomElement();

    return l.at(l.length()-1).toElement();
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
XdgMenuLayoutProcessor::XdgMenuLayoutProcessor(QDomElement& element):
    mElement(element)
{
    mDefaultParams.mShowEmpty = false;
    mDefaultParams.mInline = false;
    mDefaultParams.mInlineLimit = 4;
    mDefaultParams.mInlineHeader = true;
    mDefaultParams.mInlineAlias = false;

    mDefaultLayout = findLastElementByTag(element, "DefaultLayout");

    if (mDefaultLayout.isNull())
    {
        // Create DefaultLayout node
        QDomDocument doc = element.ownerDocument();
        mDefaultLayout = doc.createElement("DefaultLayout");

        QDomElement menus = doc.createElement("Merge");
        menus.setAttribute("type", "menus");
        mDefaultLayout.appendChild(menus);

        QDomElement files = doc.createElement("Merge");
        files.setAttribute("type", "files");
        mDefaultLayout.appendChild(files);

        mElement.appendChild(mDefaultLayout);
    }

    setParams(mDefaultLayout, &mDefaultParams);

    // If a menu does not contain a <Layout> element or if it contains an empty <Layout> element
    // then the default layout should be used.
    mLayout = findLastElementByTag(element, "Layout");
    if (mLayout.isNull() || !mLayout.hasChildNodes())
        mLayout = mDefaultLayout;
}


/************************************************

 ************************************************/
XdgMenuLayoutProcessor::XdgMenuLayoutProcessor(QDomElement& element, XdgMenuLayoutProcessor *parent):
    mElement(element)
{
    mDefaultParams = parent->mDefaultParams;

    // DefaultLayout ............................
    QDomElement defaultLayout = findLastElementByTag(element, "DefaultLayout");

    if (defaultLayout.isNull())
        mDefaultLayout = parent->mDefaultLayout;
    else
        mDefaultLayout = defaultLayout;

    setParams(mDefaultLayout, &mDefaultParams);

    // If a menu does not contain a <Layout> element or if it contains an empty <Layout> element
    // then the default layout should be used.
    mLayout = findLastElementByTag(element, "Layout");
    if (mLayout.isNull() || !mLayout.hasChildNodes())
        mLayout = mDefaultLayout;

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::setParams(QDomElement defaultLayout, LayoutParams *result)
{
    if (defaultLayout.hasAttribute("show_empty"))
        result->mShowEmpty = defaultLayout.attribute("show_empty") == "true";

    if (defaultLayout.hasAttribute("inline"))
        result->mInline = defaultLayout.attribute("inline") == "true";

    if (defaultLayout.hasAttribute("inline_limit"))
        result->mInlineLimit = defaultLayout.attribute("inline_limit").toInt();

    if (defaultLayout.hasAttribute("inline_header"))
        result->mInlineHeader = defaultLayout.attribute("inline_header") == "true";

    if (defaultLayout.hasAttribute("inline_alias"))
        result->mInlineAlias = defaultLayout.attribute("inline_alias") == "true";
}


/************************************************

 ************************************************/
QDomElement XdgMenuLayoutProcessor::searchElement(const QString &tagName, const QString &attributeName, const QString &attributeValue) const
{
    DomElementIterator it(mElement, tagName);
    while (it.hasNext())
    {
        QDomElement e = it.next();
        if (e.attribute(attributeName) == attributeValue)
        {
            return e;
        }
    }

    return QDomElement();
}


/************************************************

 ************************************************/
int childsCount(const QDomElement& element)
{
    int count = 0;
    DomElementIterator it(element);
    while (it.hasNext())
    {
        QString tag = it.next().tagName();
        if (tag == "AppLink" || tag == "Menu" || tag == "Separator")
            count ++;
    }

    return count;
}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::run()
{
    QDomDocument doc = mLayout.ownerDocument();
    mResult = doc.createElement("Result");
    mElement.appendChild(mResult);

    // Process childs menus ...............................
    {
        DomElementIterator it(mElement, "Menu");
        while (it.hasNext())
        {
            QDomElement e = it.next();
            XdgMenuLayoutProcessor p(e, this);
            p.run();
        }
    }


    // Step 1 ...................................
    DomElementIterator it(mLayout);
    it.toFront();
    while (it.hasNext())
    {
        QDomElement e = it.next();

        if (e.tagName() == "Filename")
            processFilenameTag(e);

        else if (e.tagName() == "Menuname")
            processMenunameTag(e);

        else if (e.tagName() == "Separator")
            processSeparatorTag(e);

        else if (e.tagName() == "Merge")
        {
            QDomElement merge = mResult.ownerDocument().createElement("Merge");
            merge.setAttribute("type", e.attribute("type"));
            mResult.appendChild(merge);
        }
    }

    // Step 2 ...................................
    {
        MutableDomElementIterator ri(mResult, "Merge");
        while (ri.hasNext())
        {
            processMergeTag(ri.next());
        }
    }

    // Move result cilds to element .............
    MutableDomElementIterator ri(mResult);
    while (ri.hasNext())
    {
        mElement.appendChild(ri.next());
    }

    // Final ....................................
    mElement.removeChild(mResult);

    if (mLayout.parentNode() == mElement)
        mElement.removeChild(mLayout);

    if (mDefaultLayout.parentNode() == mElement)
        mElement.removeChild(mDefaultLayout);

}


/************************************************
 The <Filename> element is the most basic matching rule.
 It matches a desktop entry if the desktop entry has the given desktop-file id
 ************************************************/
void XdgMenuLayoutProcessor::processFilenameTag(const QDomElement &element)
{
    QString id = element.text();

    QDomElement appLink = searchElement("AppLink", "id", id);
    if (!appLink.isNull())
        mResult.appendChild(appLink);
}


/************************************************
 Its contents references an immediate sub-menu of the current menu, as such it should never contain
 a slash. If no such sub-menu exists the element should be ignored.
 This element may have various attributes, the default values are taken from the DefaultLayout key.

 show_empty [ bool ]
    defines whether a menu that contains no desktop entries and no sub-menus
    should be shown at all.

 inline [ bool ]
    If the inline attribute is "true" the menu that is referenced may be copied into the current
    menu at the current point instead of being inserted as sub-menu of the current menu.

 inline_limit [ int ]
    defines the maximum number of entries that can be inlined. If the sub-menu has more entries
    than inline_limit, the sub-menu will not be inlined. If the inline_limit is 0 (zero) there
    is no limit.

 inline_header [ bool ]
    defines whether an inlined menu should be preceded with a header entry listing the caption of
    the sub-menu.

 inline_alias [ bool ]
    defines whether a single inlined entry should adopt the caption of the inlined menu. In such
    case no additional header entry will be added regardless of the value of the inline_header
    attribute.

 Example: if a menu has a sub-menu titled "WordProcessor" with a single entry "OpenOffice 4.2", and
 both inline="true" and inline_alias="true" are specified then this would result in the
 "OpenOffice 4.2" entry being inlined in the current menu but the "OpenOffice 4.2" caption of the
 entry would be replaced with "WordProcessor".
 ************************************************/
void XdgMenuLayoutProcessor::processMenunameTag(const QDomElement &element)
{
    QString id = element.text();
    QDomElement menu = searchElement("Menu", "name", id);
    if (menu.isNull())
        return;

    LayoutParams params = mDefaultParams;
    setParams(element, &params);

    int count = childsCount(menu);

    if (count == 0)
    {
        if (params.mShowEmpty)
        {
            menu.setAttribute("keep", "true");
            mResult.appendChild(menu);
        }
        return;
    }


    bool doInline = params.mInline &&
                    (!params.mInlineLimit || params.mInlineLimit > count);

    bool doAlias = params.mInlineAlias &&
                   doInline && (count == 1);

    bool doHeader = params.mInlineHeader &&
                    doInline && !doAlias;


    if (!doInline)
    {
        mResult.appendChild(menu);
        return;
    }


    // Header ....................................
    if (doHeader)
    {
        QDomElement header = mLayout.ownerDocument().createElement("Header");

        QDomNamedNodeMap attrs = menu.attributes();
        for (int i=0; i < attrs.count(); ++i)
        {
            header.setAttributeNode(attrs.item(i).toAttr());
        }

        mResult.appendChild(header);
    }

    // Alias .....................................
    if (doAlias)
    {
        menu.firstChild().toElement().setAttribute("title", menu.attribute("title"));
    }

    // Inline ....................................
    MutableDomElementIterator it(menu);
    while (it.hasNext())
    {
        mResult.appendChild(it.next());
    }

}


/************************************************
 It indicates a suggestion to draw a visual separator at this point in the menu.
 <Separator> elements at the start of a menu, at the end of a menu or that directly
 follow other <Separator> elements may be ignored.
 ************************************************/
void XdgMenuLayoutProcessor::processSeparatorTag(const QDomElement &element)
{
    QDomElement separator = element.ownerDocument().createElement("Separator");
    mResult.appendChild(separator);
}


/************************************************
 It indicates the point where desktop entries and sub-menus that are not explicitly mentioned
 within the <Layout> or <DefaultLayout> element are to be inserted.
 It has a type attribute that indicates which elements should be inserted:

 type="menus"
    means that all sub-menus that are not explicitly mentioned should be inserted in alphabetical
    order of their visual caption at this point.

 type="files" means that all desktop entries contained in this menu that are not explicitly
    mentioned should be inserted in alphabetical order of their visual caption at this point.

 type="all" means that a mix of all sub-menus and all desktop entries that are not explicitly
    mentioned should be inserted in alphabetical order of their visual caption at this point.

 ************************************************/
void XdgMenuLayoutProcessor::processMergeTag(const QDomElement &element)
{
    QString type = element.attribute("type");
    QMap<QString, QDomElement> map;
    MutableDomElementIterator it(mElement);

    while (it.hasNext())
    {
        QDomElement e = it.next();
        if (
            ((type == "menus" || type == "all") && e.tagName() == "Menu" ) ||
            ((type == "files" || type == "all") && e.tagName() == "AppLink")
           )
            map.insert(e.attribute("title"), e);
    }

    QMapIterator<QString, QDomElement> mi(map);
    while (mi.hasNext()) {
        mi.next();
        mResult.insertBefore(mi.value(), element);
    }

    mResult.removeChild(element);
}

