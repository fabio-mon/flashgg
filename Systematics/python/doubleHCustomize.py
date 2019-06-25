import FWCore.ParameterSet.Config as cms

def variablesToDump(customize):
    var_workspace = [
         #    "Mjj := dijet().M()",
        #"eventNumber := eventNumber()",
        #"ttHScore := ttHScore()",
        #"HHbbggMVA := MVA()"
    ]
    variables = [
        "leadingJet_bDis := leadJet().bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')",#FIXME make the btag type configurable?
             "subleadingJet_bDis := subleadJet().bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')",
             "leadingJet_DeepCSV := leadJet().bDiscriminator('pfDeepCSVJetTags:probb')+leadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",#FIXME make the btag type configurable?
             "subleadingJet_DeepCSV := subleadJet().bDiscriminator('pfDeepCSVJetTags:probb')+subleadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",
             "leadingJet_puJetIdMVA := leadJet().puJetIdMVA()",
             "subleadingJet_puJetIdMVA := subleadJet().puJetIdMVA()",
             "absCosThetaStar_CS := abs(getCosThetaStar_CS())",
             "absCosThetaStar_CS_old := abs(getCosThetaStar_CS_old(6500))",
             "absCosTheta_bb := abs(CosThetaAngles()[1])",
             "absCosTheta_gg := abs(CosThetaAngles()[0])",
             "diphotonCandidatePtOverdiHiggsM := diphotonPtOverM()",
             "dijetCandidatePtOverdiHiggsM := dijetPtOverM()",
             "customLeadingPhotonIDMVA := diPhoton.leadingView.phoIdMvaWrtChosenVtx",
             "customSubLeadingPhotonIDMVA := diPhoton.subLeadingView.phoIdMvaWrtChosenVtx",
             "EGMLeadingPhotonIDMVA := diPhoton.leadingPhoton.userFloat('EGMPhotonMVA')",
             "EGMSubLeadingPhotonIDMVA := diPhoton.subLeadingPhoton.userFloat('EGMPhotonMVA')",
             "leadingPhotonSigOverE := diPhoton.leadingPhoton.sigEOverE",
             "subleadingPhotonSigOverE := diPhoton.subLeadingPhoton.sigEOverE",
             "sigmaMOverM := sqrt(0.5*(diPhoton.leadingPhoton.sigEOverE*diPhoton.leadingPhoton.sigEOverE + diPhoton.subLeadingPhoton.sigEOverE*diPhoton.subLeadingPhoton.sigEOverE))",
             "sigmaMOverMDecorr := getSigmaMDecorr()",
             "PhoJetMinDr := getPhoJetMinDr()",#up to here input variables to MVA
             "HHbbggMVA := MVA()",
            # "HHbbggMVAprob0 := MVAprob()[0]",
             "MX := MX()",
             "genMhh := genMhh()",
             "Mjj := dijet().M()",
             "dijet_pt := dijet().pt",
             "dijet_eta := dijet().eta",
             "dijet_phi := dijet().phi",
             "diphoton_pt := diPhoton.pt",
             "diphoton_eta := diPhoton.eta",
             "diphoton_phi := diPhoton.phi",
             "leadingJet_btagWeight := leadJet.weight('JetBTagReshapeWeight') ",
             "subleadingJet_btagWeight := subleadJet.weight('JetBTagReshapeWeight') ",
 
             "diHiggs_pt := getdiHiggsP4().pt()",
             "diHiggs_mass := getdiHiggsP4().M()",
             "diHiggs_eta :=  getdiHiggsP4().eta()",
             "diHiggs_phi := getdiHiggsP4().phi()",
             "category := categoryNumber()",

             "leadingPhoton_pt := diPhoton.leadingPhoton.pt",
             "leadingPhoton_eta := diPhoton.leadingPhoton.eta",
             "leadingPhoton_phi := diPhoton.leadingPhoton.phi",
             "subleadingPhoton_pt := diPhoton.subLeadingPhoton.pt",
             "subleadingPhoton_eta := diPhoton.subLeadingPhoton.eta",
             "subleadingPhoton_phi := diPhoton.subLeadingPhoton.phi",

             "leadingJet_pt := leadJet().pt",
             "leadingJet_eta := leadJet().eta",
             "leadingJet_phi := leadJet().phi",
             "leadingJet_mass := leadJet().p4().M()",
             "leadingJet_hflav := leadJet().hadronFlavour()",
             "leadingJet_pflav := leadJet().partonFlavour()",

             "subleadingJet_pt := subleadJet().pt",
             "subleadingJet_eta := subleadJet().eta",
             "subleadingJet_phi := subleadJet().phi",
             "subleadingJet_mass := subleadJet().p4().M()",
             "subleadingJet_hflav := subleadJet().hadronFlavour()",
             "subleadingJet_pflav := subleadJet().partonFlavour()",
    ]
    if customize.doBJetRegression : variables +=[
             "leadingJet_bRegNNCorr := leadJet().userFloat('bRegNNCorr')",
             "leadingJet_bRegNNResolution := leadJet().userFloat('bRegNNResolution')",
             "subleadingJet_bRegNNCorr := subleadJet().userFloat('bRegNNCorr')",
             "subleadingJet_bRegNNResolution := subleadJet().userFloat('bRegNNResolution')",
             "sigmaMJets := getSigmaMOverMJets()"
    ]
    if customize.doubleHReweight > 0: 
         for num in range(0,12):  #12 benchmarks + 1 SM
		       variables += ["benchmark_reweight_%d := getBenchmarkReweight(%d)"%(num,num)]
		       var_workspace += ["benchmark_reweight_%d := getBenchmarkReweight(%d)"%(num,num)]
         variables += ["benchmark_reweight_SM := getBenchmarkReweight(12)"]
         variables += ["benchmark_reweight_box := getBenchmarkReweight(13)"]
         variables += ["benchmark_reweight_2017fake := getBenchmarkReweight(14)"]
         var_workspace += ["benchmark_reweight_SM := getBenchmarkReweight(12)"]
         var_workspace += ["benchmark_reweight_box := getBenchmarkReweight(13)"]
         var_workspace += ["benchmark_reweight_2017fake := getBenchmarkReweight(14)"]

    if customize.ttHKillerInputVariables : variables += [
            "ttH_sumET := sumET()",
            "ttH_MET := MET()",
            "ttH_phiMET := phiMET()",
            "ttH_dPhi1 := dPhi1()",
            "ttH_dPhi2 := dPhi2()",
            "ttH_PhoJetMinDr := PhoJetMinDr()",
            "ttH_njets := njets()",
            "ttH_Xtt0 := Xtt0()",
            "ttH_Xtt1 := Xtt1()",
            "ttH_pte1 := pte1()",
            "ttH_pte2 := pte2()",
            "ttH_ptmu1 := ptmu1()",
            "ttH_ptmu2 := ptmu2()",
            "ttH_ptdipho := ptdipho()",
            "ttH_etae1 := etae1()",
            "ttH_etae2 := etae2()",
            "ttH_etamu1 := etamu1()",
            "ttH_etamu2 := etamu2()",
            "ttH_etadipho := etadipho()",
            "ttH_phie1 := phie1()",
            "ttH_phie2 := phie2()",
            "ttH_phimu1 := phimu1()",
            "ttH_phimu2 := phimu2()",
            "ttH_phidipho := phidipho()",
            "ttH_fabs_CosThetaStar_CS := fabs_CosThetaStar_CS()",
            "ttH_fabs_CosTheta_bb := fabs_CosTheta_bb()",
            "ttH_ptjet1 := ptjet1()",
            "ttH_ptjet2 := ptjet2()",
            "ttH_etajet1 := etajet1()",
            "ttH_etajet2 := etajet2()",
            "ttH_phijet1 := phijet1()",
            "ttH_phijet2 := phijet2()",
            #"ttHScore := ttHScore()",
            ]
    
    
    if customize.doDoubleHttHKiller : variables +=[
            "ttHScore := ttHScore()",
    ]

    if customize.dumpWorkspace == False : return variables
    else : return var_workspace

