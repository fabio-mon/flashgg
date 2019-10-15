import FWCore.ParameterSet.Config as cms

#default values first
weightsFile = ""

flashggSingleHReweight = cms.EDProducer("FlashggSingleHReweighter",
                                        GenParticleTag = cms.InputTag( "flashggPrunedGenParticles" ), # to compute MC-truth info
                                        doReweight = cms.int32(-1),  #it is only used to specify if reweighting has to be done : targetNode >  0 - yes 
                                        Nkl = cms.uint32(20), 
                                        klmin = cms.double(-5), 
                                        klmax = cms.double(10), 
                                        Nkt = cms.uint32(10), 
                                        ktmin = cms.double(0.7), 
                                        ktmax = cms.double(1.3), 
                                        dZH   = cms.double(-1.536e-3),
                                        pTH_bin_map = cms.PSet(
                                            ttH = cms.vdouble(0.,1000.)
                                        ),
                                        C1_process_pTH_map = cms.PSet(
                                            ttH = cms.vdouble(3.51e-2)
                                        ),
                                        EWK_process_pTH_map = cms.PSet(
                                            ttH = cms.vdouble(1.014)
                                        )
                                    )
#reweight_producer = "flashggSingleHReweight"
