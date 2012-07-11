#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QString>
//#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>

#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgautostart.h>

XdgDesktopFileList desktopFiles;

XdgDesktopFile *findDesktopFile(const QString &program)
{
    XdgDesktopFileList::iterator i;
    for (i = desktopFiles.begin(); i != desktopFiles.end(); ++i)
    {
        if ((*i).value("Exec") == program)
        {
            return &(*i);
        }
    }

    return 0;
}

void processModule(const QString &program, bool enable)
{
    XdgDesktopFile *file = findDesktopFile(program);

    if (file)
    {
        // Change value in the file .............
        if (file->value("Hidden").toBool() == enable)
        {
            file->setValue("Hidden", !enable);
            file->save(XdgAutoStart::localPath(*file));
        }
    }
    else
    {
        // Create the new file ..................
        QString fileName = program;
        fileName.replace(QRegExp("[ \\\\/|]", Qt::CaseSensitive, QRegExp::RegExp), "_");
        XdgDesktopFile file;
        file.setValue("X-Razor-Module", true);
        file.setValue("OnlyShowIn", "Razor;");
        file.setValue("Type", "Application");
        file.setValue("Exec", program);
        file.setValue("Name", program);
        file.setValue("Hidden", !enable);
        file.save(QString("%1/%2.desktop").arg(XdgDirs::autostartHome(true), fileName));
    }
}

int main(int, char**)
{
    desktopFiles = XdgAutoStart::desktopFileList(false);

    QTextStream in(stdin);
    QString line;

    while (!in.atEnd())
    {
        line = in.readLine();
        if (!line.contains('='))
        {
            continue;
        }

        QString key = line.section('=', 0, 0).trimmed();
        QString value = line.section('=', 1).trimmed();

        processModule(key, value.toUpper().contains("TRUE") || value.toUpper().contains("Yes"));
    }

    return 0;
}
