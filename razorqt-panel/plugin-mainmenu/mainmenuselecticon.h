#ifndef MAINMENUSELECTICON_H
#define MAINMENUSELECTICON_H

#include <QtGui/QDialog>
#include <QtCore/QDir>
#include <QtGui/QListWidgetItem>
#include <QtCore/QSettings>

namespace Ui {
    class MainMenuSelectIcon;
}

class MainMenuSelectIcon : public QDialog
{
    Q_OBJECT

public:
    explicit MainMenuSelectIcon(QSettings &settings, QWidget *parent = 0);
    ~MainMenuSelectIcon();

private:
    Ui::MainMenuSelectIcon *ui;
    QString mIconTheme;
    QDir mIconThemeDir;
    QDir mOwnIconsDir;
    QSettings &mSettings;
    int findIconTypes();

private slots:
    void iconTypeChanged(QString type);
    void saveIcon(QListWidgetItem *item);
    void chooseIconDialog();
    void systemIconChecked(bool state);
    void userIconChecked(bool state);

signals:
    void iconSelected();
};

#endif // MAINMENUSELECTICON_H
