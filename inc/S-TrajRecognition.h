#ifndef TRAJECTORY_RECOGNITION
#define TRAJECTORY_RECOGNITION

#include <iostream>
#include <string>
#include <vector>

#include <TH1I.h>
#include <TH1D.h>
#include <TClonesArray.h>

#include "AnalysisStrategy.h"
#include "FileReader.h"
#include "config.h"

#include "ND__TSFGReconModule__TSFGHit.h"
#include "ND__TTruthTrajectoriesModule__TTruthTrajectory.h"
#include "ND__TTruthTrajectoriesModule__TTruthTrajectoryPoint.h"

class TrajRecognition : public AnalysisStrategy
{
private:
    TClonesArray* Hits = nullptr;
    ND::TSFGReconModule::TSFGHit *hit = nullptr;
    int NHits;

    TClonesArray* Trajectories = nullptr;
    ND::TTruthTrajectoriesModule::TTruthTrajectory* Trajectory = nullptr;
    std::vector<ND::TTruthTrajectoriesModule::TTruthTrajectoryPoint*> Points;
    int NTraj;

    TH1I* PDGS = nullptr;
    TH1D* LifeTime = nullptr;
    TH1D* MuonProduction = nullptr;
    TH1D* ElectronProduction = nullptr;
    TH1D* MuonEnergy = nullptr;

    std::string mode = "Recon";
public:
    TrajRecognition() : AnalysisStrategy("TrajRecognition")
    {
        PDGS = new TH1I("HitsPDG", "HitsPDG", 2000, -10000, 10000);
        LifeTime = new TH1D("Lifetime", "Initial particle lifetime", 50, 0, 100);
        MuonProduction = new TH1D("Muontime", "Muon production time", 50, 0, 100);
        ElectronProduction = new TH1D("Electrontime", "Electron production time", 600, 0, 3000);
        MuonEnergy = new TH1D("MuonEnergy", "Muon Energy", 100, 4, 5);
        hists.push_back(PDGS);
        hists.push_back(LifeTime);
        hists.push_back(MuonProduction);
        hists.push_back(ElectronProduction);
        hists.push_back(MuonEnergy);
    }

    ~TrajRecognition() = default;

    void Begin(FileReader &reader) override
    {
        AnalysisStrategy::Begin(reader);
        bool okH, okN;
        if (mode == "Recon")
        {
            bool okN = reader.SetBranchAddres("NHits", &NHits);
            bool okH = reader.SetBranchAddres("Hits", &Hits);
        }
        else if (mode == "True")
        {
            bool okN = reader.SetBranchAddres("Trajectories", &Trajectories);
            bool okH = reader.SetBranchAddres("NTraj", &NTraj);
        }
        else
        {
            std::cerr << "Mode is not setted, Recon and True is available!";
        }

        if (!okH || !okN)
        {
            std::cerr << "TrajRecognition::Begin: failed to bind branches" << std::endl;
            return;  // Выход, если привязка не удалась
        }
        PDGS->Reset();
        LifeTime->Reset();
        MuonProduction->Reset();
        ElectronProduction->Reset();
    }

    void ProcessEvent(FileReader &reader) override
    {
        try
        {
            float electron_time = 1e6, max_muon_time = 0, min_muon_time = 1e6, min_pion_time = 1e6, max_pion_time = 0;
            int nullhits = 0;
            std::vector<int> hitpdgs;
            if (mode == "Recon")
            {
                reader.GetEntry(eventCount);
                for (int it = 0; it < NHits; it++)
                {   
                    hit = dynamic_cast<ND::TSFGReconModule::TSFGHit*>(Hits->At(it));
                    if (!hit)
                    {
                        nullhits++;
                        continue;
                    }

                    hitpdgs = hit->HitSegTruePDG;
                    for (int& pdg : hitpdgs)
                    {                    
                        PDGS->Fill(pdg);

                        if (pdg == 211)
                        {
                            min_pion_time = std::min(min_pion_time, hit->Time);
                            max_pion_time = std::max(max_pion_time, hit->Time);
                        }
                    // if (pdg != 211 && hit->Time > min)
                    // {
                    //     max = std::min(max, hit->Time);
                    //     std::cout << pdg << '\n';
                    // }
                        if (pdg == -13)
                        {
                            min_muon_time = std::min(min_muon_time, hit->Time);
                            max_muon_time = std::max(max_muon_time, hit->Time);
                        }
                        if (pdg == -11)
                        {
                            electron_time = std::min(electron_time, hit->Time);
                        }
                    }
                    // std::cout << electron_time - min << '\t';
                    LifeTime->Fill(max_pion_time - min_pion_time);
                    if (min_muon_time != 1e6)
                    {
                        MuonProduction->Fill(max_muon_time - min_pion_time);
                        if (electron_time != 1e6)
                        {
                            ElectronProduction->Fill(electron_time - min_pion_time);
                        }
                    }
                }
                if (nullhits > 0)
                {
                    std::cout << "  Number of null Hits: " << nullhits << '\n';
                }
            }
            else if (mode == "True")
            {
                reader.GetEntry(eventCount);
                // std::cout << NTraj << '\t';
                for (int i = 0; i < NTraj; i++)
                {
                    Trajectory = dynamic_cast<ND::TTruthTrajectoriesModule::TTruthTrajectory*>(Trajectories->At(i));
                    PDGS->Fill(Trajectory->PDG);
                    // std::cout << Trajectory->PDG << '\t';
                    // std::cout << Trajectory->Points.size() << '\t';
                    ND::TTruthTrajectoriesModule::TTruthTrajectoryPoint point;
                    for (int i = 0; i < Trajectory->Points.size(); i++)
                    {
                        // std::cout << Trajectory->ParentID;
                        point = Trajectory->Points[i];
                        if (Trajectory->ParentID == 0)
                        {
                            min_pion_time = std::min(min_pion_time, point.PositionT);
                            max_pion_time = std::max(max_pion_time, point.PositionT);
                        }
                        if (Trajectory->PDG == -13)
                        {
                            min_muon_time = std::min(min_muon_time, point.PositionT);
                            max_muon_time = std::max(max_muon_time, point.PositionT);
                            MuonEnergy->Fill(Trajectory->InitMomentum.Energy() - 105.658); // Subtracting the muon rest mass in MeV/c^2
                        }
                    }
                    // std::cout << min_muon_time << '\t' << max_muon_time <<'\t';
                    if (min_pion_time != 1e6)
                    {
                        LifeTime->Fill(max_pion_time - min_pion_time);
                    }
                    if (min_muon_time != 1e6)
                    {
                        // std::cout << min_muon_time - min_pion_time << '\n';
                        MuonProduction->Fill(min_muon_time - min_pion_time);
                    }
                    
                }
                max_pion_time = 0;
                min_pion_time = 1e6;
                max_muon_time = 0;
                min_muon_time = 1e6;            
            }
            IncrementEventCount();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    // void End() {AnalysisStrategy::End();};

    bool SetMode(std::string input_mode)
    {
        std::cout << "  Mode is " << input_mode << '\n';
        if (input_mode == "True")
        {
            mode = "True";
            return true;
        }
        if (input_mode == "Recon")
        {
            mode = "Recon";
            return true;
        }
        return false;
    }
};

#endif