def variablesToDumpData(customize):
   variables = [
           #  "leadingJet_DeepCSV := leadJet().bDiscriminator('pfDeepCSVJetTags:probb')+leadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",#FIXME make the btag type configurable?
           #  "subleadingJet_DeepCSV := subleadJet().bDiscriminator('pfDeepCSVJetTags:probb')+subleadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",
           #  "absCosThetaStar_CS := abs(getCosThetaStar_CS())",
           #  "absCosThetaStar_CS_old := abs(getCosThetaStar_CS_old(6500))",
           #  "absCosTheta_bb := abs(CosThetaAngles()[1])",
           #  "absCosTheta_gg := abs(CosThetaAngles()[0])",
           #  "diphotonCandidatePtOverdiHiggsM := diphotonPtOverM()",
           #  "dijetCandidatePtOverdiHiggsM := dijetPtOverM()",
           #  "customLeadingPhotonIDMVA := diPhoton.leadingView.phoIdMvaWrtChosenVtx",
           #  "customSubLeadingPhotonIDMVA := diPhoton.subLeadingView.phoIdMvaWrtChosenVtx",
           #  "leadingPhotonSigOverE := diPhoton.leadingPhoton.sigEOverE",
           #  "subleadingPhotonSigOverE := diPhoton.subLeadingPhoton.sigEOverE",
           #  "sigmaMOverM := sqrt(0.5*(diPhoton.leadingPhoton.sigEOverE*diPhoton.leadingPhoton.sigEOverE + diPhoton.subLeadingPhoton.sigEOverE*diPhoton.subLeadingPhoton.sigEOverE))",
           #  "PhoJetMinDr := getPhoJetMinDr()",#up to here input variables to MVA
           #  "leadingJet_bRegNNResolution := leadJet().userFloat('bRegNNResolution')",
           #  "subleadingJet_bRegNNResolution := subleadJet().userFloat('bRegNNResolution')",
           #  "sigmaMJets := getSigmaMOverMJets()",
           #  "HHbbggMVA := MVA()",
           #  "MX := MX()",
           #  "Mjj := dijet().M()",
           #  "eventNumber := eventNumber()",
           # "ttHScore := ttHScore()"
             ]
   return variables

