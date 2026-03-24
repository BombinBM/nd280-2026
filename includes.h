#include <iostream>
#include <chrono>

#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "TGraph2D.h"
#include "TH2F.h"


#include "inc/FileReader.h"
#include "inc/AnalysisStrategy.h"
#include "inc/StrategyManager.h"
#include "inc/S-DrawEntries.h"
#include "inc/S-EventCharge.h"
#include "inc/S-EventEnergy.h"
#include "inc/S-DrawFibers.h"

#include "test3/SFGAnalysis/ND__TSFGReconModule__TSFGHit.h"