#ifndef QTXDG_TEST_H
#define QTXDG_TEST_H

#include <QObject>
#include <QString>
#include <QDebug>

class QtXdgTest : public QObject
{
    Q_OBJECT

private slots:
    void testCustomFormat();

private:
    // Test that XdgDesktopFile and xdg-mime script agree on 
    // default application for each mime-type.
    void testDefaultApp();

    void testTextHtml();
    void testMeldComparison();
    void compare(QString mimetype);
    QString xdgDesktopFileDefaultApp(QString mimetype);
    QString xdgUtilDefaultApp(QString mimetype);

};

#endif /* QTXDG_TEST_H */
