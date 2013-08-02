/* 
 * File:   YamlParser.cpp
 * Author: christian
 * 
 * Created on 9. maj 2013, 13:13
 */

#include <QIODevice>
#include <QRegExp>
#include <QDebug>

#include "yamlparser.h"

YamlParser::YamlParser()
{
    state = start;
}

YamlParser::~YamlParser()
{
}

void YamlParser::consumeLine(QString line)
{
    static QRegExp documentStart("---\\s*(\\[\\]\\s*)?");
    static QRegExp mapStart("(-\\s*)(\\w*)\\s*:(.*)$");
    static QRegExp mapEntry("(\\s*)(\\w*)\\s*:(.*)");
    static QRegExp continuation("(\\s*)(.*)");
    static QRegExp documentEnd("...\\s*");
    static QRegExp emptyLine("\\s*(#.*)?");
  
    qDebug() << line;

    if (documentStart.exactMatch(line))
    {
        m_ListOfMaps.clear();
        state = atdocumentstart;
        m_CurrentIndent = -1;
    }
    else if (state == error) 
    {
        // Skip
    }
    else if (emptyLine.exactMatch(line))
    {
        // Skip
    }
    else if ((state == atdocumentstart || state == inlist) && mapStart.exactMatch(line)) 
    {
        m_ListOfMaps << QMap<QString, QString>();
        addEntryToCurrentMap(mapStart.cap(2), mapStart.cap(3));
        m_CurrentIndent = mapStart.cap(1).size();
        state = inlist;
    }
    else if (state == inlist && mapEntry.exactMatch(line) && mapEntry.cap(1).size() == m_CurrentIndent)
    {
        addEntryToCurrentMap(mapEntry.cap(2), mapEntry.cap(3));
    }
    else if (state == inlist && continuation.exactMatch(line) && continuation.cap(1).size() > m_CurrentIndent)
    {
        m_ListOfMaps.last()[m_LastKey].append(continuation.cap(2));
    }
    else if ((state == atdocumentstart || state == inlist) && documentEnd.exactMatch(line))
    {
        qDebug() << "emitting:" << m_ListOfMaps;
        emit newListOfMaps(m_ListOfMaps);
        state = documentdone;
    }
    else 
    {
        qWarning() << "Yaml parser could not read:" << line;
        state = error;
    }
}

void YamlParser::addEntryToCurrentMap(QString key, QString value)
{
    m_ListOfMaps.last()[key.trimmed()] = value.trimmed();
    m_LastKey = key.trimmed();
}
