import FWCore.ParameterSet.Config as cms
from  flashgg.Systematics.flashggJetSystematics_cfi import jetSystematicsCustomize

class HHTTHcustomizer():
    """
    (HH->bbgg + TTH) combination process customizaton class
    """
    
    def __init__(self, process, customize, metaConditions):
        self.process = process
        self.customize = customize
        self.metaConditions = metaConditions
        self.tagList = [ 
            ["NoTag",0],
            ["TTHHadronicTag",4],
            ["TTHLeptonicTag",4],
            ["DoubleHTag",12]
        ]
        self.customizeTagSequence()

    def variablesToDump(self):
        variables = [
            'Mjj := mjj()',
            'Mjj := mjj()',
            'leadJetPt:=leadJetPt()',
            'subleadJetPt:=subleadJetPt()']

        var_workspace = [
            "eventNumber := eventNumber()"]

        if self.customize.processId != "Data":
            variables += [
                'btagReshapeWeight := weight("JetBTagReshapeWeightCentral")']

        if self.customize.dumpWorkspace :
            return var_workspace
        else :
            return variables

    def systematicVariables(self):
      systematicVariables=[
          "CMS_hgg_mass[160,100,180]:=diPhoton().mass",
          'btagReshapeWeight[100,-10.,10]:=weight("JetBTagReshapeWeightCentral")',
          'Mjj[120,70,190] := mjj()',
          'leadJetPt[100,0,1000]:=leadJetPt()',
          'subleadJetPt[100,10,300]:=subleadJetPt()']

      return systematicVariables

    def variablesToDumpData(self):
        variables = []
        if not(self.customize.dumpWorkspace):
            return self.variablesToDump()
        return variables

    def customizeSystematics(self,systlabels,jetsystlabels,metsystlabels):

       for s in metsystlabels:
           systlabels.remove(s)
       metsystlabels = []
       if self.metaConditions['bRegression']['useBRegressionJERsf'] :
           for s in jetsystlabels:
               if "JER" in s :
                   systlabels.remove(s)
                   jetsystlabels.remove(s)
           if self.customize.doSystematics:
               for direction in ["Up","Down"]:
                   jetsystlabels.append("JERbreg%s01sigma" % direction)
                   systlabels.append("JERbreg%s01sigma" % direction)
       return systlabels,jetsystlabels,metsystlabels

    def customizeTagSequence(self):

        #costumize HH 
        self.process.load("flashgg.Taggers.flashggCustomTag_cff")
        
        # customizing training file (with/wo Mjj) 
        training_type = 'with_Mjj' if self.customize.doubleHTagsUseMjj else 'wo_Mjj' 
        
        self.process.flashggDoubleHTag.MVAConfig.weights=cms.FileInPath(str(self.metaConditions["doubleHTag"]["weightsFile"][training_type]))  
        self.process.flashggDoubleHTag.MVAFlatteningFileName = cms.untracked.FileInPath(str(self.metaConditions["doubleHTag"]["MVAFlatteningFileName"][training_type]))
        if training_type == 'with_Mjj' :
            self.process.flashggDoubleHTag.MVABoundaries = cms.vdouble(0.44,0.67,0.79)
            self.process.flashggDoubleHTag.MXBoundaries = cms.vdouble(250.,385.,470.,640.,250.,345.,440.,515.,250.,330.,365.,545.)
            self.process.flashggDoubleHTag.ttHScoreThreshold = cms.double(0.26) #0.26
        elif training_type == 'wo_Mjj' :
            self.process.flashggDoubleHTag.MVAConfig.variables.pop(0) 
            self.process.flashggDoubleHTag.MVABoundaries = cms.vdouble(0.30,0.54, 0.75)
            self.process.flashggDoubleHTag.MXBoundaries = cms.vdouble(250., 395.,470.,585.,250.,345.,375.,540.,250.,330.,375.,530.)
            self.process.flashggDoubleHTag.ttHScoreThreshold = cms.double(0.) #0.26

        ## customize meta conditions
        self.process.flashggDoubleHTag.JetIDLevel=cms.string(str(self.metaConditions["doubleHTag"]["jetID"]))
        self.process.flashggDoubleHTag.MVAscaling = cms.double(self.metaConditions["doubleHTag"]["MVAscalingValue"])
        self.process.flashggDoubleHTag.dottHTagger = cms.bool(self.customize.doDoubleHttHKiller)
        self.process.flashggDoubleHTag.ttHWeightfile = cms.untracked.FileInPath(str(self.metaConditions["doubleHTag"]["ttHWeightfile"]))
        self.process.flashggDoubleHTag.ttHKiller_mean = cms.vdouble(self.metaConditions["doubleHTag"]["ttHKiller_mean"])
        self.process.flashggDoubleHTag.ttHKiller_std = cms.vdouble(self.metaConditions["doubleHTag"]["ttHKiller_std"])
        self.process.flashggDoubleHTag.ttHKiller_listmean = cms.vdouble(self.metaConditions["doubleHTag"]["ttHKiller_listmean"])
        self.process.flashggDoubleHTag.ttHKiller_liststd = cms.vdouble(self.metaConditions["doubleHTag"]["ttHKiller_liststd"])
        self.process.flashggDoubleHTag.MaxJetEta = cms.double(self.metaConditions["bTagSystematics"]["eta"])

        # remove all tags from the default tag sequence
        self.process.flashggTagSequence.remove(self.process.flashggVBFTag)
        self.process.flashggTagSequence.remove(self.process.flashggTTHLeptonicTag)
        self.process.flashggTagSequence.remove(self.process.flashggTTHHadronicTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHEtTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHLooseTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHTightTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHMetTag)
        self.process.flashggTagSequence.remove(self.process.flashggWHLeptonicTag)
        self.process.flashggTagSequence.remove(self.process.flashggZHLeptonicTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHLeptonicLooseTag)
        self.process.flashggTagSequence.remove(self.process.flashggVHHadronicTag)
        self.process.flashggTagSequence.remove(self.process.flashggVBFMVA)
        self.process.flashggTagSequence.remove(self.process.flashggVBFDiPhoDiJetMVA)
        self.process.flashggTagSequence.remove(self.process.flashggTTHDiLeptonTag)
        self.process.flashggTagSequence.remove(self.process.flashggUntagged)
        self.process.flashggTagSequence.remove(self.process.flashggUntagged)
        self.process.flashggTagSequence.remove(self.process.flashggTHQLeptonicTag)
        self.process.flashggTagSequence.remove(self.process.flashggDoubleHTag)

 
    def doubleHTagMerger(self,systlabels=[]):

        self.process.p.remove(self.process.flashggTagSorter)
        self.process.p.replace(self.process.flashggSystTagMerger,self.process.flashggCustomTagSequence*self.process.flashggTagSorter*self.process.flashggSystTagMerger)
        for systlabel in systlabels:
           if systlabel!='':
             self.process.p.remove(getattr(self.process,'flashggTagSorter'+systlabel))
             self.process.p.replace(self.process.flashggSystTagMerger,getattr(self.process, 'flashggTagSorter'+systlabel)*self.process.flashggSystTagMerger)

             setattr(getattr(self.process, 'flashggTagSorter'+systlabel), 'TagPriorityRanges', cms.VPSet( 
                 cms.PSet(TagName = cms.InputTag('flashggDoubleHTag', systlabel)), 
                 cms.PSet(TagName = cms.InputTag('flashggTTHLeptonicTag', systlabel)), 
                 cms.PSet(TagName = cms.InputTag('flashggTTHHadronicTag', systlabel))) 
             )
            #print 'from loop after:',process.flashggSystTagMerger.src


    def customizeRunSequence(self,systlabels,jetsystlabels,phosystlabels,metsystlabels):
