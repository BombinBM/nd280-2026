#ifndef DRAW_FIBERS
#define DRAW_FIBERS

#include <iostream>
#include <string>

#include <TH2D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class DrawFibers : public AnalysisStrategy
{
private:
    TH2D* XYFibprojection = nullptr;
    TH2D* XZFibprojection = nullptr;
    TH2D* YZFibprojection = nullptr;

    TH1D* ChargeZ = nullptr;
    TH1D* ChargeY = nullptr;
    TH1D* ChargeX = nullptr;
public:
    DrawFibers() : AnalysisStrategy("DrawFibers")
    {
        XYFibprojection = new TH2D("FibHitsXY", "FibHitsXY", 194, -1000, 1000, 58, -300, 300);
        XZFibprojection = new TH2D("FibHitsXZ", "FibHitsXZ", 194, -3000, -1000, 194, -1000, 1000);
        YZFibprojection = new TH2D("FibHitsYZ", "FibHitsYZ", 194, -3000, -1000, 58, -300, 300);

        ChargeX = new TH1D("ChargeX", "ChargeX", 200, 0, 400);
        ChargeY = new TH1D("ChargeY", "ChargeY", 200, 0, 400);
        ChargeZ = new TH1D("ChargeZ", "ChargeZ", 200, 0, 400);

        
        hists2D.push_back(XYFibprojection);
        hists2D.push_back(XZFibprojection);
        hists2D.push_back(YZFibprojection);

        hists.push_back(ChargeX);
        hists.push_back(ChargeY);
        hists.push_back(ChargeZ);
    }
    ~DrawFibers() = default;

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            TClonesArray *FibHits = nullptr;
            ND::TSFGReconModule::TSFGHit *Fibhit = nullptr;
            int NFibhits;

            reader.SetBranchAddres("NFibers", &NFibhits);
            reader.SetBranchAddres("Fibers", &FibHits);
            reader.GetEntry(eventCount);
            for (int it = 0; it < NFibhits; it++)
            {
                Fibhit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(FibHits->At(it));
                if (Fibhit->Charge > 10)
                {    
                    if(Fibhit->Position.X() < -980)
                    {
                        YZFibprojection->Fill(Fibhit->Position.Z(), Fibhit->Position.Y(), Fibhit->Charge);
                        ChargeX->Fill(Fibhit->Charge);
                        continue;
                    }
                    else if (Fibhit->Position.Z() < -2855)
                    {
                        XYFibprojection->Fill(Fibhit->Position.X(), Fibhit->Position.Y(), Fibhit->Charge);
                        ChargeZ->Fill(Fibhit->Charge);
                        continue;
                    }
                    else
                    {
                        XZFibprojection->Fill(Fibhit->Position.Z(), Fibhit->Position.X(), Fibhit->Charge);
                        ChargeY->Fill(Fibhit->Charge);
                        continue;
                    }
                
                std::cout << "Bad coordinates for fiber with: "; 
                
                Fibhit->Position.Print();
                std::cout << std::endl;
                }
            }
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
};

#endif