#ifndef TRAJECTORY_RECOGNITION
#define TRAJECTORY_RECOGNITION

#include <iostream>
#include <string>
#include <vector>

#include <TH1I.h>
#include <TH1D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"
#include "config.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class TrajRecognition : public AnalysisStrategy
{
private:
    TClonesArray* Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr;
    int NHits;

    TH1I* PDGS = nullptr;
    TH1D* LifeTime = nullptr;
public:
    TrajRecognition() : AnalysisStrategy("TrajRecognition")
    {
        PDGS = new TH1I("HitsPDG", "HitsPDG", 2000, -10000, 10000);
        LifeTime = new TH1D("Lifetime", "Initial particle lifetime", 50, 0, 100);
        hists.push_back(PDGS);
        hists.push_back(LifeTime);
    }

    ~TrajRecognition() = default;

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        bool okN = reader.SetBranchAddres("NHits", &NHits);
        bool okH = reader.SetBranchAddres("Hits", &Hits);

        if (!okH || !okN)
        {
            std::cerr << "TrajRecognition::Begin: failed to bind NHits/Hits branches" << std::endl;
            return;  // Выход, если привязка не удалась
        }
        PDGS->Reset();
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            float min = 1e6, max = 0;
            int nullhits = 0;
            std::vector<int> hitpdgs;
            reader.GetEntry(eventCount);

            for (int it = 0; it < NHits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                if (!hit)
                {
                    nullhits++;
                    continue;
                }
                hitpdgs = hit->HitSegTruePDG;
                for (int& pdg : hitpdgs)
                {                    
                    PDGS->Fill(pdg);
                    if (pdg == 211)
                    {
                        min = std::min(min, hit->Time);
                        max = std::max(max, hit->Time);
                    }
                    // if (pdg != 211 && hit->Time > min)
                    // {
                    //     max = std::min(max, hit->Time);
                    //     std::cout << pdg << '\n';
                    // }
                    
                    
                }
            }
            std::cout << max - min << '\t';
            LifeTime->Fill(max - min);
            if (nullhits > 0)
            {
                std::cout << "  Number of null Hits: " << nullhits << '\n';
            }
            
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    // void End() {AnalysisStrategy::End();};
};

#endif