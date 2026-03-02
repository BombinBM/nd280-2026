void test()
{
Int_t NTraj; // Variable to store number of trajectories
// Variable to store trajectories
TClonesArray *Trajectories
   = new TClonesArray("ND::TTruthTrajectoriesModule::TTruthTrajectory", 1000);
TChain trajTreeChain; // Chain of files containing identical TTrees
trajTreeChain.SetName("TruthDir/Trajectories"); // Location of tree in file
//trajTreeChain.AddFile(inputfilename);           // Add file to chain

TString command = "ls *_anal_*.root";
TString res = gSystem->GetFromPipe(command);

trajTreeChain.AddFile(res);
trajTreeChain.SetBranchAddress("NTraj", &NTraj); // Where to store variable called &quot;NTraj&quot;
trajTreeChain.SetBranchAddress("Trajectories", &Trajectories); // Where to store Trajectories
int entries = trajTreeChain.GetEntries(); // Find number of entries (i.e. events) in file

auto gr = new TGraph2D(3);
gr->SetTitle("Truth trajectories");
int size = 0;
int N = 0;

for(int ient = 0; ient < entries; ient++) // loop over entries
{
  trajTreeChain.GetEntry(ient); // get this entry
  for( int it=0;it<NTraj;it++) //loop over number of trajectories
  {
    // get trajectory number it
    ND::TTruthTrajectoriesModule::TTruthTrajectory* trajectory
       = (ND::TTruthTrajectoriesModule::TTruthTrajectory*)   Trajectories->At(it);
    cout<<" mass of trajectory is " << trajectory->Mass<<endl; // print out the mass information
    size = trajectory->Points.size();
    cout<<" size of the trajectory is " << size <<endl;
	for(int dot = 0; dot < size; dot ++)
	{
		gr->SetPoint(N, trajectory->Points[dot].PositionX, trajectory->Points[dot].PositionY, trajectory->Points[dot].PositionZ); 
		N++;
	}
   cout<< " N = "<< N<< endl;  
}
}
gr->Draw("LINE");
gr->SaveAs("graph");

}

