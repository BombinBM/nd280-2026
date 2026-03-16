#include <iostream>
#include "includes.h"

using std::cout, std::endl;

int main(int argc, char** argv)
{

    std::string filename;

    if (argc > 1)
    {
        filename = argv[1];
        cout << "Использован файл: " << filename << endl;}
    else
    {
        filename = "data/mup/oa_pg_mup_00000001-0001_400MeV_begin.root";
        cout << "Использован файл по умолчанию: " << filename << endl;
    }
    
    TApplication *app = new TApplication("App", &argc, argv);

    const char* load_script_in = ".x /home/bogdan/Desktop/VScode/nd280-2026/test3/load_classes.C";

    gROOT->ProcessLine(load_script_in);

    std::string treename = "ReconDir/SFG";

    FileReader reader;

    if (!reader.OpenFile(filename, treename))
    {
        std::cerr << "Failed to open file" << endl;
        return 0;
    }

    // reader.PrintBranches();
    std::vector<std::string> strats; 

    StrategyManager stratMan;
    // stratMan.AddStrategy(std::unique_ptr<DrawEntries>(new DrawEntries()));
    stratMan.AddStrategy(std::unique_ptr<DrawFibers>(new DrawFibers()));
    // stratMan.AddStrategy(std::unique_ptr<EventCharge>(new EventCharge()));
    // stratMan.AddStrategy(std::unique_ptr<EventEnergy>(new EventEnergy()));
    stratMan.BeginAll(reader);
    // stratMan.ProcessRun(reader);
    stratMan.ProcessEvent(reader);
    stratMan.EndAll();
    stratMan.PrintAllStats();
    // stratMan.DrawAll();
    stratMan.WriteAll("out.root");
    app->Run();
    cout << "Bye" << endl;
    return 0;
}