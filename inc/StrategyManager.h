#ifndef STRATEGY_MANAGER_H
#define STRATEGY_MANAGER_H

#include <vector>

#include "AnalysisStrategy.h"

class StrategyManager
{
private:
    std::vector<std::unique_ptr<AnalysisStrategy>> strategies;
public:
    StrategyManager() {};

    void AddStrategy(std::unique_ptr<AnalysisStrategy> strategy)
    {
        if (!strategy)
        {
            std::cerr << "❌ StrategyManager: попытка добавить nullptr стратегию!" << std::endl;
            return;
        }
        
        std::cout << "Добавлена стратегия " << strategy->GetName() << std::endl;
        strategies.push_back(std::move(strategy));
    }

    bool RemoveStrategy(const std::string name)
    {
        auto it = std::find_if(strategies.begin(), strategies.end(), [&name](const auto& s) {return s->GetName() == name;});
        if(it != strategies.end())
        {
            std::cout << "Удалена стратегия" << name << std::endl;
            strategies.erase(it);
            return true;
        }
        std::cout << "Стратегия не найдена" << std::endl;
        return false;
    }

    AnalysisStrategy* GetStrategy(const std::string& name)
    {
        auto it = std::find_if(strategies.begin(), strategies.end(), [&name](const auto& s){return s->GetName() == name;});

        if (it != strategies.end())
        {
            return it->get();
        }
        return nullptr;
    }

    void BeginAll(FileReader& reader)
    {
        std::cout << "" << std::endl;
        for (auto& s : strategies)
        {
            s->Begin(reader);
        }
    }

    void ProcessEvent(FileReader& reader)
    {
        for(auto& s : strategies)
        {
            s->ProcessEvent(reader);
        }
    }

    void ProcessRun(FileReader& reader)
    {
        for(auto& s : strategies)
        {
            s->ProcessRun(reader);
        }
    }

    void EndAll()
    {
        std::cout << "" << std::endl;
        for(auto& s : strategies)
        {
            s->End();
        }
    }

    void PrintAllStats() const 
    {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "📈 ИТОГОВАЯ СТАТИСТИКА ПО ВСЕМ СТРАТЕГИЯМ" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
        for (const auto& s : strategies) {
            s->PrintStats();
            std::cout << std::string(40, '-') << std::endl;
        }
    }

    void DrawAll() const
    {
        for (const auto& s : strategies)
        {
            s->Draw();
        }
    }

    void WriteAll(const std::string filename, const std::string option = "recreate") const
    {
        for (const auto& s : strategies)
        {
            s->Write(filename.c_str(), option.c_str());
        }
    }

    void ResetAll()
    {
        for (auto& s: strategies)
        {
            s->Reset();
        }
    }

    std::vector<std::string> GetStrategyNames() const
    {
        std::vector<std::string> names;
        for(const auto& s: strategies)
        {
            names.push_back(s->GetName());
        }
        return names;
    }

    size_t Size() const {return strategies.size();}
};

#endif