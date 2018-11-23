import FWCore.ParameterSet.Config as cms

import flashgg.Taggers.dumperConfigTools as cfgTools

from flashgg.Taggers.flashggTags_cff import UnpackedJetCollectionVInputTag
import flashgg.Taggers.PUJID_wps as pujid

from flashgg.Taggers.globalVariables_cff import globalVariables

flashggDoubleHTag = cms.EDProducer("FlashggDoubleHTagProducer",
                                   DiPhotonTag=cms.InputTag('flashggPreselectedDiPhotons'), # diphoton collection (will be replaced by systematics machinery at run time)
                                   JetTags= UnpackedJetCollectionVInputTag, # one jet per vertex
                                   GenParticleTag = cms.InputTag( "flashggPrunedGenParticles" ), # to compute MC-truth info
                                   SystLabel      = cms.string(""), # used by systematics machinery
                                   
                                   VetoConeSize   = cms.double(0.4),
                                   MinLeadPhoPt   = cms.double(1./3.),
                                   MinSubleadPhoPt   = cms.double(0.25),
                                 #  VetoConeSize   = cms.double(0.),
                                 #  MinLeadPhoPt   = cms.double(0.),
                                 #  MinSubleadPhoPt   = cms.double(0.),
                                   ScalingPtCuts = cms.bool(True),
                                   DoSigmaMDecorr =cms.untracked.uint32(1),#transformation of sigmaM/M
                                   SigmaMDecorrFile = cms.untracked.FileInPath("flashgg/Taggers/data/diphoMVA_sigmaMoMdecorr_split_Mgg40_180.root"),
                                   ApplyEGMPhotonID = cms.untracked.bool(False),
                                   PhotonIDCut = cms.double(0.2),#this is loose id for 2016
                                   PhotonElectronVeto =cms.untracked.vint32(1, 1), #0: Pho1, 1: Pho2
                                  # PhotonElectronVeto =cms.untracked.vint32(0, 0), #0: Pho1, 1: Pho2

                                  # MinJetPt   = cms.double(20.),
                                   MinJetPt   = cms.double(0.),
                                   MaxJetEta   = cms.double(2.5),
                                 #  MJJBoundaries = cms.vdouble(70.,190.),
                                   MJJBoundaries = cms.vdouble(0.,1000.),
                                  # BTagType = cms.untracked.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'), #string for btag algorithm
                                   BTagType = cms.untracked.string('pfDeepCSVJetTags:probb'), #string for btag algorithm
                                   UseJetID = cms.bool(True),
                                   JetIDLevel = cms.string('Loose'),

                                   MVABoundaries  = cms.vdouble(0.29,0.441, 0.724), # category boundaries for MVA
                                   MXBoundaries   = cms.vdouble(250., 354., 478., 560.), # .. and MX
                                   MJJBoundariesLower = cms.vdouble(98.0,95.0,97.0,96.0,95.0,95.0,95.0,95.0,95.0,95.0,95.0,95.0),#for each category following the convention cat0=MX0 MVA0, cat1=MX1 MVA0, cat2=MX2 MVA0....
                                   MJJBoundariesUpper = cms.vdouble(150.0,150.0,143.0,150.0,150.0,150.0,150.0,145.0,155.0,142.0,146.0,152.0),#for each category following the convention cat0=MX0 MVA0, cat1=MX1 MVA0, cat2=MX2 MVA0....
                                   MVAConfig = cms.PSet(variables=cms.VPSet(), # variables are added below
                                                        classifier=cms.string("BDT::bdt"), # classifier name
                                                        weights=cms.FileInPath("flashgg/Taggers/data/HHTagger/training_with_28_10_2018_common2017.weights.xml"), # path to TMVA weights
                                                        regression=cms.bool(False), # this is not a regression
                                                        multiclass=cms.bool(True), # this is multiclass 
                                                        multiclassSignalIdx=cms.int32(2), # this is multiclass index for Signal
                                                        ),

                                   doMVAFlattening=cms.bool(False),#do transformation of cumulative to make it flat
                                   doCategorization=cms.bool(False),#do categorization based on MVA x MX or only fill first tree with all events
                                   MVAFlatteningFileName=cms.untracked.FileInPath("flashgg/Taggers/data/HHTagger/cumulativeTransformation_2016_2017_20181028_common.root"),#FIXME, this should be optional, is it? 
                                   globalVariables=globalVariables
                                  ) 

cfgTools.addVariables(flashggDoubleHTag.MVAConfig.variables,
                      # here the syntax is VarNameInTMVA := expression
                      [#"leadingJet_bDis := leadJet().bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')",#FIXME make the btag type configurable?
                       #"subleadingJet_bDis := subleadJet().bDiscriminator('pfCombinedInclusiveSecondaryVertexV2BJetTags')",
                       "leadingJet_DeepCSV := leadJet().bDiscriminator('pfDeepCSVJetTags:probb')+leadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",#FIXME make the btag type configurable?
                       "subleadingJet_DeepCSV := subleadJet().bDiscriminator('pfDeepCSVJetTags:probb')+subleadJet().bDiscriminator('pfDeepCSVJetTags:probbb')",
                       "absCosThetaStar_CS := abs(getCosThetaStar_CS(6500))",#FIXME get energy from somewhere?
                       "absCosTheta_bb := abs(CosThetaAngles()[1])",
                       "absCosTheta_gg := abs(CosThetaAngles()[0])",
                       "diphotonCandidatePtOverdiHiggsM := diphotonPtOverM()",
                       "dijetCandidatePtOverdiHiggsM := dijetPtOverM()",
                       "customLeadingPhotonIDMVA := diPhoton.leadingView.phoIdMvaWrtChosenVtx",
                       "customSubLeadingPhotonIDMVA := diPhoton.subLeadingView.phoIdMvaWrtChosenVtx",
                       "leadingPhotonSigOverE := diPhoton.leadingPhoton.sigEOverE",
                       "subleadingPhotonSigOverE := diPhoton.subLeadingPhoton.sigEOverE",
                       "sigmaMOverMDecorr := getSigmaMDecorr()",
                       "PhoJetMinDr := getPhoJetMinDr()",
                       "rho := global.rho",
                       "(leadingJet_bRegNNResolution*1.4826) := leadJet().userFloat('bRegNNResolution')*1.4826",
                       "(subleadingJet_bRegNNResolution*1.4826) := subleadJet().userFloat('bRegNNResolution')*1.4826",
                       "(sigmaMJets*1.4826) := getSigmaMOverMJets()*1.4826",
                       "year := 0",
                       ]
                      )


