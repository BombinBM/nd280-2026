#include <iostream>
#include "includes.h"

int main(int argc, char** argv)
{

    TApplication *app = new TApplication("App", &argc, argv);

    const char* load_script_in = ".x /home/bogdan/Desktop/VScode/nd280-2026/test3/load_classes.C";

    gROOT->ProcessLine(load_script_in);

    std::cout << "Bye";
    gSystem->Abort();

    return 0;
}
