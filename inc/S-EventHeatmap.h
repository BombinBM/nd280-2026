#ifndef EVENT_HEATMAP
#define EVENT_HEATMAP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <TH3D.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class EventHeatmap : public AnalysisStrategy
{
public:
    EventHeatmap() : AnalysisStrategy("EventHeatmap")
    {}    

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            TClonesArray *Hits = nullptr;
            ND::TSFGReconModule::TSFGHit *hit = nullptr, *endhit=nullptr, *beginhit=nullptr;
            int Nhits;
            double x,y,z,t, charge;
            float min_time = 1e6;
            std::string name = "Heatmap of event number ";
            reader.SetBranchAddres("NHits", &Nhits);
            reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);

            hists3D.push_back(new TH3D(name + eventCount + "\\time", "Event Heatmap", 194, -1000, 1000, 194, -3000, -1000, 58, -300, 300));
            hists3D.push_back(new TH3D(name + eventCount + "\\charge", "Event Heatmap", 194, -1000, 1000, 194, -3000, -1000, 58, -300, 300));

            for (int i = 0; i < Nhits; i++)
            {
                hit=dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                min_time = std::min(min_time,hit->Time);
            }

            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                x = hit->Position.X();
                y = hit->Position.Y();
                z = hit->Position.Z();
                t = hit->Time;
                charge = hit->Charge;
                // if (t == min_time)
                // {
                //     hists3D.back()->Fill(x,z,y,1000);    
                // }
                // else
                if(charge > 100 && t - min_time < 500)
                {
                    hists3D[2*eventCount]->Fill(x,z,y,t - min_time);
                    hists3D[2*eventCount+1]->Fill(x,z,y,charge);
                }
            }

            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << "\n";
        }
    }
};

#endif