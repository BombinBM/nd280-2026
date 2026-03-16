#ifndef EVENT_CHARGE
#define EVENT_CHARGE

#include <iostream>
#include <string>

#include <TH1D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class EventCharge : public AnalysisStrategy
{
private:
    TH1D *edep = nullptr;
    TH1D *fulledep = nullptr;
public:
    EventCharge() : AnalysisStrategy("EventCharge")
    {
        std::cout << "EventCharge constructor started!" << std::endl;
        edep = new TH1D("Edep", "Edep, PE", 300, 0, 300);
        fulledep = new TH1D("FullEdep", "Full Edep, PE", 100, 0, 100000);

        hists.push_back(edep);
        hists.push_back(fulledep);
    }
    ~EventCharge() = default;

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            TClonesArray *Hits = nullptr;
            ND::TSFGReconModule::TSFGHit *hit = nullptr;
            int Nhits;
            double sum = 0;
            reader.SetBranchAddres("NHits", &Nhits);
            reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);
            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                edep->Fill(hit->Charge);
                sum+= hit->Charge;
            }
            fulledep->Fill(sum);
            IncrementEventCount();
            
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
};


#endif