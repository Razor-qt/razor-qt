#ifndef SUDIALOG_H
#define SUDIALOG_H

#include <QByteArray>

#include "ui_sudialog.h"


class SuDialog : public QDialog, public Ui::SuDialog
{
    Q_OBJECT

public:
    SuDialog(const QString & user, const QString & authUser, const QString & command);
    QString password();
};


#endif