def tagList(customize,process):
    return [ ["DoubleHTag",12] ]#12 is the number of categories


def customizeTagSequence(customize,process):
    process.load("flashgg.Taggers.flashggDoubleHTag_cff")
    from flashgg.Taggers.flashggTags_cff import UnpackedJetCollectionVInputTag

    ## customize here (regression, kin-fit, MVA...)
    #if customize.doBJetRegression : process.flashggDoubleHTag.JetTags = cms.VInputTag( ["bRegProducer%d" % icoll for icoll,coll in enumerate(UnpackedJetCollectionVInputTag) ] )
    #if customize.doBJetRegression : process.flashggDoubleHTag.JetTags = cms.VInputTag( "bRegProducer" )
    if customize.doBJetRegression : 
            jetTagsSystematics = cms.VInputTag()
            for icoll,coll in enumerate(UnpackedJetCollectionVInputTag):
                jetTagsSystematics.append(cms.InputTag("bRegProducer", str(icoll)))
            getattr(process, "flashggDoubleHTag").JetTags = jetTagsSystematics

    ## remove single Higgs tags

    if customize.doubleHTagsOnly:
        process.flashggTagSequence.remove(process.flashggVBFTag)
        process.flashggTagSequence.remove(process.flashggTTHLeptonicTag)
        process.flashggTagSequence.remove(process.flashggTTHHadronicTag)
        process.flashggTagSequence.remove(process.flashggVHEtTag)
        process.flashggTagSequence.remove(process.flashggVHLooseTag)
        process.flashggTagSequence.remove(process.flashggVHTightTag)
        process.flashggTagSequence.remove(process.flashggVHMetTag)
        process.flashggTagSequence.remove(process.flashggWHLeptonicTag)
        process.flashggTagSequence.remove(process.flashggZHLeptonicTag)
        process.flashggTagSequence.remove(process.flashggVHLeptonicLooseTag)
        process.flashggTagSequence.remove(process.flashggVHHadronicTag)
        process.flashggTagSequence.remove(process.flashggVBFMVA)
        process.flashggTagSequence.remove(process.flashggVBFDiPhoDiJetMVA)
        process.flashggTagSequence.remove(process.flashggTTHDiLeptonTag)


        process.flashggTagSequence.replace(process.flashggUntagged, process.flashggDoubleHTagSequence)   

