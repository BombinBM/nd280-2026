#ifndef EVENT_ENERGY
#define EVENT_ENERGY

#include <iostream>
#include <string>

#include <TH1D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "config.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class EventEnergy : public AnalysisStrategy
{
private:
    TClonesArray *TrueHits = nullptr;
    ND::TSFGReconModule::TSFGHit *TrueHit = nullptr;
    int NTrueHits = 0;

    TH1D *trueEdep = nullptr;
    TH1D *trueFulledep = nullptr;
public:
    EventEnergy() : AnalysisStrategy("EventEnergy")
    {
        trueEdep = new TH1D("trueEdep", "Edep, MeV", 100, 0, 30);
        trueFulledep = new TH1D("trueFullEdep", "Full Edep, MeV", 100, 0, 1000);

        hists.push_back(trueEdep);
        hists.push_back(trueFulledep);
    }

    ~EventEnergy() = default;

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        // set branch addresses to member variables so pointers remain valid
        bool okN = reader.SetBranchAddres("NTrueHits", &NTrueHits);
        bool okH = reader.SetBranchAddres("TrueHits", &TrueHits);
        if (!okN || !okH)
        {
            std::cerr << "EventEnergy::Begin: failed to bind NTrueHits/TrueHits branches" << std::endl;
        }
        trueEdep->Reset();
        trueFulledep->Reset();
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            ND::TSFGReconModule::TSFGHit *hit = nullptr;
            double sum = 0;

            if (!reader.GetEntry(eventCount))
            {
                std::cerr << "EventEnergy: failed to read entry " << eventCount << std::endl;
                return;
            }

            if (!TrueHits)
            {
                std::cerr << "EventEnergy: TrueHits pointer is null" << std::endl;
                return;
            }

            for (int i = 0; i < NTrueHits; i++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(TrueHits->At(i));
                if (!hit) continue;
                trueEdep->Fill(hit->Charge);
                sum += hit->Charge;
            }
            trueFulledep->Fill(sum);
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
};

#endif