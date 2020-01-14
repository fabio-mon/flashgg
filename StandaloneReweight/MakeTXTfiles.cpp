
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
  int Nnodes=0;
  if(!isHH)
    r = new SingleHReweighter::SingleHReweighter(conf,process);
  else
  {
    doubler = new DoubleHReweighter(conf);    
    Nnodes=doubler->GetNnodes();
  }

  //define the objects to store the yields in the category and the normalization
  int NUMev=0;
  float SUMev_fakeSM2017=0;
  map<float,map<float,float> > SUMev_klkt; // SUMev_klkt [kl] [kt]
  map<float , map<float,map<string,float> > > SUMev_cat_klkt; //SUMev_cat_klkt [kl] [kt] [treename]
  map<int,float> SUMev_node; // SUMev_node [node]
  map<int, map<string,float> > SUMev_cat_node; //SUMev_cat_node [node] [treename]

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
  for(int inode=0; inode<Nnodes; ++inode)
  {
    SUMev_node[inode]=0;
    for(auto treename : *treenames)
      SUMev_cat_node[inode][treename]=0;
  }
    
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
    /*
    float benchmark_reweight[15];
    if(isHH)
    {
      for(int ibench=0;ibench<12;++ibench)
	ingenchain->SetBranchAddress(Form("benchmark_reweight_%i",ibench),&benchmark_reweight[ibench]);
      ingenchain->SetBranchAddress("benchmark_reweight_SM",&benchmark_reweight[12]);
      ingenchain->SetBranchAddress("benchmark_reweight_box",&benchmark_reweight[13]);
      ingenchain->SetBranchAddress("benchmark_reweight_2017fake",&benchmark_reweight[14]);
    }
    */

    //loop over events
    cout<<"Reading tree "<<treename<<endl;
    Long64_t nentries = ingenchain->GetEntries();
    cout<<nentries<<" entries"<<endl;
    for(long ientry=0;ientry<nentries; ++ientry)
    //for(long ientry=0;ientry<nentries; ientry+=300)
    {
      ingenchain->GetEntry(ientry);
      ++NUMev;
      if(ientry%5000==0)
	cout<<"reading entry "<<ientry<<"\r"<<std::flush;

      //Compute the SM reweight for this event
      float reweightSM=1;
      if(isHH)
	reweightSM=doubler->getWeight(1, 1, genmHH, gencosthetaHH);
      else
	reweightSM=r->getWeight(genpTH1,1,1);

      SUMev_klkt[1][1] += MCweight * reweightSM;	  
      SUMev_cat_klkt[1][1][treename] += MCweight * reweightSM;

      if(SUMev_klkt[1][1] < 0)
      {
	cout<<"SUMev_klkt[1][1] < 0"<<endl;
	cout<<"MCweight="<<MCweight<<endl;
	cout<<"reweightSM="<<reweightSM<<endl;
	getchar();
      }

      
      //Recompute the benchmark reweights
      if(isHH)
	for(int inode=0; inode<Nnodes; ++inode)
        {
	  double benchmarkreweight = doubler->getWeight(inode,genmHH, gencosthetaHH);
	  SUMev_node[inode] += MCweight * benchmarkreweight;
	  SUMev_cat_node[inode][treename] += MCweight * benchmarkreweight;
	}
      

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

	  if(SUMev_klkt[kl][kt] < 0)
	  {
	    cout<<"SUMev_klkt["<<kl<<"]["<<kt<<"] < 0"<<endl;
	    cout<<"MCweight="<<MCweight<<endl;
	    cout<<"reweight="<<reweight<<endl;
	    getchar();
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

      //open the txt
      string outtxtname;
      if(isHH)
	outtxtname = Form("%s/reweighting_hh_node_SM_%i_kl_%.3f_kt_%.3f.txt",outtxt_folder.c_str(),GetYear(treenames->at(0)),kl,kt);
      else
	outtxtname = Form("%s/reweighting_%s_%i_kl_%.3f_kt_%.3f.txt",outtxt_folder.c_str(),process.c_str(),GetYear(treenames->at(0)),kl,kt);
      outtxt.open(outtxtname);

      //write the first line which is the list of the tags
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
      
      //write the second line which is the change of the yield in each category
      for(auto treename : *treenames)
      {
	if(treename.find("NoTag") != string::npos)
	  continue;
	cout<<"SUMev_cat_klkt[1][1][treename]="<<SUMev_cat_klkt[1][1][treename]<<endl;
	cout<<"SUMev_klkt[1][1]="<<SUMev_klkt[1][1]<<endl;
	cout<<"SUMev_cat_klkt[kl][kt][treename]="<<SUMev_cat_klkt[kl][kt][treename]<<endl;
	cout<<"SUMev_klkt[kl][kt]="<<SUMev_klkt[kl][kt]<<endl;
	float reweight_cat_SM = SUMev_cat_klkt[1][1][treename];
	float reweight_cat = SUMev_cat_klkt[kl][kt][treename];

	if(SUMev_klkt[kl][kt]==0)//it happens to hh when kt=0
	{
	  outtxt<<"0\t";
	  continue;
	}
	if(isHH)//double H reweight looses the absolute normalization, so I have to reapply it
	{
	  reweight_cat_SM /= SUMev_klkt[1][1];
	  reweight_cat    /= SUMev_klkt[kl][kt];
	}

	if(reweight_cat==0)//I don't have events of this process in this category (signalfit already knows so i simply not reweight)
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
  
  //create the txt files also for the nodes
  if(isHH)
  {
    for(int inode=0; inode<Nnodes; ++inode)
    {
      cout<<"node "<<inode<<endl;
      string outtxtname = Form("%s/reweighting_hh_benchmark_%i_%i.txt",outtxt_folder.c_str(),inode,GetYear(treenames->at(0)));
      outtxt.open(outtxtname);

      //write the first line which is the list of the tags
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
	cout<<"\t"<<treename<<endl;
	if(treename.find("NoTag") != string::npos)
	  continue;
	cout<<"SUMev_cat_klkt[1][1][treename]"<<SUMev_cat_klkt[1][1][treename]<<endl;
	cout<<"SUMev_cat_node[inode][treename]"<<SUMev_cat_node[inode][treename]<<endl;
	cout<<"SUMev_klkt[1][1]"<<SUMev_klkt[1][1]<<endl;
	cout<<"SUMev_node[inode]"<<SUMev_node[inode]<<endl;
	float reweight_cat_SM = SUMev_cat_klkt[1][1][treename];
	float reweight_cat = SUMev_cat_node[inode][treename];
	cout<<"1reweight_cat_SM"<<reweight_cat_SM<<endl;
	cout<<"1reweight_cat"<<reweight_cat<<endl;
	reweight_cat_SM /= SUMev_klkt[1][1];
	reweight_cat    /= SUMev_node[inode];
	cout<<"2reweight_cat_SM"<<reweight_cat_SM<<endl;
	cout<<"2reweight_cat"<<reweight_cat<<endl;
	if(reweight_cat==0)//I don't have events of this process in this category (signalfit already knows so i simply not reweight)
	{
	  outtxt<<"1\t";
	  continue;
	}

	reweight_cat /= reweight_cat_SM;
	cout<<"3reweight_cat"<<reweight_cat<<endl;
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
