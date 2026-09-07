#ifndef CUBES_EDEP
#define CUBES_EDEP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

#include <TH1D.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"

#include "config.h"

#include "ND__TSFGReconModule__TSFGHit.h"

class CubesEdep : public AnalysisStrategy
{
private:
    TClonesArray *Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr, *endhit=nullptr;
    int NHits = 0;

    TVector3 endpos;
    int Ncubes = MAX_CUBES;
    int Nhists = 0;
    std::string mode = "Charge";

public:
    CubesEdep() : AnalysisStrategy("CubesEdep")
    {
        std::string tmp, name = "Cube Number ";
        std::string title = mode + " deposition";

        hists2D.push_back(new TH2D("Edep", "Energy deposition", 250, 0, 2500, 150, 0, 1500));
        // hists2D.push_back(new TH2D("Edep", "Energy deposition", 250, 0, 2500, 150, 0, 15));


        for (int i = 0; i < Ncubes; i++)
        {
            tmp = name + i;
            hists.push_back(new TH1D(tmp.c_str(), title.c_str(), 150, 0, 1500));
            // hists.push_back(new TH1D(tmp.c_str(), title.c_str(), 150, 0, 15));
        }
        hists.push_back(new TH1D("TrackLength", "Track Length", 250, 0, 2500));
        hists.push_back(new TH1D("MuonEdep", "Muon Edep", 100, 0, 20));
        hists.push_back(new TH1D("EndposEdep", "Endpos Edep", 200, 0, 200));
        Nhists = hists.size() - 3;
    } 

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        
        bool okN = reader.SetBranchAddres("NHits", &NHits);
        bool okH = reader.SetBranchAddres("Hits", &Hits);
        // bool okN = reader.SetBranchAddres("NTrueHits", &NHits);
        // bool okH = reader.SetBranchAddres("TrueHits", &Hits);
        if (!okN || !okH)
        {
            std::cerr << "CubesEdep::Begin: failed to bind NHits/Hits branches" << std::endl;
        }
        for (auto hist : hists)
        {
            hist->Reset();
        }
        for(auto hist2D : hists2D)
        {
            hist2D->Reset();
        }
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            int Exact_Dist_Cubes = 0, Cube_Number = 0;
        
            float charge = 0, dist = 0, min_time = 1e6, max_charge = 0, min_dist = 1e6;
            double tracklength = 0;
            
            double muon_energy_sum = 0;  // Сумма энергии мюонов
            std::set<std::pair<float, int>> processed_muon_segments;  // Для отслеживания уникальных сегментов (время хита, индекс сегмента)
            float muP = 0;
            std::vector<int> pdgs;

            // reader.SetBranchAddres("NHits", &Nhits);
            // reader.SetBranchAddres("Hits", &Hits);
            reader.GetEntry(eventCount);
            
            // Проход 1: Суммируем энергию всех уникальных мюонов (pdg = -13)
            for (int i = 0; i < NHits; i++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                min_time = std::min(min_time, hit->Time);
                max_charge = std::max(max_charge, hit->Charge);
                
                pdgs = hit->HitSegTruePDG;
                
                // Ищем мюоны в этом хите и суммируем их энергию (только уникальные)
                for (size_t j = 0; j < pdgs.size(); j++)
                {
                    if (pdgs[j] == -13)  // Мюон найден
                    {
                        std::pair<float, int> segment_id = std::make_pair(hit->HitSegPosition[j].T(), j);  // Уникальный идентификатор: (время хита, индекс сегмента)
                        
                        // Проверяем, не обработали ли мы этот сегмент уже
                        if (processed_muon_segments.find(segment_id) == processed_muon_segments.end())
                        {
                            // muon_energy_sum += hit->HitSegTrueEdepo[j];
                            muP = hit->HitSegTrueP[j];
                            muon_energy_sum += sqrt(muP * muP + 105.658 * 105.658) - 105.658;  // Используем истинный импульс мюона
                            processed_muon_segments.insert(segment_id);
                            
                            // std::cout << "Event: " << eventCount 
                            //           << ", Time: " << hit->Time
                            //           << ", Segment: " << j
                            //           << ", Muon energy: " << sqrt(muP * muP + 105.658 * 105.658) - 105.658
                            //           << ", Total: " << muon_energy_sum << std::endl;
                        }
                        // else
                        {
                            // std::cout << "Event: " << eventCount 
                            //           << ", Time: " << hit->Time
                            //           << ", Segment: " << j
                            //           << " - DUPLICATE (уже обработан)" << std::endl;
                        }
                    }
                }
            }
            
            // Заполняем гистограмму только если найдены мюоны
            if (muon_energy_sum > 0)
            {
                hists[hists.size() - 2]->Fill(muon_energy_sum);
                std::cout << ">>> Event " << eventCount << " - Total UNIQUE muon energy: " << muon_energy_sum 
                          << " (найдено " << processed_muon_segments.size() << " уникальных сегментов)" << std::endl;
            }
            
            // Проход 2: Ищем точку с максимальным зарядом (endpos)
            for (int it = 0; it < NHits; it++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                if(hit->Charge == max_charge)
                {
                    endpos = hit->Position;
                    hists[hists.size() - 1]->Fill(hit->Charge);
                }
            }
            
            // Проход 3: Заполняем гистограмму 2D (расстояние vs заряд)
            for (int j = 0; j < NHits; j++)
            {
                hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(j));
                if (hit->Time - min_time < MIN_TIME_CUT && hit->Charge > MIN_CHARGE_CUT)
                {
                    hists2D[0]->Fill((endpos-hit->Position).Mag(), hit->Charge);
                }
            }
            
            while (true)
            {
                for (int i = 0; i < NHits; i++)
                {
                    hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(i));
                    
                    if ((float)(endpos - hit->Position).Mag() > dist && hit->Charge > MIN_CHARGE_CUT && hit->Time - min_time < MIN_TIME_CUT)
                    {
                        // std::cout << (float)(endpos - hit->Position).Mag() << '\t';
                        min_dist = std::min(min_dist, (float)(endpos - hit->Position).Mag());
                    }
                }
                if (min_dist == 1e6)
                {
                    break;
                }
                
                // std::cout << min_dist << "\t";
                for (int j = 0; j < NHits; j++)
                {
                    hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(j));
                    if ((float)(endpos - hit->Position).Mag() == min_dist && hit->Charge > MIN_CHARGE_CUT && hit->Time - min_time < MIN_TIME_CUT)
                    {
                        if (Cube_Number > Nhists - 1)
                        {
                            break;
                        }
                        
                        charge += hit->Charge;
                        Exact_Dist_Cubes++;
                        tracklength += min_dist - dist;
                        dist = min_dist;
                    }
                }
                if (Exact_Dist_Cubes != 0 && Cube_Number < Nhists - 1)
                {
                    charge = charge / Exact_Dist_Cubes;
                    hists[Cube_Number]->Fill(charge);
                    Cube_Number++;
                    charge = 0;
                    Exact_Dist_Cubes = 0;
                }
                
                min_dist = 1e6;
                // std::cout << min_dist << '\t' << dist << "\n";
                if (Cube_Number > Nhists - 1)
                {
                    break;
                }
                
            }
            if (eventCount % 100 == 0)
            {
                std::cout << "Event number " << eventCount << std::endl;
            }
            // std::cout << tracklength << '\t';
            hists[hists.size() - 3]->Fill(tracklength);
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << "\n";
        }
    }

    void SetMode(std::string input_mode) {mode = input_mode;}
    void SetNcubes(int N) {Ncubes = N;}
};

#endif