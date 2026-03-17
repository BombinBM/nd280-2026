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
        XYFibprojection = new TH2D("HitsXY", "HitsXY", 194, -1000, 1000, 58, -300, 300);
        XZFibprojection = new TH2D("HitsXZ", "HitsXZ", 194, -3000, -1000, 194, -1000, 1000);
        YZFibprojection = new TH2D("HitsYZ", "HitsYZ", 194, -3000, -1000, 58, -300, 300);

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
            TClonesArray *Hits = nullptr;
            ND::TSFGReconModule::TSFGHit *hit = nullptr;
            int Nhits;

            reader.SetBranchAddres("NFibers", &Nhits);
            reader.SetBranchAddres("Fibers", &Hits);
            reader.GetEntry(eventCount);
            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                if(hit->Position.X() < -980)
                {
                    YZFibprojection->Fill(hit->Position.Z(), hit->Position.Y(), hit->Charge);
                    ChargeX->Fill(hit->Charge);
                    continue;
                }
                else if (hit->Position.Z() < -2855)
                {
                    XYFibprojection->Fill(hit->Position.X(), hit->Position.Y(), hit->Charge);
                    ChargeZ->Fill(hit->Charge);
                    continue;
                }
                else
                {
                    XZFibprojection->Fill(hit->Position.Z(), hit->Position.X(), hit->Charge);
                    ChargeY->Fill(hit->Charge);
                    continue;
                }
                std::cout << "Bad coordinates for fiber with: "; 
                hit->Position.Print();
                std::cout << std::endl;
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