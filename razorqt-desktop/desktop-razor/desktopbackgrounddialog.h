#ifndef DESKTOPBACKGROUNDDIALOG_H
#define DESKTOPBACKGROUNDDIALOG_H

#include "ui_desktopbackgrounddialog.h"
#include "workspacemanager.h"

class ReadSettings;


class DesktopBackgroundDialog : public QDialog, public Ui::DesktopBackgroundDialog
{
    Q_OBJECT
    
public:
    DesktopBackgroundDialog(QSize desktopSize, QWidget * parent);
    ~DesktopBackgroundDialog();
    QBrush background();
    void save(int screen, ReadSettings * cfg);

private:
    QSize m_desktopSize;
    RazorWorkSpaceManager::BackgroundType m_type;
    QColor m_color;
    QString m_wallpaper;

private slots:
    void colorButton_clicked();
    void wallpaperButton_clicked();
    void systemButton_clicked();
    void preview();
};

#endif
