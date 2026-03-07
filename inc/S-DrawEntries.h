#ifndef DRAW_ENTRIES
#define DRAW_ENTRIES

#include <iostream>
#include <string>

#include <TH2D.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

class DrawEntries : public AnalysisStrategy
{
private:
    TH2D* XYprojection;
    TH2D* XZprojection;
    TH2D* YZprojection;
    int currEvent;
public:
    DrawEntries() 
    : AnalysisStrategy("DrawEntries")
    {
        XYprojection = new TH2D("Hits", "HitsXY", 194, -1000, -1000, 58, -300, 300);
        XZprojection = new TH2D("Hits", "HitsXZ", 194, -3000, -1000, 194, -1000, 1000);
        YZprojection = new TH2D("Hits", "HitsYZ", 194, -3000, -1000, 58, -300, 300);
    };
    ~DrawEntries();

    void Begin(FileReader& reader) override
    {
        currEvent = reader.GetValue<int>("EventID");
    };

    void ProcessEvent(FileReader& reader) override
    {
        try
        {
            float x = reader.GetValue<float>("X");
            float y = reader.GetValue<float>("Y");
            float z = reader.GetValue<float>("Z");
            int event = reader.GetValue<int>("EventID");

            XZprojection->Fill(z,x);
            YZprojection->Fill(z,y);
            XYprojection->Fill(x,y);
            if (event != currEvent)
            {
                currEvent = event;
                IncrementEventCount();
            }
            
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void PrintStats() const override
    {
        AnalysisStrategy::PrintStats();
        if(GetEventCount() > 0)
        {
            std::cout << " Было введено " << GetEventCount() << "событий" << std::endl;
        }
    }
};

#endif