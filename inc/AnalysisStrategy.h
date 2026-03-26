#ifndef ANALYSIS_STRATEGY_H
#define ANALYSIS_STRATEGY_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TFile.h>

#include "FileReader.h"

class AnalysisStrategy
{
protected:
    std::chrono::_V2::system_clock::time_point start, end;
    std::chrono::duration<double, std::milli> diff;
    std::string stratName;
    std::vector<TH1*> hists;
    std::vector<TH2*> hists2D;
    int eventCount;
public:
    AnalysisStrategy(const std::string& name)
    :stratName(name), eventCount(0) 
    {
        std::cout << "  🔨 AnalysisStrategy конструктор: " << name << std::endl;
    }
    virtual ~AnalysisStrategy()
    {
        for (auto* h : hists)
        {
            delete h;
        }  
        for(auto* h2 : hists2D)
        {
            delete h2;
        }
    };

    virtual void Begin(FileReader& reader)
    {
        std::cout << " ▶ Начало стратегии: " << stratName << std::endl;
        eventCount = 0;
        start = std::chrono::high_resolution_clock::now();
    }

    virtual void ProcessEvent(FileReader& reader) = 0;

    virtual void ProcessRun(FileReader& reader)
    {
        try
        {
            for (int i = 0; i < reader.GetEntries(); i++)
            {
                ProcessEvent(reader);
            }
            
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    virtual void End()
    {
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        std::cout << "  ◀ Завершена стратегия: " << stratName 
            << " (обработано событий: " << eventCount << ")" << std::endl
            << "   ◀ Затраченное время: " << diff.count() << " мс" << std::endl;
        
    }

    virtual void Draw()
    {
        if (hists.empty() && hists2D.empty())
        {
            std::cout << "  Нет гистограмм для отображения" << std::endl;
            return;
        }

        TCanvas* c = new TCanvas(stratName.c_str(), stratName.c_str(), 800, 600);
        c->Divide(2, (hists.size() + hists2D.size() + 1) /2);

        for (size_t i = 0; i < hists.size(); i++)
        {
            c->cd(i+1);
            hists[i]->Draw();
        }
        for (size_t i = 0; i < hists2D.size(); i++)
        {
            c->cd(hists.size()+i+1);
            hists2D[i]->Draw();
        }
        c->Draw();
    }

    virtual void PrintStats() const
    {
        std::cout << "\n📊 Статистика по стратегии: " << stratName << std::endl;
        // std::cout << "   " << fDescription << std::endl;
        std::cout << "   Обработано событий: " << eventCount << std::endl;
        std::cout << "   ◀ Затраченное время: " << diff.count() << " мс" << std::endl;

    }

    virtual void Reset()
    {
        for (auto* h :hists)
        {
            h->Reset();
        }
        for (auto* h2 :hists2D)
        {
            h2->Reset();
        }
        eventCount = 0;
    }

    virtual void Write(const std::string filename = "output.root", const std::string option = "update") const
    {
        TFile *file = TFile::Open(filename.c_str(), option.c_str());
        for (auto* h : hists)
        {
            h->Write();
        }
        for (auto* h2 :hists2D)
        {
            h2->Write();
        }
        file->Close();
    }

    std::string GetName() const {return stratName;}
    int GetEventCount() const {return eventCount;}

    virtual void SetCuts(double x, double y, double z, double e) = 0;
    virtual void PrintCuts() = 0;

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