def addNodesReweighting(customize,process):
    if customize.doubleHReweight > 0 :
        from flashgg.Taggers.flashggDoubleHReweight_cfi import flashggDoubleHReweight
        process.flashggDoubleHReweight = flashggDoubleHReweight
        process.p.replace(process.flashggDoubleHTag, process.flashggDoubleHReweight*process.flashggDoubleHTag)
        process.flashggDoubleHReweight.doReweight = customize.doubleHReweight
    

def addGenAnalysis(customize,process,tagList):
    if customize.processId == "Data": 
        return 
    
    import flashgg.Taggers.dumperConfigTools as cfgTools
    ## load gen-level bbgg 
    process.load( "flashgg.MicroAOD.flashggGenDiPhotonDiBJetsSequence_cff" )
        
    ## match gen-level to reco tag
    process.load("flashgg.Taggers.flashggTaggedGenDiphotons_cfi")
    process.flashggTaggedGenDiphotons.src  = "flashggSelectedGenDiPhotonDiBJets"
    process.flashggTaggedGenDiphotons.tags = "flashggTagSorter"
    process.flashggTaggedGenDiphotons.remap = process.tagsDumper.classifierCfg.remap
    
    ## prepare gen-level dumper
    process.load("flashgg.Taggers.genDiphotonDumper_cfi")
    process.genDiphotonDumper.dumpTrees = True
    process.genDiphotonDumper.dumpWorkspace = False
    process.genDiphotonDumper.src = "flashggTaggedGenDiphotons"
            
    from flashgg.Taggers.globalVariables_cff import globalVariables
    process.genDiphotonDumper.dumpGlobalVariables = True
    process.genDiphotonDumper.globalVariables = globalVariables
    
    genVariables = ["mgg := mass",
                    "mbb := dijet.mass",
                    "mhh := sqrt( pow(energy+dijet.energy,2) - pow(px+dijet.px,2) - pow(py+dijet.py,2) - pow(pz+dijet.pz,2))",                    

                    
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
    if customize.doubleHReweight > 0: 
         for num in range(0,12):
		       genVariables += ["benchmark_reweight_%d := getHHbbggBenchmarkReweight(%d)"%(num,num)]
         genVariables += ["benchmark_reweight_SM := getHHbbggBenchmarkReweight(12)"]
         genVariables += ["benchmark_reweight_box := getHHbbggBenchmarkReweight(13)"]
         genVariables += ["benchmark_reweight_2017fake := getHHbbggBenchmarkReweight(14)"]
    
    ## define categories for gen-level dumper
    cfgTools.addCategory(process.genDiphotonDumper,  ## events with not reco-level tag
                         "NoTag", 'isTagged("flashggNoTag")',1,
                         variables=genVariables,
                         )
    ## tagList = tagList(customize,process)
    
    for tag in tagList: ## tagged events
        tagName,subCats = tag
        # need to define all categories explicitely because cut-based classifiers does not look at sub-category number
        for isub in xrange(subCats):
            cfgTools.addCategory(process.genDiphotonDumper,
                                 "%s_%d" % ( tagName, isub ), 
                                 'isTagged("%s") && categoryNumber == %d' % (tagName, isub),0,
                                 variables=genVariables##+recoVariables
                                 )

    process.genp = cms.Path(process.flashggGenDiPhotonDiBJetsSequence*process.flashggTaggedGenDiphotons*process.genDiphotonDumper)
