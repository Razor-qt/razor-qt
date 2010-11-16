#ifndef XDGREADER_H
#define XDGREADER_H

//2010 Chris "VdoP" Regali
//Reads XML-Files and stores them in a tree-widget



/**
 * @file xdgreader.h
 * @brief Declares Xdghandler and Xdgreader
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */

#include "defs.h"
#include "xdgmenucross.h"
//we need a datahandler to handle the data from the reader

/**
* @brief Handles all of our XML-Parsers Calls
*/

class XdgHandler : public QXmlDefaultHandler
{
public:
    XdgHandler(XdgMenuCross* _tree);
    bool startElement(	const QString& _nameuri,
                        const QString& _localname,
                        const QString& _qName,
                        const QXmlAttributes& _attributes);

    bool endElement(	const QString& _nameuri,
                        const QString& _localname,
                        const QString& _qName);
    bool characters(const QString& _str);
    bool fatalError(const QXmlParseException& _exception);


private:
    XdgMenuCross* 	currentTree;
    XdgMenuLogic* currentItem;
    QString		 	currentText;

};

/**
* @brief Does the actual parsing of the XML files
*/
//now we actually need to read that stuff
class XdgReader : public QObject
{
    Q_OBJECT
public:
    XdgReader(QString _filename, XdgMenuCross& _mainmenu ,QObject* _parent=0);
    ~XdgReader();
    XdgMenuCross*			 mainMenu;

private:
    QXmlInputSource*   		 xdgfile;
    QXmlSimpleReader*		 xdgreader;
    XdgHandler* 			 xdghandler;
    QFile* 					 qfilehandler;
};







#endif

