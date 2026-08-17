#include "includes.h"

using std::cout, std::endl;

int main(int argc, char** argv)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::string filename;
    std::string outputfilename;
    std::string outputfileoption;

    switch (argc)
    {
    case 1:
        // filename = "data/37/mup/oa_pg_mup_00000001-0001_300MeV_begin.root";
        filename = "data/37/mup/beam/oa_pg_mup_00000001-0001_400MeV_sfg_center.root";
        cout << "Использован файл по умолчанию: " << filename << endl << endl;
        break;
    case 2:
        filename = argv[1];
        cout << "Использован файл: " << filename << endl << endl;
        break;
    case 3:
        filename = argv[1];
        outputfilename = argv[2];
        cout << "Использован файл: " << filename << endl;
        cout << "Данные будут в файле: " << outputfilename << endl << endl;
        break;
    case 4:
        filename = argv[1];
        outputfilename = argv[2];
        outputfileoption = argv[3];
        cout << "Использован файл: " << filename << endl;
        cout << "Файл вывода называется: " << outputfilename << endl;
        cout << "Опция для файла: " << outputfileoption << endl << endl;
        break;
    default:
        break;
    }

    TApplication *app = new TApplication("App", &argc, argv);

    gROOT->ProcessLine(".L /home/bogdan/Desktop/VScode/nd280-2026/test3/SFGAnalysis/SFGAnalysis.so");

    std::string treename = "ReconDir/SFG";

    FileReader hits_reader, true_reader, fiber_reader;

    if (!hits_reader.OpenFile(filename, treename) || !true_reader.OpenFile(filename, treename) || !fiber_reader.OpenFile(filename, treename))
    {
        std::cerr << "Failed to open file" << endl;
        return 0;
    }

    // hits_reader.PrintBranches();
    std::vector<std::string> strats; 

    StrategyManager hitsStratMan, trueStratMan, fiberStratMan;

    // trueStratMan.AddStrategy(std::unique_ptr<EventEnergy>(new EventEnergy));

    // trueStratMan.ProcessStrategies(true_reader);

    // strats = fiberStratMan.GetStrategyNames();

    // fiberStratMan.GetStrategy<DrawFibers>(strats[0])->SetCuts(1000,600,-1855, 10);
    // fiberStratMan.GetStrategy<DrawFibers>(strats[0])->PrintCuts();

    // fiberStratMan.ProcessStrategies(fiber_reader);

    // fiberStratMan.ProcessEvent(fiber_reader);
    
    // Debug: run only EventEnergy to isolate segfault
    // hitsStratMan.AddStrategy(std::unique_ptr<DrawEntries>(new DrawEntries));
    hitsStratMan.AddStrategy(std::unique_ptr<EventCharge>(new EventCharge));
    // hitsStratMan.AddStrategy(std::unique_ptr<CubesEdep>(new CubesEdep));
    // hitsStratMan.AddStrategy(std::unique_ptr<DrawFibers>(new DrawFibers));
    // hitsStratMan.AddStrategy(std::unique_ptr<EventEnergy>(new EventEnergy));
    // hitsStratMan.AddStrategy(std::unique_ptr<EventHeatmap>(new EventHeatmap));

    // strats = hitsStratMan.GetStrategyNames();

    hitsStratMan.ProcessStrategies(hits_reader);

    // for (int i = 0; i < 1; i++)
    // {
    //     hitsStratMan.BeginAll(hits_reader);
    //     hitsStratMan.ProcessEvent(hits_reader);
    //     hitsStratMan.EndAll();
    // }
    

    // stratMan.AddStrategy(std::unique_ptr<DrawFibers>(new DrawFibers()));
    // stratMan.AddStrategy(std::unique_ptr<EventEnergy>(new EventEnergy()));
    // stratMan.AddStrategy(std::unique_ptr<EventCharge>(new EventCharge()));
    // stratMan.AddStrategy(std::unique_ptr<DrawEntries>(new DrawEntries()));
    // stratMan.BeginAll(reader);
    // stratMan.ProcessRun(reader);
    // stratMan.ProcessEvent(reader);
    // stratMan.EndAll();
    // stratMan.ProcessStrategies(hits_reader);
    // stratMan.PrintAllStats();
    // stratMan.DrawAll();

    hitsStratMan.PrintAllStats();
    // trueStratMan.PrintAllStats();
    // fiberStratMan.PrintAllStats();

    if(!outputfileoption.empty() && !outputfilename.empty())
    {
        hitsStratMan.WriteAll(outputfilename, outputfileoption);
        // trueStratMan.WriteAll(outputfilename, outputfileoption);
        // fiberStratMan.WriteAll(outputfilename, outputfileoption);
    }
    else if(!outputfilename.empty())
    {
        hitsStratMan.WriteAll(outputfilename);
        // trueStratMan.WriteAll(outputfilename);
        // fiberStratMan.WriteAll(outputfilename);
    }
    else
    {
        hitsStratMan.WriteAll();
        // trueStratMan.WriteAll();
        // fiberStratMan.WriteAll();
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration_ms = end - start;
    cout << "Время выполнения: " << duration_ms.count() << " мс" << endl;

    // app->Run();
    cout << "Bye" << endl;
    return 0;
}