#       if self.customize.doubleHTagsOnly: 
       self.doubleHTagMerger(systlabels)

       # Set lists of systematics for each tag
       for tag in ["flashggTTHLeptonicTag", "flashggTTHHadronicTag"]:
            getattr(self.process, tag).DiPhotonSuffixes = cms.vstring(phosystlabels)
            getattr(self.process, tag).JetsSuffixes = cms.vstring(jetsystlabels)
            getattr(self.process, tag).MetSuffixes = cms.vstring(metsystlabels)
            getattr(self.process, tag).ModifySystematicsWorkflow = cms.bool(True)
            getattr(self.process, tag).UseLargeMVAs = cms.bool(True) # enable memory-intensive MVAs

       if len(systlabels)>1 :
          getattr(self.process, "flashggDoubleHTag").JetsSuffixes = cms.vstring([systlabels[0]]+jetsystlabels)
          getattr(self.process, "flashggDoubleHTag").DiPhotonSuffixes = cms.vstring([systlabels[0]]+phosystlabels)

       if self.customize.doubleHReweight>0:
          self.addNodesReweighting()
    
       if self.customize.doDoubleHGenAnalysis:
          self.addGenAnalysis()

       print 'here we print the tag sequence after the Systematics.doubleHCustomize.doubleHTagRunSequence'
       print self.process.flashggTagSequence



    def addNodesReweighting(self):
        if self.customize.doubleHReweight > 0 :
            from flashgg.Taggers.flashggDoubleHReweight_cfi import flashggDoubleHReweight
            self.process.flashggDoubleHReweight = flashggDoubleHReweight
            self.process.flashggDoubleHReweight.doReweight = self.customize.doubleHReweight
            self.process.flashggDoubleHReweight.weightsFile = cms.untracked.FileInPath(str(self.metaConditions["doubleHTag"]["NodesReweightingFileName"]))
            self.process.p.replace(self.process.flashggDoubleHTag, self.process.flashggDoubleHReweight*self.process.flashggDoubleHTag)


    def addGenAnalysis(self):
        if self.customize.processId == "Data": 
            return 

        import flashgg.Taggers.dumperConfigTools as cfgTools
        ## load gen-level bbgg 
        self.process.load( "flashgg.MicroAOD.flashggGenDiPhotonDiBJetsSequence_cff" )

        ## match gen-level to reco tag
        self.process.load("flashgg.Taggers.flashggTaggedGenDiphotons_cfi")
        self.process.flashggTaggedGenDiphotons.src  = "flashggSelectedGenDiPhotonDiBJets"
        self.process.flashggTaggedGenDiphotons.tags = "flashggTagSorter"
        self.process.flashggTaggedGenDiphotons.remap = self.process.tagsDumper.classifierCfg.remap
        self.process.flashggTaggedGenDiphotons.ForceGenDiphotonProduction = self.customize.ForceGenDiphotonProduction

        ## prepare gen-level dumper
        self.process.load("flashgg.Taggers.genDiphotonDumper_cfi")
        self.process.genDiphotonDumper.dumpTrees = True
        self.process.genDiphotonDumper.dumpWorkspace = False
        self.process.genDiphotonDumper.src = "flashggTaggedGenDiphotons"

        from flashgg.Taggers.globalVariables_cff import globalVariables
        self.process.genDiphotonDumper.dumpGlobalVariables = True
        self.process.genDiphotonDumper.globalVariables = globalVariables

        genVariables = ["absCosThetaStar_CS := abs(getcosthetaHHgen())",
                        "mhh := getmHHgen()",
                        "ptH1 := getptH1gen()",
                        "ptH2 := getptH2gen()"
                       ]

        if not self.customize.ForceGenDiphotonProduction:
            genVariables += ["mgg := mass",
                             "mbb := dijet.mass",
                             
                             "leadPho_px := leadingPhoton.px",
                             "leadPho_py := leadingPhoton.py",
                             "leadPho_pz := leadingPhoton.pz",
                             "leadPho_e  := leadingPhoton.energy",
                             "subleadPho_px := subLeadingPhoton.px",
                             "subleadPho_py := subLeadingPhoton.py",
                             "subleadPho_pz := subLeadingPhoton.pz",
                             "subleadPho_e  := subLeadingPhoton.energy",
                             
                             "leadJet_px := leadingJet.px",
                             "leadJet_py := leadingJet.py",
                             "leadJet_pz := leadingJet.pz",
                             "leadJet_e  := leadingJet.energy",
                             "subleadJet_px := subLeadingJet.px",
                             "subleadJet_py := subLeadingJet.py",
                             "subleadJet_pz := subLeadingJet.pz",
                             "subleadJet_e  := subLeadingJet.energy",
                            ]
            
        if self.customize.doubleHReweight > 0: 
             for num in range(0,12):
                   genVariables += ["benchmark_reweight_%d := getHHbbggBenchmarkReweight(%d)"%(num,num)]
             genVariables += ["benchmark_reweight_SM := getHHbbggBenchmarkReweight(12)"]
             genVariables += ["benchmark_reweight_box := getHHbbggBenchmarkReweight(13)"]
             genVariables += ["benchmark_reweight_2017fake := getHHbbggBenchmarkReweight(14)"]

        ## define categories for gen-level dumper
        cfgTools.addCategory(self.process.genDiphotonDumper,  ## events with not reco-level tag
                             "NoTag", 'isTagged("flashggNoTag")',1,#"NoTag", 'isTagged("NoTag")',0,
                             variables=genVariables)

        for tag in self.tagList: ## tagged events
            tagName,subCats = tag
            if "NoTag" in tagName:
                continue
            # need to define all categories explicitely because cut-based classifiers does not look at sub-category number
            if subCats>0:
                for isub in xrange(subCats):
                    cfgTools.addCategory(self.process.genDiphotonDumper,
                                         "%s_%d" % ( tagName, isub ), 
                                         'isTagged("%s") && categoryNumber == %d' % (tagName, isub),0,
                                         variables=genVariables##+recoVariables
                    )
            else:
                    cfgTools.addCategory(self.process.genDiphotonDumper,
                                         tagName, 
                                         'isTagged("%s")' % (tagName),0,
                                         variables=genVariables##+recoVariables
                    )
                

        self.process.genp = cms.Path(self.process.flashggGenDiPhotonDiBJetsSequence*self.process.flashggTaggedGenDiphotons*self.process.genDiphotonDumper)
