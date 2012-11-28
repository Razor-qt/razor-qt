#ifndef TECHNICALINFO_H
#define TECHNICALINFO_H

#include <QtCore/QList>
#include <QtCore/QPair>
#include <QDateTime>
#include <QVariant>

class TechInfoTable;

class TechnicalInfo
{
public:
    TechnicalInfo();
    ~TechnicalInfo();

    QString html() const;
    QString text() const;

    TechInfoTable *newTable(const QString &title);
    void add(const TechInfoTable &table);

private:
    QList<TechInfoTable*> mItems;
};




#endif // TECHNICALINFO_H
