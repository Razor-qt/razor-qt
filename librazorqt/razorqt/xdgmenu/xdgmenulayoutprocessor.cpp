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
#include "../domhelper.h"
#include <QDebug>


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

 ************************************************/
void setParams(QDomElement defaultLayout, LayoutParams defaultValues, LayoutParams* result)
{
    *result = defaultValues;

    if (defaultLayout.hasAttribute("show_empty"))
        result->mShowEmpty = defaultLayout.attribute("show_empty") == "true";

    if (defaultLayout.hasAttribute("inline"))
        result->mShowEmpty = defaultLayout.attribute("inline") == "true";

    if (defaultLayout.hasAttribute("inline_limit"))
        result->mShowEmpty = defaultLayout.attribute("inline_limit").toInt();

    if (defaultLayout.hasAttribute("inline_header"))
        result->mShowEmpty = defaultLayout.attribute("inline_header") == "true";

    if (defaultLayout.hasAttribute("inline_alias"))
        result->mShowEmpty = defaultLayout.attribute("inline_alias") == "true";
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

    if (!mDefaultLayout.isNull())
    {
        // DefaulLayout node found copy params from it
        setParams(mDefaultLayout, mDefaultParams, &mDefaultParams);
    }
    else
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

    // If a menu does not contain a <Layout> element or if it contains an empty <Layout> element
    // then the default layout should be used.
    mLayout = findLastElementByTag(element, "Layout");
    if (mLayout.isNull() || !mLayout.hasChildNodes())
        mLayout = mDefaultLayout;
}


/************************************************

 ************************************************/
XdgMenuLayoutProcessor::XdgMenuLayoutProcessor(QDomElement &element, XdgMenuLayoutProcessor *parent):
    mElement(element)
{
    // DefaultLayout ............................
    QDomElement defaultLayout = findLastElementByTag(element, "DefaultLayout");

    if (!defaultLayout.isNull())
    {
        // DefaulLayout node found copy params from it
        mDefaultLayout = defaultLayout;
        setParams(mDefaultLayout, mDefaultParams, &(parent->mDefaultParams));
    }
    else
    {
        // Copy DefaulLayout from parent
        mDefaultLayout = parent->mDefaultLayout;
        mDefaultParams = parent->mDefaultParams;
    }

    // If a menu does not contain a <Layout> element or if it contains an empty <Layout> element
    // then the default layout should be used.
    mLayout = findLastElementByTag(element, "Layout");
    if (mLayout.isNull() || !mLayout.hasChildNodes())
        mLayout = mDefaultLayout;

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::run()
{
    return; // <-------------------------------------------------
    QDomDocument doc = mLayout.ownerDocument();
    mResult = doc.createElement("Result");
    mElement.appendChild(mResult);

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
            processMergeTag(e);

    }

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::processFilenameTag(const QDomElement &element)
{

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::processMenunameTag(const QDomElement &element)
{

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::processSeparatorTag(const QDomElement &element)
{

}


/************************************************

 ************************************************/
void XdgMenuLayoutProcessor::processMergeTag(const QDomElement &element)
{

}

