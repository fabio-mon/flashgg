flashgg for HHbbgg
=======
Instuctions how to run HHbbgg code intergrated to the flashgg framework.

Main readme for flashgg can be found here : flashgg/README.md.

#### The most recent branch : hh_tag_ttH_22_02_2019 

First presentation of the new code from Summer 2018 can be found here :
https://indico.cern.ch/event/718608/contributions/3040484/attachments/1668225/2675178/micheli_HHbbgg_20180613.pdf
It explains how the framework is set up with its advantages. 

*Please, do not checkout the code based on any instuctions in this talk though, 
because since Summer 2018 the code changed a lot.*

### The up-to-date instuctions are summarized in this README.

#### The code for HHbbgg right now is run from CMSSW_9_4_9 for both 2016 and 2017 years.

For 924, starting from a clean area and checking out Nadya's branch :

 ```
cmsrel CMSSW_9_4_9
cd CMSSW_9_4_9/src
cmsenv
git cms-init
cd $CMSSW_BASE/src
git clone https://github.com/chernyavskaya/flashgg flashgg
cd flashgg     
git remote add chernyavskaya https://github.com/chernyavskaya/flashgg.git
git fetch chernyavskaya
git checkout -b branch_name --track chernyavskaya/name_of_the_branch
 
```
In the sourse file flashgg/setup_9_4_X.sh , comment out the following linesm you do not need them, because you check out the code directly from me instead of general flahgg:
https://github.com/chernyavskaya/flashgg/blob/hh_tag_94X_20181217/setup_9_4_X.sh#L27-L33
```
cd $CMSSW_BASE/src    
source flashgg/setup_9_4_X.sh
```

If everything now looks reasonable, you can build:
 ```
 cd $CMSSW_BASE/src
 scram b -j 3
 source flashgg/afterbuild_9_4_X.sh     
 ```
 Now you have to manually copy the Scales and Smearing file (it works without it in interactive mode, but to run the jobs you need to copy the file)
 ```
cp $CMSSW_BASE/external/$SCRAM_ARCH/data/RecoEgamma/ScalesSmearings/data/Run2017_17Nov2017_v1_ele_unc_scales.dat   $CMSSW_BASE/src/RecoEgamma/ScalesSmearings/data/
 ```
 
 
 ### How to run the code :
 ```
 fggRunJobs.py --load json/json.file -d output_dir Systematics/test/workspaceStd.py maxEvents=-1
 doubleHTagsOnly=True doDoubleHTag=True doBJetRegression=True doSystematics=False doPdfWeights=False
 dumpWorkspace=False dumpTrees=True 
 year='2017' PUyear='2017'
 puTarget='PutHereCorrectPUtarget'
 -H -P -n 100  --no-copy-proxy useAAA=1  -q whichqueue
 ```
 The code is run in exactly the same way for 2016 and 2017 years. All needed corrections, systematics, etc are picked up automatically,
 you only need to specify two flags specifying the year you wish to run on : 
 year='2016' PUyear='2016' or  year='2017' PUyear='2017' 
 
 #### To run on DATA:
 One has to specify:
```
 processId=Data processType=data
```
As well as add lumimask (the names of CMS data certification files can be found in jsons)
```
lumiMask=path/name.txt
```

  
  
#### For MC one has to specify the correct PU target to be used :
It can be found in these jsons :

**2016** : jsons/HHbbgg_MC_2016_80X_DiphotonBjets.json

**2017** : jsons/2017/HHbbgg_2017_94X_bkg.json

#### In flashgg, first the MicroAOD ntuples are produced from MINIAOD. We run the HHbbgg code on top of these MicroAOD.
In the json.json we specify the Campaign - which MicroAOD prodcution to use, as well as the names of the samples we want to run on. 
All jsons for 2016 and 2017 MC and data can be found here :
jsons

While all Campaigns with the file paths to each MC or data samples can be found here :
MetaData/data

### The code set up: 
The code is set up in a very flexible way, with almost nothing being hardcoded, but rather access from easily changeble config files.
#### Main config files :
* Main config for DoubleHTagger with all cuts, MVA weights files, categrozation, etc :
Taggers/python/flashggDoubleHTag_cfi.py
* b-jet energy regression config :
Taggers/python/flashggbRegressionProducer_cfi.py
* variables added to the trees/workspaces :
Systematics/python/doubleHCustomize.py

 
#### Categorization :
One can produce the trees already with categorization defined here :
https://github.com/chernyavskaya/flashgg/blob/hh_tag_94X_20181217/Taggers/python/flashggDoubleHTag_cfi.py#L56-L59
For this you need to set the flag either to True or False :
https://github.com/chernyavskaya/flashgg/blob/hh_tag_94X_20181130/Taggers/python/flashggDoubleHTag_cfi.py#L70

#### Nodes reweighting :
To reweight for different BSM benchmarks you need to specify the option:
```
doubleHReweight=1
```
By default it is set to -1, which means no reweighting is applied.

You have to prepare a json with only and exacly 12 BSM nodes for 2016 and 6 node for 2017. You should use the following jsons:
```
jsons/2017/HHbbgg_2017_94X_nodes_samename.json
jsons/HHbbgg_MC_2016_80X_nodes_samename.json
```
14 reweighting weights are saved in the trees : 12 for benchmark_reweight_NUM, also SM and box are present in the weights as well. The weight benchmark_reweight_NUM should be applied to obtain a benchmark number NUM. _Make sure that you preserve normalization of the nodes! It is not guranteed with this reweighting!_  To do that you just have to divide the weights for the nodes by the value from the following .json :
```
jsons/reweighting_normalization_18_03_2018.json
````

#### Events at generator level :
You can save also all events at generator level with several gen-leve quantities for photons and jets. 
Even events that do not pass preselection will be saved in a separate tree 'NoTag'
```
doDoubleHGenAnalysis=True
```

#### Met name change in 2017 MicroAOD production
There was a change in the name of the Met for new 2017 MicroAODs : 
If you wish to run on MicroAOD from campaign=RunIIFall17-3_2_0 or Hbbgg_Signal_SM_20181120, or after 3_2_0,
you have to do the following change in the code:
```
Mets->MetsCorr in these 2 files:
Systematics/python/flashggMetSystematics_cfi.py
Taggers/python/flashggDoubleHTag_cfi.py   ## this file has to be changed for SM 2017 as well
```
If you do not do it, the code will simply crash. To help navigate which MC samples are affected, I have created two separate campaigns for 2017 : with Mets and MetsCorr

#### MicroAOD campaigns
2016 : RunIIMoriond17_HHbbgg_breg_20180601
2017 : HHbbgg_MC2017_14032019_Mets and HHbbgg_MC2017_14032019_MetsCorr


 
 
