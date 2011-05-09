#ifndef RAZORABOUTDLG_H
#define RAZORABOUTDLG_H

#include <QDialog>

/**
 * @file razoraboutdlg.n
 * @author Christopher "VdoP" Regali
 * @brief this declares the about dialog
 */

/**
 * @brief displays a simple about dialog
 */

class RazorAboutDLGPrivate;

class RazorAboutDLG: public QDialog
{
    Q_OBJECT
public:
    RazorAboutDLG();
private:
    RazorAboutDLGPrivate * d_ptr;
};


#endif
