#ifndef QTXDG_TEST_H
#define QTXDG_TEST_H

#include <QString>
#include <QDebug>

class QtXdgTest
{
public:
    // Test that XdgDesktopFile and xdg-mime script agree on 
    // default application for each mime-type.
    void testDefaultApp();

    void testTextHtml();
    void testMeldComparison();
    bool testCustomFormat();
    void compare(QString mimetype);
    QString xdgDesktopFileDefaultApp(QString mimetype);
    QString xdgUtilDefaultApp(QString mimetype);
};



int main(int argc, char** args)
{
//    QtXdgTest().testDefaultApp();
//      qDebug() << "Default for text/html:" << QtXdgTest().xdgDesktopFileDefaultApp("text/html");
//    QtXdgTest().testMeldComparison();
    qDebug() << QtXdgTest().testCustomFormat();
};

#endif /* QTXDG_TEST_H */
