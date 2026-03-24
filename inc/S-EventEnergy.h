#ifndef EVENT_ENERGY
#define EVENT_ENERGY

#include <iostream>
#include <string>

#include <TH1F.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class EventEnergy : public AnalysisStrategy
{
private:
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

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            TClonesArray *Hits = nullptr;
            ND::TSFGReconModule::TSFGHit *hit = nullptr;
            int NHits;
            double sum = 0;
            reader.SetBranchAddres("NTrueHits", &NHits);
            reader.SetBranchAddres("TrueHits", &Hits);
            reader.GetEntry(eventCount);
            for (int i = 0; i < NHits; i++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
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