#ifndef CUBES_EDEP
#define CUBES_EDEP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <TH1D.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class CubesEdep : public AnalysisStrategy
{
private:
    TClonesArray *Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr, *endhit=nullptr;
    TVector3 endpos;
    int Ncubes = 250;
    int Nhists = 0;
    std::string mode = "Charge";

public:
    CubesEdep() : AnalysisStrategy("CubesEdep")
    {
        std::string tmp, name = "Cube Number ";
        std::string title = mode + " deposition";

        hists2D.push_back(new TH2D("Edep", "Energy deposition", 250, 0, 2500, 150, 0, 1500));

        for (int i = 0; i < Ncubes; i++)
        {
            tmp = name + i;
            hists.push_back(new TH1D(tmp.c_str(), title.c_str(), 150, 0, 1500));
        }

        Nhists = hists.size();
    } 

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            int Nhits, Exact_Dist_Cubes = 0, Cube_Number = 0;
        
            float charge = 0, dist = 0, min_time = 1e6, max_charge = 0, min_dist = 1e6;

            reader.SetBranchAddres("NHits", &Nhits);
            reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);
            for (int i = 0; i < Nhits; i++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                min_time = std::min(min_time,hit->Time);
                max_charge = std::max(max_charge, hit->Charge);
            }
            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                if(hit->Charge == max_charge)
                {
                    endpos = hit->Position;
                }
            }

            for (int j = 0; j < Nhits; j++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(j));
                if (hit->Time - min_time < 500 && hit->Charge > 100)
                {
                    hists2D[0]->Fill((endpos-hit->Position).Mag(), hit->Charge);
                }
                
            }
            
            while (true)
            {
                for (int i = 0; i < Nhits; i++)
                {
                    hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                    
                    if ((float)(endpos - hit->Position).Mag() > dist && hit->Charge > 80 && hit->Time - min_time < 1000)
                    {
                        // std::cout << (float)(endpos - hit->Position).Mag() << '\t';
                        min_dist = std::min(min_dist, (float)(endpos - hit->Position).Mag());
                    }
                }
                if (min_dist == 1e6)
                {
                    break;
                }
                
                // std::cout << min_dist << "\t";
                for (int j = 0; j < Nhits; j++)
                {
                    hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(j));
                    if ((float)(endpos - hit->Position).Mag() == min_dist && hit->Charge > 80 && hit->Time - min_time < 1000)
                    {   
                        if (Cube_Number > Nhists - 1)
                        {
                            break;
                        }
                        
                        charge += hit->Charge;
                        Exact_Dist_Cubes++;
                        dist = min_dist;
                        // break;
                    }
                }
                if (Exact_Dist_Cubes != 0 && Cube_Number < Nhists - 1)
                {
                    charge = charge / Exact_Dist_Cubes;
                    Cube_Number++;
                    hists[Cube_Number]->Fill(charge);
                    charge = 0;
                    Exact_Dist_Cubes = 0;
                }
                
                min_dist = 1e6;
                // std::cout << min_dist << '\t' << dist << "\n";
                if (Cube_Number > Nhists - 1)
                {
                    break;
                }
                
            }
            if (eventCount % 100 == 0)
            {
                std::cout << "Event number " << eventCount << std::endl;
            }

            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    void SetMode(std::string input_mode) {mode = input_mode;}
    void SetNcubes(int N) {Ncubes = N;}
};

#endif