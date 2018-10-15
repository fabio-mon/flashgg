import FWCore.ParameterSet.Config as cms

from flashgg.Taggers.flashggTags_cff import UnpackedJetCollectionVInputTag
from flashgg.Taggers.flashggTags_cff import flashggUnpackedJets

#recoJetCollections = UnpackedJetCollectionVInputTag

#print recoJetCollections

#for icoll,coll in enumerate(recoJetCollections):
flashggbRegressionProducer= cms.EDProducer('flashggbRegressionProducer94',
#                                               JetTag=coll,
                                           JetTag=cms.InputTag("flashggUnpackedJets","0"),
                                           bRegressionWeightfile= cms.untracked.string("flashgg/MetaData/data/DNN_models/breg_training_2017.pb"), #FIXME, check if this is the last model 
                                           y_mean = cms.untracked.double(1.0610932111740112),#FIXME, check if these are correct for these model, ask nadya 
                                           y_std =cms.untracked.double(0.39077115058898926) 
                                               )

