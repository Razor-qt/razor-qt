#ifndef RAZORMAINMENUCONFIGURATION_H
#define RAZORMAINMENUCONFIGURATION_H

#include <razorqt/razorsettings.h>

#include <QtGui/QDialog>
#include <QtGui/QAbstractButton>
#include <QtCore/QSettings>

namespace Ui {
    class RazorMainMenuConfiguration;
}

class RazorMainMenuConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit RazorMainMenuConfiguration(QSettings &settings, QWidget *parent = 0);
    ~RazorMainMenuConfiguration();

private:
    Ui::RazorMainMenuConfiguration *ui;
    QSettings &mSettings;
    RazorSettingsCache mOldSettings;

private slots:
    /*
      Saves settings in conf file.
    */
    void saveSettings();
    void loadSettings();
    void dialogButtonsAction(QAbstractButton *btn);
    void showSelectIconWindow();
};

#endif // RAZORMAINMENUCONFIGURATION_H
