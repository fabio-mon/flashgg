
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

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
string GetCatName(string treename);
int    GetYear(string treename);

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
  map<string,TChain*> ingenchain_map;
  map<string,TChain*> inrecochain_map;
  for(auto treename : *treenames)
  {
    ingenchain_map[treename]  = new TChain(("gen_"+treename).c_str(),"");
    inrecochain_map[treename] = new TChain(("reco_"+treename).c_str(),"");
    for(auto filename : infilenames)
    {
      ingenchain_map[treename] -> Add((filename+"/genDiphotonDumper/trees/"+treename).c_str());
      inrecochain_map[treename] -> Add((filename+"/tagsDumper/trees/"+treename).c_str());
    }
  }

  //configure the reweighter
  cout<<"Creating reweighter object"<<endl;
  string process = IdentifyProcess(treenames->at(0));
  cout<<"Process is "<<process<<endl;
  bool isHH = (process=="hh");

  //Define h reweighter objects
  DoubleHReweighter *doubler;
  SingleHReweighter::SingleHReweighter *r;
  if(!isHH)
    r = new SingleHReweighter::SingleHReweighter(conf,process);
  else
    doubler = new DoubleHReweighter(conf);    

  //define the objects to store the yields in the category and the normalization
  int NUMev=0;
  float SUMev_fakeSM2017=0;
  map<float,map<float,float> > SUMev_klkt; // SUMev_klkt [kl] [kt]
  map<float , map<float,map<string,float> > > SUMev_cat_klkt; //SUMev_cat_klkt [kl] [kt] [treename]

  //Initialize map content just as precaution
  //force the presence of SM case
  SUMev_klkt[1][1]=0;
  for(auto treename : *treenames)
    SUMev_cat_klkt[1][1][treename]=0;
  for(int ikl=0; ikl<Nkl; ++ikl)
  {
    float kl = klmin + (ikl+0.5)*(klmax-klmin)/Nkl;
    for(int ikt=0; ikt<Nkt; ++ikt)
    {
      float kt = ktmin + (ikt+0.5)*(ktmax-ktmin)/Nkt;
      SUMev_klkt[kl][kt]=0;
      for(auto treename : *treenames)
	SUMev_cat_klkt[kl][kt][treename]=0;
    }
  }
    
  //open the files
  //TFile *newfile = new TFile(newfilename.c_str(),"RECREATE");
  //TDirectory* newdir = newfile->mkdir("tagsDumper");
  //newdir = newdir->mkdir("trees");
  for(auto &ingenchain_element : ingenchain_map)
  {
    string treename = ingenchain_element.first;
    TChain *ingenchain = ingenchain_element.second;
    cout<<"Starting to run on "<<treename<<endl;

    bool isNoTag = treename.find("NoTag")!=string::npos ;
    //branch the gen tree
    cout<<"branch ingenchain"<<endl;
    float genpTH1=1;
    float genpTH2=1;
    float genmHH;
    float gencosthetaHH;
    unsigned genrun;
    unsigned long genevent;
    float MCweight;
    //ingenchain->SetBranchAddress("ptH1",&genpTH1);    
    //ingenchain->SetBranchAddress("ptH2",&genpTH2);    
    ingenchain->SetBranchAddress("mhh",&genmHH);
    ingenchain->SetBranchAddress("absCosThetaStar_CS",&gencosthetaHH);
    ingenchain->SetBranchAddress("run",&genrun);
    ingenchain->SetBranchAddress("event",&genevent);
    ingenchain->SetBranchAddress("weight",&MCweight);

    //branch the reco tree
    cout<<"branch inrecochain"<<endl;
    TChain *inrecochain=inrecochain_map[treename];
    //float CMS_hgg_mass;
    //float dZ;
    //float genAbsCosThetaStar_CS;
    //float genMhh;
    unsigned recorun;
    unsigned long recoevent;
    //float centralObjectWeight;
    if(!isNoTag)
    {
      //inrecochain->SetBranchAddress("centralObjectWeight",&centralObjectWeight);
      //inrecochain->SetBranchAddress("CMS_hgg_mass",&CMS_hgg_mass);
      //inrecochain->SetBranchAddress("dZ",&dZ);
      inrecochain->SetBranchAddress("run",&recorun);
      inrecochain->SetBranchAddress("event",&recoevent);
    }

    //add reco tree as friend to gen tree
    if(!isNoTag)
    {
      assert(inrecochain->GetEntries() == ingenchain->GetEntries());//Just a precaution 
      cout<<"Adding recochain as friend to genchain"<<endl;
      ingenchain->AddFriend(("reco_"+treename).c_str());
    }

    //Precaution to avoid crashes
    //if(ingenchain->GetEntries() == 0)
    //{
    //  cout<<"[ERROR]: tree "<<treename<<" is empty! --> switch to next tree"<<endl;
    //  continue;
    //}

    //create and branch the new tree
    //cout<<"Creating output tree"<<endl;
    //float benchmark_reweight_SM;
    //float benchmark_reweight_box;
    //float benchmark_reweight_2017fake;
    //float benchmark_reweight[12];
    //newdir->cd();
    //TTree *newtree = new TTree(treename.c_str(),treename.c_str());
    //newtree->Branch("ptH1",&genpTH1,"ptH1/F");    
    //newtree->Branch("ptH2",&genpTH2,"ptH2/F");    
    //newtree->Branch("mhh",&genmHH,"mhh/F");
    //newtree->Branch("absCosThetaStar_CS",&gencosthetaHH,"absCosThetaStar_CS/F");
    //newtree->Branch("run",&genrun,"run/i");
    //newtree->Branch("event",&genevent,"event/l");
    //newtree->Branch("weight",&MCweight,"weight/F");
    //newtree->Branch("CMS_hgg_mass",&CMS_hgg_mass,"CMS_hgg_mass/F");
    //newtree->Branch("centralObjectWeight",&centralObjectWeight,"centralObjectWeight/F");
    //newtree->Branch("dZ",&dZ,"dZ/F");
    //if(isHH)
    //{
    //  newtree->Branch("benchmark_reweight_SM",&benchmark_reweight_SM,"benchmark_reweight_SM/F");
    //  newtree->Branch("benchmark_reweight_box",&benchmark_reweight_box,"benchmark_reweight_box/F");
    //  newtree->Branch("benchmark_reweight_2017fake",&benchmark_reweight_2017fake,"benchmark_reweight_2017fake/F");
    //  for(int ibench=0;ibench<12;++ibench)
    //  newtree->Branch(Form("benchmark_reweight_%i",ibench),&benchmark_reweight[ibench],"benchmark_reweight_%i/F");
    //}


    //float* klktreweight = new float[Nkl*Nkt];
    //float* klarray = new float[Nkl*Nkt];
    //float* ktarray = new float[Nkl*Nkt];
    //newtree->Branch("klktreweight",klktreweight,Form("klktreweight[%i]/F",Nkl*Nkt));
    //newtree->Branch("klarray",klarray,Form("klarray[%i]/F",Nkl*Nkt));
    //newtree->Branch("ktarray",ktarray,Form("ktarray[%i]/F",Nkl*Nkt));


    //loop over events
    cout<<"Reading tree "<<treename<<endl;
    Long64_t nentries = ingenchain->GetEntries();
    cout<<nentries<<" entries"<<endl;
    for(long ientry=0;ientry<nentries; ++ientry)
      //for(long ientry=0;ientry<nentries; ientry+=5000)
    {
      ingenchain->GetEntry(ientry);
      ++NUMev;
      if(ientry%5000==0)
	cout<<"reading entry "<<ientry<<"\r"<<std::flush;

      //just to be sure that events in gen and reco tree are properly aligned
      if(!isNoTag)
      {
	assert(genrun==recorun);
	assert(genevent==recoevent);
      }


      //Compute the SM reweight for this event
      float reweightSM=1;
      if(isHH)
	reweightSM=doubler->getWeight(1, 1, genmHH, gencosthetaHH);
      else
	reweightSM=r->getWeight(genpTH1,1,1);

      SUMev_klkt[1][1] += MCweight * reweightSM;	  
      SUMev_cat_klkt[1][1][treename] += MCweight * reweightSM;

      //Recompute the benchmark reweights
      //if(isHH)
      //{
      //SUMev_fakeSM2017 += MCweight * doubler->getWeight(14,genmHH, gencosthetaHH);
      //benchmark_reweight_SM       = doubler->getWeight(12,genmHH, gencosthetaHH);
      //benchmark_reweight_box      = doubler->getWeight(13,genmHH, gencosthetaHH);
      //benchmark_reweight_2017fake = doubler->getWeight(14,genmHH, gencosthetaHH);
      //for(int ibench=0;ibench<12;++ibench)
      //  benchmark_reweight[ibench] = doubler->getWeight(ibench,genmHH, gencosthetaHH);
      //}

      //Loop over kl and kt
      for(int ikl=0; ikl<Nkl; ++ikl)
      {
	float kl = klmin + (ikl+0.5)*(klmax-klmin)/Nkl;
	for(int ikt=0; ikt<Nkt; ++ikt)
        {
	  float kt = ktmin + (ikt+0.5)*(ktmax-ktmin)/Nkt;
	  //Fill outtree branches
	  //klarray[ikl+Nkl*ikt] = kl;
	  //ktarray[ikl+Nkl*ikt] = kt;
	  float reweight=1;
	  if(isHH)
	    reweight=doubler->getWeight(kl, kt, genmHH, gencosthetaHH);
	  else
	    reweight=r->getWeight(genpTH1,kl,kt);
	  //klktreweight[ikl+Nkl*ikt] = reweight;

	  //Fill maps 
	  if(kl!=1 || kt!=1)//avoid double counting of SM
	  {
	    SUMev_klkt[kl][kt] += MCweight * reweight;
	    SUMev_cat_klkt[kl][kt][treename] += MCweight * reweight;
	  }
	}
      }
      //newtree->Fill();
    }
    cout<<"done"<<endl;
    //newdir->cd();
    //newtree->AutoSave();
    cout<<endl;
  }
  //cout<<"deleting newfile"<<endl;
  //delete newfile;
  cout<<"deleting ingenchains"<<endl;
  for(auto &ingenchain_element : ingenchain_map)
    delete ingenchain_element.second;

  //Open the output txt files
  ofstream outtxt;
  string outtxt_folder = conf.GetOpt<string> ("Output.txtfilefolder"); 
  if(isHH)
    outtxt_folder += Form("/hh_node_SM_%i/",GetYear(treenames->at(0)));
  else
    outtxt_folder += Form("/%s_%i/",process.c_str(),GetYear(treenames->at(0)));
  system(Form("mkdir -p %s",outtxt_folder.c_str()));

  cout<<"NUMev="<<NUMev<<endl;
  cout<<"Standard Model normalization = "<<SUMev_klkt[1][1]<<endl;
  cout<<"Standard Model normalization / 0.000079913385 = "<<SUMev_klkt[1][1]/0.000079913385<<endl;
  cout<<"fakeSM2017 normalization = "<<SUMev_fakeSM2017<<endl;
  cout<<"-------------------------------------------------------------------------------------------------"<<endl;
  
  for(int ikl=0; ikl<Nkl; ++ikl)
  {
    float kl = klmin + (ikl+0.5)*(klmax-klmin)/Nkl;
    for(int ikt=0; ikt<Nkt; ++ikt)
    {
      float kt = ktmin + (ikt+0.5)*(ktmax-ktmin)/Nkt;
      //cout<<"kl="<<kl<<"\tkt="<<kt<<endl;
      //cout<<"-------------------------------------------------------------------------------------------------"<<endl;

      string outtxtname;
      if(isHH)
	outtxtname = Form("%s/reweighting_hh_node_SM_%i_kl_%.3f_kt_%.3f.txt",outtxt_folder.c_str(),GetYear(treenames->at(0)),kl,kt);
      else
	outtxtname = Form("%s/reweighting_%s_%i_kl_%.3f_kt_%.3f.txt",outtxt_folder.c_str(),process.c_str(),GetYear(treenames->at(0)),kl,kt);
      outtxt.open(outtxtname);
      for(unsigned itreename=0; itreename!=treenames->size(); ++itreename)
      {
	if(treenames->at(itreename).find("NoTag") != string::npos)
	  continue;
	string catname = GetCatName(treenames->at(itreename));
	if(itreename != 0)
	  outtxt<<"\t";
	outtxt<<catname;
      }
      outtxt<<endl;
      
      for(auto treename : *treenames)
      {
	if(treename.find("NoTag") != string::npos)
	  continue;
	//cout<<"SUMev_cat_klkt[1][1][treename]="<<SUMev_cat_klkt[1][1][treename]<<endl;
	//cout<<"SUMev_klkt[1][1]="<<SUMev_klkt[1][1]<<endl;
	//cout<<"SUMev_cat_klkt[kl][kt][treename]="<<SUMev_cat_klkt[kl][kt][treename]<<endl;
	//cout<<"SUMev_klkt[kl][kt]="<<SUMev_klkt[kl][kt]<<endl;
	float reweight_cat_SM = SUMev_cat_klkt[1][1][treename];
	float reweight_cat = SUMev_cat_klkt[kl][kt][treename];
	if(isHH)//double H reweight looses the absolute normalization, so I have to reapply it
	{
	  reweight_cat_SM /= SUMev_klkt[1][1];
	  reweight_cat    /= SUMev_klkt[kl][kt];
	}
	if(reweight_cat==0)
	{
	  outtxt<<"1\t";
	  continue;
	}
	//cout<<"1reweight_cat="<<reweight_cat<<endl;
	reweight_cat /= reweight_cat_SM;
	//cout<<"2reweight_cat="<<reweight_cat<<endl;
	if(isHH)
	  reweight_cat *= doubler->getXSratio(kl,kt);
	//cout<<"3reweight_cat="<<reweight_cat<<endl;
        outtxt<<reweight_cat<<"\t";
      }
      outtxt<<endl;
      outtxt.close();
    }
  }

  if(!isHH)
    delete r;
  else
    delete doubler;

  return 0;

}

vector<string> * GetListOfTrees(const string &oldfilename)
{
  TFile *oldfile = new TFile(oldfilename.c_str(),"READ");
  oldfile->cd("genDiphotonDumper/trees");
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

string GetCatName(string treename)
{
  std::size_t pos = treename.rfind("13TeV");
  if(pos!=std::string::npos)
  {
    treename.replace(0,pos+6,"");
    return treename;
  }
  return string("");
}

int    GetYear(string treename)
{

  if(treename.find("2016") != std::string::npos)
    return 2016;
  if(treename.find("2017") != std::string::npos)
    return 2017;
  if(treename.find("2018") != std::string::npos)
    return 2018;

  return -1;  

}
