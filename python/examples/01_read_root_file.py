#!/usr/bin/env python3
"""Пример 1 — чтение ROOT-файла и перечисление TTrees и ветвей.
Usage: python examples/01_read_root_file.py /path/to/file.root
"""
import sys
import ROOT
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import tqdm
# PDG codes muon: 13, antimuon: -13, electron: 11, positron: -11, pion+: 211, pion-: -211, kaon+: 321, kaon-: -321, proton: 2212, antiproton: -2212
def get_pdg_from_argv(input: str) -> str:
    input = str(input)
    cases = {
        "13": "muon",
        "-13": "antimuon",
        "11": "electron",
        "-11": "positron",
        "211": "pion+",
        "-211": "pion-",
        "321": "kaon+",
        "-321": "kaon-",
        "2212": "proton",
        "-2212": "antiproton"
    }
    return cases.get(input, "undefined particle")

def main(path, particle_pdg = "-13"):
    print(get_pdg_from_argv(particle_pdg))
    ROOT.gROOT.ProcessLine(".L /home/bogdan/Desktop/VScode/nd280-2026/test3/SFGAnalysis/SFGAnalysis.so")
    
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        print(f"Не удалось открыть файл: {path}")
        return

    dir = f.Get("ReconDir")

    keys = [k.GetName() for k in dir.GetListOfKeys() if k.GetClassName().startswith("TTree")]
    for k in keys:
        print(k, end='\t')
    if not keys:
        print("TTrees не найдены в файле.")
        return

    name = "SFG"
    tree = dir.Get(name)
    # print(f"Найден TTree: {name} (entries={tree.GetEntries()})")
    # print("Ветви:")
    # for b in tree.GetListOfBranches():
        # print(" -", b.GetName())

    read_arr = ROOT.TClonesArray("ND::TSFGReconModule::TSFGHit")
    tree.SetBranchAddress("Hits", read_arr)
    
    entries = []
    interested = []
    zeros = 0
    # Обязательно сделать проверку на количество хитов со временем больше ката и при большом количестве откидывать минимальное время и считать иначе, иначе будет слишком много нулевых событий
    for i in tqdm.tqdm(range(tree.GetEntries())):
        tree.GetEntry(i)
        min_time = float('inf')
        max_time = 0
        charge = 0
        pos = read_arr.At(0).Position
        for j in range(read_arr.GetEntries()):
            hit = read_arr.At(j)
            charge += hit.Charge
            if (hit.Position - pos).Mag() < 30:
                min_time = min(hiparticle_pdgt.Time, min_time)
                max_time = max(hit.Time, max_time)
        interested.append([0, charge / 2, 0, min_time, max_time])

    for i in tqdm.tqdm(range(tree.GetEntries())):
        tree.GetEntry(i)
        entered_hits = 0
        for j in range(read_arr.GetEntries()):
            hit = read_arr.At(j)

            entries.append([i+1, hit.Charge, hit.Time, hit.Position.X(), hit.Position.Y(), hit.Position.Z()])
            if hit.Charge > 80 and hit.Time - interested[i][3] < 500:
                entered_hits += 1
        interested[i][0] = entered_hits / 2
        if entered_hits > 0:
            interested[i][2] = interested[i][1] / entered_hits
        else:
            zeros += 1
    fulldata = pd.DataFrame(entries, columns=["event", "charge", "time", "pos_x", "pos_y", "pos_z"])
    # print(fulldata.head())
    # print(fulldata.describe())
    interested_data = pd.DataFrame(interested, columns=["entered_hits", "total_charge", "average_charge", "min_time", "max_time"])
    interested_data.insert(0, "PDG", particle_pdg)

    print(f"Процентов событий с нулевым количеством вошедших хитов: {zeros / tree.GetEntries() * 100:.3f}%")
    print(interested_data.head())
    print(interested_data.describe())
    interested_data.to_csv(f"{get_pdg_from_argv(particle_pdg)}_data.csv", index=False, mode="a", header= False)
    figure = plt.figure(figsize=(10, 6))
    ax = figure.add_subplot(projection='3d')
    ax.scatter(interested_data["entered_hits"], interested_data["total_charge"], interested_data["average_charge"], cmap='viridis')

    plt.suptitle("Charge distribution")
    plt.xlabel("Charge")
    plt.ylabel("Counts")
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python 01_read_root_file.py /path/to/file.root [particle_pdg]")
        sys.exit(1)
    particle_pdg = sys.argv[2] if len(sys.argv) > 2 else "-13"
    main(sys.argv[1], particle_pdg)
