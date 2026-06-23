#ifndef DRAW_ENTRIES
#define DRAW_ENTRIES

#include <iostream>
#include <string>

#include <TH2D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class DrawEntries : public AnalysisStrategy
{
private:
    TClonesArray *Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr;
    int Nhits;

    TH2D* XYprojection = nullptr;
    TH2D* XZprojection = nullptr;
    TH2D* YZprojection = nullptr;
public:
    DrawEntries() : AnalysisStrategy("DrawEntries")
    {
        XYprojection = new TH2D("HitsXY", "HitsXY", 194, -1000, 1000, 58, -300, 300);
        XZprojection = new TH2D("HitsXZ", "HitsXZ", 194, -3000, -1000, 194, -1000, 1000);
        YZprojection = new TH2D("HitsYZ", "HitsYZ", 194, -3000, -1000, 58, -300, 300);
        
        hists2D.push_back(XZprojection);
        hists2D.push_back(XYprojection);
        hists2D.push_back(YZprojection);
    }
    ~DrawEntries() = default;

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        bool okN = reader.SetBranchAddres("NHits", &Nhits);
        bool okH = reader.SetBranchAddres("Hits", &Hits);
        if (!okN || !okH)
        {
            std::cerr << "DrawEntries::Begin: failed to bind NHits/Hits branches" << std::endl;
        }
        XYprojection->Reset();
        XZprojection->Reset();
        YZprojection->Reset();
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            // TClonesArray *Hits = nullptr;
            // ND::TSFGReconModule::TSFGHit *hit = nullptr;
            // int Nhits;

            // reader.SetBranchAddres("NHits", &Nhits);
            // reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);
            for (int it = 0; it < Nhits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                XYprojection->Fill(hit->Position.X(), hit->Position.Y(), hit->Charge);
                XZprojection->Fill(hit->Position.Z(), hit->Position.X(), hit->Charge);
                YZprojection->Fill(hit->Position.Z(), hit->Position.Y(), hit->Charge);
            }
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void End()
    {
        AnalysisStrategy::End();
        
    }

    void PrintStats() const override
    {
        AnalysisStrategy::PrintStats();
        if(GetEventCount() > 0)
        {
            std::cout << " Было введено " << GetEventCount() << " событий" << std::endl;
        }
    }
};

#endif