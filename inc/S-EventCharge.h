#ifndef EVENT_CHARGE
#define EVENT_CHARGE

#include <iostream>
#include <string>

#include <TH1D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "config.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class EventCharge : public AnalysisStrategy
{
private:
    TClonesArray *Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr;
    int Nhits;

    TH1D *edep = nullptr;
    TH1D *fulledep = nullptr;
    TH1D *timehist = nullptr;
public:
    EventCharge() : AnalysisStrategy("EventCharge")
    {
        // std::cout << "EventCharge constructor started!" << std::endl;
        edep = new TH1D("Edep", "Edep, PE", 300, 0, 300);
        fulledep = new TH1D("FullEdep", "Full Edep, PE", 100, 0, 100000);
        timehist = new TH1D("TimeHist", "Time of Hits", 200, 0, 2000);
        hists.push_back(edep);
        hists.push_back(fulledep);
        hists.push_back(timehist);
    }
    ~EventCharge() = default;

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        // set branch addresses to member variables so pointers remain valid
        bool okN = reader.SetBranchAddres("NHits", &Nhits);
        bool okH = reader.SetBranchAddres("Hits", &Hits);
        if (!okN || !okH)
        {
            std::cerr << "EventCharge::Begin: failed to bind NHits/Hits branches" << std::endl;
        }
        edep->Reset();
        fulledep->Reset();
        timehist->Reset();
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            // TClonesArray *Hits = nullptr;
            // ND::TSFGReconModule::TSFGHit *hit = nullptr;
            // int Nhits;
            double dl, dt, sum = 0;
            float min_time = 1e6, max_time = 0;
            // reader.SetBranchAddres("NHits", &Nhits);
            // reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);
            TVector3 pos = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(0))->Position;
            float t0 = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(0))->Time;
            for (int i = 0; i < Nhits; i++)
            {
                hit=dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                dt = hit->Time - t0;
                dl = (hit->Position - pos).Mag();
                if (dl < MIN_LENGTH_BETWEEN_CUBES_CUT)
                {
                    min_time = std::min(min_time,hit->Time);
                    max_time = std::max(max_time,hit->Time);
                }
                t0 = hit->Time;
                pos = hit->Position; 
            }

            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                if (hit->Time - min_time < MIN_TIME_CUT && hit->Charge > MIN_CHARGE_CUT)
                {
                    edep->Fill(hit->Charge);
                    sum+= hit->Charge;
                    timehist->Fill(hit->Time - min_time);
                    // hit->Position.Print();
                    // std::cout << hit->Charge;
                }
            }
            fulledep->Fill(sum/2);
            IncrementEventCount();
            
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }
};


#endif