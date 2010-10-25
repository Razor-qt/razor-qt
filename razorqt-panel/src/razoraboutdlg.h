#ifndef RAZORABOUTDLG_H
#define RAZORABOUTDLG_H
#include "defs.h"
#include "ui_aboutdlg.h"

/**
 * @file razoraboutdlg.n
 * @author Christopher "VdoP" Regali
 * @brief this declares the about dialog
 */

/**
 * @brief displays a simple about dialog
 */
class Razoraboutdlg: public Ui_about, public QDialog
{
public:
    Razoraboutdlg(QString _about);
};


#endif
