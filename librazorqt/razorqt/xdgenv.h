#ifndef XDGENV_H
#define XDGENV_H

#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
//2010 Chris "VdoP" Regali
//sets and gets Environmental variables of XDG


/**
 * @file xdgenv.h
 * @brief Declares the class Xdgenv
 * @date 9.9.2010 (DDMMYYYY)
 * @author Christopher "VdoP" Regali
  */


/**
 * @brief Does all the Handling for the XDG-Environmental Variables
 */
class Xdgenv : public QObject
{
	Q_OBJECT
	public:
		Xdgenv();
		~Xdgenv();
		void updateEnv();
		QString getEnv(QString _name);
		void setEnv();
		void debugEnv();
	private:
		QMap<QString, QString> envStore;
};


#endif
