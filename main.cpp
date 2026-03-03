#include <iostream>
#include "includes.h"

int main(int argc, char** argv)
{

    TApplication *app = new TApplication("App", &argc, argv);

    const char* load_script_in = ".x /home/bogdan/Desktop/VScode/nd280-2026/test3/load_classes.C";

    gROOT->ProcessLine(load_script_in);

    std::string filename = "data/mup/oa_pg_mup_00000001-0001_300MeV_begin.root";
    std::string treename = "ReconDir/SFG";

    FileReader reader;

    if (!reader.OpenFile(filename, treename))
    {
        std::cerr << "Failed to open file" << std::endl;
        return 0;
    }

    reader.PrintBranches();

    std::cout << "Bye" <<std::endl;
    return 0;