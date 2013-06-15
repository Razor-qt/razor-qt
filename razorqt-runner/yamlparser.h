/* 
 * File:   YamlParser.h
 * Author: christian
 *
 * Created on 9. maj 2013, 13:13
 */

#ifndef YAMLPARSER_H
#define	YAMLPARSER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>

class QIODevice;

class YamlParser : public QObject
{
    Q_OBJECT
public:
    YamlParser();
    virtual ~YamlParser();
   
    void consumeLine(QString line);
 
signals:
    void newListOfMaps(QList<QMap<QString, QString> > maps);
   
    
private:
    
    QList<QMap<QString, QString> > m_ListOfMaps;

    enum 
    {
        start,
        atdocumentstart,
        inlist,
        documentdone,
        error
    } state;

    int m_CurrentIndent;
    QString m_LastKey;

    void addEntryToCurrentMap(QString key, QString value);
};

#endif	/* YAMLPARSER_H */

