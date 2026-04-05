#ifndef DRAW_FIBERS
#define DRAW_FIBERS

#include <iostream>
#include <string>
#include <map>

#include <TH2D.h>
#include <TF1.h>
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

    double XCut, YCut, ZCut, ECut;
public:
    DrawFibers() : AnalysisStrategy("DrawFibers")
    {
        XCut = 1000;
        YCut = 600;
        ZCut = 0;
        ECut = 10;
        XYFibprojection = new TH2D("FibHitsXY", "FibHitsXY", 194, -1000, 1000, 58, -300, 300);
        XZFibprojection = new TH2D("FibHitsXZ", "FibHitsXZ", 194, -3000, -1000, 194, -1000, 1000);
        YZFibprojection = new TH2D("FibHitsYZ", "FibHitsYZ", 194, -3000, -1000, 58, -300, 300);

        ChargeX = new TH1D("ChargeX", "ChargeX", 200, 0, 400);
        ChargeY = new TH1D("ChargeY", "ChargeY", 200, 0, 400);
        // ChargeZ = new TH1D("ChargeZ", "ChargeZ", 200, 0, 400);

        
        hists2D.push_back(XYFibprojection);
        hists2D.push_back(XZFibprojection);
        hists2D.push_back(YZFibprojection);

        hists.push_back(ChargeX);
        hists.push_back(ChargeY);
        // hists.push_back(ChargeZ);
    }
    ~DrawFibers() = default;

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            TClonesArray *FibHits = nullptr;
            ND::TSFGReconModule::TSFGHit *Fibhit = nullptr;
            int NFibhits;

            std::vector<double> LayerCharges;

            double x,y,z,charge;

            std::map<double, double> ZYmap, ZXmap, Zmap;

            reader.SetBranchAddres("NFibers", &NFibhits);
            reader.SetBranchAddres("Fibers", &FibHits);
            reader.GetEntry(eventCount);

            Fibhit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(FibHits->At(0));
            x = Fibhit->Position.X();
            y = Fibhit->Position.Y();
            z = Fibhit->Position.Z();

            for (int it = 0; it < NFibhits; it++)
            {
                Fibhit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(FibHits->At(it));

                x = Fibhit->Position.X();
                y = Fibhit->Position.Y();
                z = Fibhit->Position.Z();
                charge = Fibhit->Charge;                
                
                if (charge > ECut && z < ZCut && y < YCut && x < XCut)
                {    
                    if (y < -282)
                    {
                        if (ZYmap[z] < charge)
                        {
                            ZYmap[z] = charge;
                        }
                    }
                    else if (x < -980)
                    {
                        if (ZXmap[z] < charge)
                        {
                            ZXmap[z] = charge;
                        }
                        
                    }
                    
                    if(x < -980 && z > -2855)
                    {
                        YZFibprojection->Fill(z, y, charge);                        
                        // ChargeX->Fill(charge);
                        continue;
                    }
                    else if (z < -2855)
                    {
                        XYFibprojection->Fill(x, y, charge);
                        // ChargeZ->Fill(charge);
                        continue;
                    }
                    else
                    {
                        XZFibprojection->Fill(z, x, charge);
                        // ChargeY->Fill(charge);
                        continue;
                    }
                
                std::cout << "Bad coordinates for fiber with: "; 
                
                Fibhit->Position.Print();
                std::cout << std::endl;
                }
            }

            for(const auto& pair : ZXmap)
            {
                // std::cout << "Max charge in Z = " << pair.first << " by X is " << pair.second << std::endl;
                ChargeX->Fill(pair.second);
            }
            for(const auto& pair : ZYmap)
            {
                // std::cout << "Max charge in Z = " << pair.first << " by Y is " << pair.second << std::endl;
                ChargeY->Fill(pair.second);
            }
            // for(const auto& pair : Zmap)
            // {
            //     ChargeZ->Fill(pair.second);
            // }
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void SetCuts(double x, double y, double z, double e) override
    {
        XCut = x;
        YCut = y;
        ZCut = z;
        ECut = e;
    }

    void PrintCuts() override
    {
        std::cout << " ▶ Установленные каты по координатам и энергии" << '\n';
        std::cout << "   XCut = " << XCut << '\n';
        std::cout << "   YCut = " << YCut << '\n';
        std::cout << "   ZCut = " << ZCut << '\n';
        std::cout << "   Energy Cut = " << ECut << '\n';
    }

    void FitHists() const
    {
        for (const auto& hist : hists)
        {
            TF1 *fit_fun = new TF1("fit_fun", "gaus", ECut, hist->GetXaxis()->GetXmax());
            hist->Fit(fit_fun, "Q", "", ECut, hist->GetXaxis()->GetXmax());
            std::cout << "  Mean = " << fit_fun->GetParameter(1)  << "+-" << fit_fun->GetParError(1) << std::endl;
            std::cout << "  Sigma = " << fit_fun->GetParameter(2) << "+-" << fit_fun->GetParError(2) << std::endl;
        }
    }

    void PrintStats() const override
    {
        AnalysisStrategy::PrintStats();
        FitHists();
    }
};

#endif