
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "SingleHReweighter.h"
#include "DoubleHReweighter.h"
#include "CfgManager.h"
#include "CfgManagerT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TROOT.h"
using namespace std;
using namespace SingleHReweighter;

vector<string> * GetListOfTrees(const string &oldfilename);

int main(int argc, char** argv)
{
  CfgManager conf(argv[1]);
  int   Nkl          = conf.GetOpt<int>    ("Input.Nkl");
  float klmin        = conf.GetOpt<float>  ("Input.klmin");
  float klmax        = conf.GetOpt<float>  ("Input.klmax");
  int   Nkt          = conf.GetOpt<int>    ("Input.Nkt");
  float ktmin        = conf.GetOpt<float>  ("Input.ktmin");
  float ktmax        = conf.GetOpt<float>  ("Input.ktmax");
  vector<string> infilenames = conf.GetOpt<vector<string> >("Input.filename");
  string newfilename = conf.GetOpt<string> ("Output.filename");
  
  //Get the name of the trees in the old file
  vector<string> *treenames = GetListOfTrees(infilenames.at(0));
  gDirectory->cd(0);
  
  //Create the input TChains
  map<string,TChain*> inchain_map;
  for(auto treename : *treenames)
  {
    inchain_map[treename] = new TChain();
    for(auto filename : infilenames)
      inchain_map[treename] -> Add((filename+"/tagsDumper/trees/"+treename).c_str());
  }

  //Define h reweighter objects
  DoubleHReweighter *doubler = new DoubleHReweighter(conf);
  SingleHReweighter::SingleHReweighter *r;
  
  //open the files
  TFile *newfile = new TFile(newfilename.c_str(),"RECREATE");
  TDirectory* newdir = newfile->mkdir("tagsDumper");
  newdir = newdir->mkdir("trees");
  for(auto &inchain_element : inchain_map)
  {
    string treename = inchain_element.first;
    TChain *inchain = inchain_element.second;
    float genpTH;
    float genmHH;
    float gencosthetaHH;
    inchain->SetBranchAddress("genpTH",&genpTH);    
    inchain->SetBranchAddress("mHH",&genmHH);
    inchain->SetBranchAddress("costhetaHH",&gencosthetaHH);

    //create and branch the new tree
    newdir->cd();
    TTree *newtree = inchain->CloneTree(0);
    float* klktreweight = new float[Nkl*Nkt];
    float* klarray = new float[Nkl*Nkt];
    float* ktarray = new float[Nkl*Nkt];
    newtree->Branch("klktreweight",klktreweight,Form("klktreweight[%i]/F",Nkl*Nkt));
    newtree->Branch("klarray",klarray,Form("klarray[%i]/F",Nkl*Nkt));
    newtree->Branch("ktarray",ktarray,Form("ktarray[%i]/F",Nkl*Nkt));

    //configure the reweighter
    string process = IdentifyProcess(treename);
    bool isHH = (process=="hh");
    if(!isHH)
      r = new SingleHReweighter::SingleHReweighter(conf,process);

    //loop over events
    cout<<"Reading tree "<<treename<<endl;
    Long64_t nentries = inchain->GetEntries();
    cout<<nentries<<" entries"<<endl;
    for(long ientry=0;ientry<nentries; ++ientry)
    {
      inchain->GetEntry(ientry);
      if(ientry%1000000==0)
	cout<<"reading entry "<<ientry<<"\r"<<std::flush;
      for(int ikl=0; ikl<Nkl; ++ikl)
      {
	float kl = klmin + (ikl+0.5)*(klmax-klmin)/Nkl;
	for(int ikt=0; ikt<Nkt; ++ikt)
        {
	  float kt = ktmin + (ikt+0.5)*(ktmax-ktmin)/Nkt;
	  //cout<<"kl="<<kl<<endl;
	  //cout<<"kt="<<kt<<endl;
	  //cout<<"klarray["<<ikl+Nkl*ikt<<"]="<<kl;
	  //cout<<"ktarray["<<ikl+Nkl*ikt<<"]="<<kt;
	  //cout<<"klktreweight["<<ikl+Nkl*ikt<<"]="<<r.getWeight(genpTH,kl,kt)<<endl;
	  klarray[ikl+Nkl*ikt] = kl;
	  ktarray[ikl+Nkl*ikt] = kt;
	  float reweight=1;
	  if(isHH)
	    reweight=doubler->getWeight(kl, kt, genmHH, gencosthetaHH);
	  else
	      reweight=r->getWeight(genpTH,kl,kt);

	  klktreweight[ikl+Nkl*ikt] = reweight;
	}
      }
      newtree->Fill();
    }
    newdir->cd();
    newtree->AutoSave();
    if(!isHH)
      delete r;
    cout<<endl;
  }
  cout<<"deleting newfile"<<endl;
  delete newfile;
  cout<<"deleting inchains"<<endl;
  for(auto &inchain_element : inchain_map)
    delete inchain_element.second;

  return 0;

}

vector<string> * GetListOfTrees(const string &oldfilename)
{
  TFile *oldfile = new TFile(oldfilename.c_str(),"READ");
  oldfile->cd("tagsDumper/trees");
  TIter next(gDirectory->GetListOfKeys());
  TKey *key;
  vector<string> *treenames = new vector<string>;
  while ((key = (TKey*)next())) 
  {
    key->Print();
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TTree")) continue;
    //branching old tree
    TTree *oldtree = (TTree*)key->ReadObj();
    treenames->push_back(oldtree->GetName());
  }
  oldfile->Close();
  gDirectory->cd(0);
  return treenames;

}
