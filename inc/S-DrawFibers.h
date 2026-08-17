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
    TClonesArray *FibHits = nullptr;
    ND::TSFGReconModule::TSFGHit *Fibhit = nullptr;
    int NFibhits;

    TH2D* XYFibprojection = nullptr;
    TH2D* XZFibprojection = nullptr;
    TH2D* YZFibprojection = nullptr;

    TH1D* ChargeZ = nullptr;
    TH1D* ChargeY = nullptr;
    TH1D* ChargeX = nullptr;

    double XCut, YCut, ZCut;
    int LinearTracks;
public:
    DrawFibers() : AnalysisStrategy("DrawFibers")
    {
        XCut = 1000;
        YCut = 600;
        ZCut = 0;
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

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        LinearTracks = reader.GetEntries();
        bool okN = reader.SetBranchAddres("NFibers", &NFibhits);
        bool okH = reader.SetBranchAddres("Fibers", &FibHits);
        if (!okN || !okH)
        {
            std::cerr << "DrawFibers::Begin: failed to bind NFibers/Fibers branches" << std::endl;
        }
        XYFibprojection->Reset();
        XZFibprojection->Reset();
        YZFibprojection->Reset();

        ChargeX->Reset();
        ChargeY->Reset();
        ChargeZ->Reset();
    }

    // Запускает ридер одного эвента, а потом заполняет гистограммы положений файбера. Используется для анализа максимального
    // заряда в файбере, но не для полного энерговыделения, так как не учитывается crosstalk, метод валиден для треков, 
    // направленных почти коллинеарно одной из осей детектора
    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            // 0 - not X or Y or Z, 1 - Z, 2 - Y, 3 - X
            int track_direction = TrackDirection(reader);

            // std::cout << track_direction << '\t';

            // TClonesArray *FibHits = nullptr;
            // ND::TSFGReconModule::TSFGHit *Fibhit = nullptr;
            // int NFibhits;

            double x,y,z,charge;

            std::map<double, double> ZYmap, ZXmap, YZmap, YXmap, XYmap, XZmap;

            // reader.SetBranchAddres("NFibers", &NFibhits);
            // reader.SetBranchAddres("Fibers", &FibHits);
            // reader.GetEntry(eventCount);

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
                
                if (charge > STANDARD_ENERGY_CUT && z < ZCut && y < YCut && x < XCut)
                {    
                    if (track_direction == 1)
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
                    }
                    if (track_direction == 2)
                    {
                        if (z < -2855)
                        {
                            if (YZmap[y] < charge)
                            {
                                YZmap[y] = charge;
                            }
                        }
                        else if (x < -980)
                        {
                            if (YXmap[y] < charge)
                            {
                                YXmap[y] = charge;
                            }
                        }
                    }
                    if (track_direction == 2)
                    {
                        if (y < -282)
                        {
                            if (XYmap[x] < charge)
                            {
                                XYmap[x] = charge;
                            }
                        }
                        else if (z < -2855)
                        {
                            if (XZmap[x] < charge)
                            {
                                XZmap[x] = charge;
                            }
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
                ChargeX->Fill(pair.second);
            }
            for(const auto& pair : ZYmap)
            {
                ChargeY->Fill(pair.second);
            }

            for(const auto& pair : YXmap)
            {
                ChargeX->Fill(pair.second);
            }
            for(const auto& pair : YZmap)
            {
                ChargeZ->Fill(pair.second);
            }
            for(const auto& pair : XYmap)
            {
                ChargeY->Fill(pair.second);
            }
            for(const auto& pair : XZmap)
            {
                ChargeZ->Fill(pair.second);
            }

            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    int TrackDirection(FileReader &reader)
    {
        // TClonesArray *FibHits = nullptr;
        // ND::TSFGReconModule::TSFGHit *Fibhit = nullptr;
        // int NFibhits;

        // reader.SetBranchAddres("NFibers", &NFibhits);
        // reader.SetBranchAddres("Fibers", &FibHits);
        reader.GetEntry(eventCount);

        double x,y,z;
        double x_max = -1e6, y_max = -1e6, z_max = -1e6;
        double x_min = 1e6, y_min = 1e6, z_min = 1e6;
        for (int it = 0; it < NFibhits; it++)
        {
            Fibhit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(FibHits->At(it));
            x = Fibhit->Position.X();
            y = Fibhit->Position.Y();
            z = Fibhit->Position.Z();
            if (Fibhit->Charge > STANDARD_ENERGY_CUT && z < ZCut && y < YCut && x < XCut)
            {
                if(x > -980)
                {
                    if (x > x_max)
                    {
                        x_max = x;
                    }
                    if (x < x_min)
                    {
                        x_min = x;
                    }
                }
                if (z > -2855)
                {
                    if (z > z_max)
                    {
                        z_max = z;
                    }
                    if (z < z_min)
                    {
                        z_min = z;
                    }
                }
                if (y > -282)
                {
                    if (y > y_max)
                    {
                        y_max = y;
                    }
                    if (y < y_min)
                    {
                        y_min = y;
                    }   
                }
            }
        }

        if (abs(z_max-z_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(y_max-y_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(x_max-x_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT)
        {
            std::cout << "Too short track" << std::endl;
            return 0;
        }
        if (abs(z_max-z_min) >= MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(y_max-y_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(x_max-x_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT)
        {
            return 1;
        }
        if (abs(z_max-z_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(y_max-y_min) >= MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(x_max-x_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT)
        {
            return 2;
        }
        if (abs(z_max-z_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(y_max-y_min) < MAX_DIV_FOR_LINEAR_TRACK_CUT && abs(x_max-x_min) >= MAX_DIV_FOR_LINEAR_TRACK_CUT)
        {
            return 3;
        }

        // std::cout << "Track direction is undefined:" << (z_max-z_min) << '\t' << (y_max-y_min) << '\t' << (x_max-x_min) << std::endl;
        LinearTracks--;
        std::cout << "Track direction is not X,Y,Z" << std::endl;
        return 0;
    }

    void SetCuts(double x, double y, double z) override
    {
        XCut = x;
        YCut = y;
        ZCut = z;
    }

    void PrintCuts() override
    {
        std::cout << " ▶ Установленные каты по координатам и энергии" << '\n';
        std::cout << "   XCut = " << XCut << '\n';
        std::cout << "   YCut = " << YCut << '\n';
        std::cout << "   ZCut = " << ZCut << '\n';
        std::cout << "   ECut is standard = " << STANDARD_ENERGY_CUT << '\n';
    }

    void FitHists() const
    {
        for (const auto& hist : hists)
        {
            TF1 *fit_fun = new TF1("fit_fun", "gaus", STANDARD_ENERGY_CUT, hist->GetXaxis()->GetXmax());
            hist->Fit(fit_fun, "Q", "", STANDARD_ENERGY_CUT, hist->GetXaxis()->GetXmax());
            std::cout << "  Mean = " << fit_fun->GetParameter(1)  << "+-" << fit_fun->GetParError(1) << std::endl;
            std::cout << "  Sigma = " << fit_fun->GetParameter(2) << "+-" << fit_fun->GetParError(2) << std::endl;
        }
    }

    void PrintStats() const override
    {
        int total_events = GetEventCount(), linear_tracks = LinearTracks;
        AnalysisStrategy::PrintStats();
        std::cout << "   ◀ Прямолинейных треков: " << linear_tracks / static_cast<double>(total_events) * 100 << " %" << std::endl;
        FitHists();
    }
};

#endif