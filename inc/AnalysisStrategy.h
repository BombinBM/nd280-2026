#ifndef ANALYSIS_STRATEGY_H
#define ANALYSIS_STRATEGY_H

#include <iostream>
#include <string>
#include <vector>

#include <TH1.h>
#include <TCanvas.h>

#include "FileReader.h"

class AnalysisStrategy
{
private:
    std::string fileName;
    std::vector<TH1*> hists;
    int eventCount;
public:
    AnalysisStrategy(const std::string& name)
    :fileName(name), eventCount(0) {}
    virtual ~AnalysisStrategy()
    {
        for (auto* h : hists)
        {
            delete h;
        }  
    };

    virtual void Begin(FileReader& reader)
    {
        std::cout << " ▶ Начало стратегии: " << fileName << std::endl;
        eventCount = 0;
    }

    virtual void ProcessEvent(FileReader& reader) = 0;

    virtual void End()
    {
        std::cout << "  ◀ Завершена стратегия: " << fileName 
            << " (обработано событий: " << eventCount << ")" << std::endl;
    }

    virtual void Draw()
    {
        if (hists.empty())
        {
            std::cout << "  Нет гистограмм для отображения" << std::endl;
            return;
        }

        TCanvas* c = new TCanvas(fileName.c_str(), fileName.c_str(), 800, 600);
        c->Divide(2, (hists.size() + 1) /2);

        for (size_t i = 0; i < hists.size(); i++)
        {
            c->cd(i+1);
            hists[i]->Draw();
        }
    }

    virtual void PrintStats() const
    {
        std::cout << "\n📊 Статистика по стратегии: " << fileName << std::endl;
        // std::cout << "   " << fDescription << std::endl;
        std::cout << "   Обработано событий: " << eventCount << std::endl;
    }

    virtual void Reset()
    {
        for (auto* h :hists)
        {
            h->Reset();
        }
        eventCount = 0;
    }

    std::string GetName() const {return fileName;}
    int GetEventCount() const {return eventCount;}

    protected:
    template<typename T>
    T* CreateHist(T* hist)
    {
        hists.push_back(hist);
        return hist;
    }

    void IncrementEventCount() {eventCount++;}
};

#endif 