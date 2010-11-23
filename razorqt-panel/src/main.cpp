#include "defs.h"
#include "main.h"
#include "razor.h"

/**
 * @file main.cpp
 * @author Christopher "VdoP" Regali
 * @brief does the initialisation stuff
 */


int main (int argc, char* argv[])
{
    Razor* myinst = &Razor::getInstance();
    myinst->setupEvents(argc, argv);

    if (! myinst->setupGui())
        return 1;

    return myinst->get_events()->exec();
}
