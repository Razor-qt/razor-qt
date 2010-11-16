#ifndef XDGREADER_CPP
#define XDGREADER_CPP

#include "xdgreader.h"

/**
 * @file xdgreader.cpp
 * @brief Implements Xdghandler and Xdgreader
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */

// 2010 Chris "VdoP" Regali
//implementation of the Xdgreader

/**
 * @brief Constructor of the Class
 *
 * @param[in] _filename The XML-File to read
 * @param[in] _mainmenu the root-node (Xdgmenucross) which will be used to build the tree on
 * @param[in] _parent The QObject* which is the parent of the instance (not needed)
 */

XdgReader::XdgReader(QString _filename,XdgMenuCross& _mainmenu , QObject* _parent)
{
    Q_UNUSED(_parent);
    qDebug() << "Xdgreader: initialising...";
    //read the datafile and prepare the heap
    mainMenu=&_mainmenu;
    qfilehandler = new QFile(_filename);
    xdgfile = new QXmlInputSource(qfilehandler);
    xdghandler = new XdgHandler(mainMenu);
    xdgreader = new QXmlSimpleReader();

    //setting the handler
    xdgreader->setContentHandler(xdghandler);
    xdgreader->setErrorHandler(xdghandler);
    //start the parser
    xdgreader->parse(xdgfile);
}
/**
 * @brief The destructor
 */
XdgReader::~XdgReader()
{
    qDebug() << "Xdgreader: dying";
    delete xdgfile;
    delete xdghandler;
    delete xdgreader;
    delete qfilehandler;
}


/**
 * @brief The constructor of the Handler
 * Prepares our root-node for filling
 * @param[in] _tree the root-node (Xdgmenucross*) to fill
 */
//Implementation of the Event Handler
XdgHandler::XdgHandler(XdgMenuCross* _tree)
{
    qDebug() << "Xdghandler: initialising...";
    currentTree = _tree;
    currentTree->hasParent = false;
    currentTree->name="root";
    currentTree->directory="";
    currentTree->hasInclude = false;
    currentItem = 0;
}


/**
 * @brief This is called if there is any text incoming from the Xdgreader
 * @param[in] _str The text found by the Xdgreader
 */

bool XdgHandler::characters(const QString& _str)
{
    currentText += _str;
    return true;
}


/**
 * @brief This is called by the Xdgreader when a fatal error occures while parsing
 * @param[in] _exception the QXmlParseException that holds some info about the error
 * @todo make this use the MsgBox-Class of the Razor-Qt project
 */

bool XdgHandler::fatalError(const QXmlParseException& _exception)
{
    //todo: make this stuff use msgbox-class

    QMessageBox::warning(0, QObject::tr("xdgreader.cpp"),
                         QObject::tr("Parse error at line %1, column "
                                     "%2:\n%3.")
                         .arg(_exception.lineNumber())
                         .arg(_exception.columnNumber())
                         .arg(_exception.message()));
    return false;
}

/**
 * @brief This is called by the Xdgreader of an Element starts
 * @param[in] _qName holds the name of the Tag
 * Read the QT-Manual for more info about the params
 */


bool XdgHandler::startElement(  const QString& _nameuri,
                                const QString& _localname,
                                const QString& _qName,
                                const QXmlAttributes& _attributes)
{
    Q_UNUSED(_nameuri);
    Q_UNUSED(_attributes);
    Q_UNUSED(_localname);


    if (_qName == "Menu")
    {
        XdgMenuCross* newMenu = new XdgMenuCross(currentTree);
        newMenu->hasParent=true;
        currentTree->subMenu.append(newMenu);
        currentTree=newMenu;

    }
    else if (_qName == "Name")
    {
        currentText.clear();
    }
    else if (_qName == "Directory")
    {

        currentText.clear();
    }
    else if (_qName == "Include")
    {
        currentTree->inc_categories = new XdgMenuLogic(XDGMENULOGIC_AND, false, NULL);
        currentTree->inc_categories->hasParent = false; //this is the fucking root-include
        currentTree->hasInclude=true;
        currentItem=currentTree->inc_categories;
    }
    else if (_qName == "And")
    {
        int newcurrent = currentItem->makeChild(XDGMENULOGIC_AND,false, currentItem);
        currentItem = currentItem->children.at(newcurrent-1);
    }
    else if (_qName == "Or")
    {
        int newcurrent = currentItem->makeChild(XDGMENULOGIC_OR,false, currentItem);
        currentItem = currentItem->children.at(newcurrent-1);
    }
    else if (_qName == "Not")
    {
        int newcurrent = currentItem->makeChild(XDGMENULOGIC_AND,false, currentItem);
        currentItem = currentItem->children.at(newcurrent-1);
        currentItem->not_=true;
    }


    /*else if (_qName == "Exclude")
    {
    	currentItem = new QTreeWidgetItem(currentItem);
    	currentItem->setText(0, "Exclude");
    }*/
    else if (_qName == "Category")
    {
        currentText.clear();
    }
    /*else if (_qName == "Filename")
    {
    	currentItem = new QTreeWidgetItem(currentItem);
    	currentText.clear();
    }*/

    return true;

}

/**
 * @brief This is called by the Xdgreader of an Element ends
 * @param[in] _qName holds the name of the Tag
 * Read the QT-Manual for more info about the params
 */

bool XdgHandler::endElement(	const QString& _nameuri,
                                const QString& _localname,
                                const QString& _qName)
{

    Q_UNUSED(_localname);
    Q_UNUSED(_nameuri);

    if (_qName == "Menu")
    {
        if (currentTree->hasParent)
            currentTree = currentTree->parent;
    }
    else if (_qName == "Name")
    {
        currentTree->name = currentText;
    }
    else if (_qName == "Directory")
    {
        currentTree->directory = currentText;
    }
    else if (_qName == "Include")
    {
        //currentItem = currentItem->parent;
    }
    /*else if (_qName == "Exclude")
    {
    	currentItem = currentItem->parent();
    }*/
    else if (_qName == "Category")
    {
        currentItem->cat_list.append(currentText);
    }

    /*else if (_qName == "Filename")
    {
    	currentItem->setText(0, "Filename: "+ currentText);
    	currentItem = currentItem->parent();
    }*/
    else if (_qName == "Not" || _qName == "And" || _qName == "Or" || _qName == "Include")
    {
        if (currentItem->hasParent)
            currentItem = currentItem->parent;
        else
            qDebug()<< "Xdgreader: Tried to escape tree!!!";
    }

    //GNOME / KDE / Quasistandart:



    return true;
}



#endif
