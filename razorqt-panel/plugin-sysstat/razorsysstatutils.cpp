#include <QtCore/QRegExp>
#include <QtCore/qmath.h>

#include "razorsysstatutils.h"


namespace PluginSysStat
{

QString netSpeedToString(int value)
{
    QString prefix;
    static const char prefixes[] = "kMG";
    if (value / 10)
        prefix = QChar(prefixes[value / 10 - 1]);

    return QString("%1 %2B/s").arg(1 << (value % 10)).arg(prefix);
}

int netSpeedFromString(QString value)
{
    QRegExp re("^(\\d+) ([kMG])B/s$");
    if (re.exactMatch(value))
    {
        int shift = 0;
        switch (re.cap(2)[0].toAscii())
        {
        case 'k':
            shift = 10;
            break;

        case 'M':
            shift = 20;
            break;

        case 'G':
            shift = 30;
            break;
        }

        return qCeil(qLn(re.cap(1).toInt()) / qLn(2.)) + shift;
    }

    return 0;
}

}
