#include <QApplication>

#include "razormodman.h"

/**
* @file main.cpp
* @author Christopher "VdoP" Regali
* @brief just starts the sub-apps and in future maybe saves the windowstates
*/

/**
* @brief our main function doing the loading
*/

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    RazorModuleManager modman;
    return app.exec();
}
