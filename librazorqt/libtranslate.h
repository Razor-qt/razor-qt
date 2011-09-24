#ifndef LIBTRANSLATE_H
#define LIBTRANSLATE_H

#include <QtCore/QTranslator>
#include <QtCore/QCoreApplication>

inline void libTranslate(const QString &name)
{
    static bool alreadyLoaded = false;

    if (alreadyLoaded)
        return;

    QString locale = QLocale::system().name();
    QTranslator *translator = new QTranslator();
    translator->load(QString("%1/%2_%3.qm").arg(TRANSLATIONS_DIR, name, locale));

    QCoreApplication::installTranslator(translator);
    alreadyLoaded = true;
}


#endif // LIBTRANSLATE_H
