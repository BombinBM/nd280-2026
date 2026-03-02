void read_anal(){


std::string name_in = "/home/bogdan/Desktop/VScode/nd280-2026/test3/oa_pg_mup_00000001-0001_z5juzaudoyei_anal_000_bsdv01_2.root";

TFile *_file0 = TFile::Open(name_in.c_str());
_file0->MakeProject("SFGAnalysis","ND::TSFGReconModule", "recreate++");


}

