#include "qtxdg_test.h"

#include "qtxdg/xdgdesktopfile.h"
#include "qtxdg/xdgdirs.h"

#include <QtTest>

#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <QDebug>
#include <QSettings>

void QtXdgTest::testDefaultApp()
{
    QStringList mimedirs = XdgDirs::dataDirs();
    mimedirs.prepend(XdgDirs::dataHome(false));
    foreach (QString mimedir, mimedirs)
    {
        QDir dir(mimedir + "/mime");
        qDebug() << dir.path();
        QStringList filters = (QStringList() << "*.xml");
        foreach(QFileInfo mediaDir, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            qDebug() << "    " << mediaDir.fileName();
            foreach (QString mimeXmlFileName, QDir(mediaDir.absoluteFilePath()).entryList(filters, QDir::Files))
            {
                QString mimetype = mediaDir.fileName() + "/" + mimeXmlFileName.left(mimeXmlFileName.length() - 4);
                
                QString xdg_utils_default = xdgUtilDefaultApp(mimetype);
                QString desktop_file_default = xdgDesktopFileDefaultApp(mimetype);
                
                if (xdg_utils_default != desktop_file_default) 
                {
                    qDebug() << mimetype;
                    qDebug() << "xdg-mime query default:" << xdg_utils_default;
                    qDebug() << "xdgdesktopfile default:" << desktop_file_default;
                }
            }
        }
         
    }
}

void QtXdgTest::compare(QString mimetype)
{
    QString xdgUtilDefault = xdgUtilDefaultApp(mimetype);
    QString xdgDesktopDefault = xdgDesktopFileDefaultApp(mimetype);
    if (xdgUtilDefault != xdgDesktopDefault) 
    {
        qDebug() << mimetype;
        qDebug() << "xdg-mime default:" << xdgUtilDefault;
        qDebug() << "xdgDesktopfile default:" << xdgDesktopDefault;
    }
}


void QtXdgTest::testTextHtml()
{
    compare("text/html");
}

void QtXdgTest::testMeldComparison()
{
    compare("application/x-meld-comparison");
}

// Implemented in xdgdesktopfile.cpp
bool readDesktopFile(QIODevice & device, QSettings::SettingsMap & map);
bool writeDesktopFile(QIODevice & device, const QSettings::SettingsMap & map);


void QtXdgTest::testCustomFormat()
{
    QSettings::Format desktopFormat = QSettings::registerFormat("list", readDesktopFile, writeDesktopFile);
    QFile::remove("/tmp/test.list");
    QFile::remove("/tmp/test2.list");
    QSettings test("/tmp/test.list", desktopFormat);
    test.beginGroup("Default Applications");
    test.setValue("text/plain", QString("gvim.desktop"));
    test.setValue("text/html", QString("firefox.desktop"));
    test.endGroup();
    test.beginGroup("Other Applications");
    test.setValue("application/pdf", QString("qpdfview.desktop"));
    test.setValue("image/svg+xml", QString("inkscape.desktop"));
    test.sync();

    QFile::copy("/tmp/test.list", "/tmp/test2.list");

    QSettings test2("/tmp/test2.list", desktopFormat);
    QVERIFY(test2.allKeys().size() == 4);

    test2.beginGroup("Default Applications");
//    qDebug() << test2.value("text/plain");
    QVERIFY(test2.value("text/plain") == QString("gvim.desktop"));

//    qDebug() << test2.value("text/html");
    QVERIFY(test2.value("text/html") == QString("firefox.desktop"));
    test2.endGroup();

    test2.beginGroup("Other Applications");
//    qDebug() << test2.value("application/pdf");
    QVERIFY(test2.value("application/pdf") == QString("qpdfview.desktop"));

//    qDebug() << test2.value("image/svg+xml");
    QVERIFY(test2.value("image/svg+xml") == QString("inkscape.desktop"));
    test2.endGroup();
}


QString QtXdgTest::xdgDesktopFileDefaultApp(QString mimetype)
{
    XdgDesktopFile *defaultApp = XdgDesktopFileCache::getDefaultApp(mimetype); 
    QString defaultAppS;
    if (defaultApp)
    {
        defaultAppS = QFileInfo(defaultApp->fileName()).fileName();
    } 
    return defaultAppS;
}



QString QtXdgTest::xdgUtilDefaultApp(QString mimetype)
{
    QProcess xdg_mime;
    QString program = "xdg-mime";
    QStringList args = (QStringList() << "query" << "default" << mimetype);
    qDebug() << "running" << program << args.join(" ");
    xdg_mime.start(program, args);
    xdg_mime.waitForFinished(1000);
    return QString(xdg_mime.readAll()).trimmed();
}

#if 0
int main(int argc, char** args)
{
//    QtXdgTest().testDefaultApp();
//      qDebug() << "Default for text/html:" << QtXdgTest().xdgDesktopFileDefaultApp("text/html");
//    QtXdgTest().testMeldComparison();
    qDebug() << QtXdgTest().testCustomFormat();
};
#endif // 0

QTEST_MAIN(QtXdgTest)
