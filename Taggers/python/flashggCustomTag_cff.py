import FWCore.ParameterSet.Config as cms

from flashggDoubleHTag_cfi import flashggDoubleHTag
from flashggTags_cff       import flashggTTHHadronicTag
from flashggTags_cff       import flashggTTHLeptonicTag


flashggCustomTagSequence = cms.Sequence( flashggDoubleHTag+flashggTTHHadronicTag+flashggTTHLeptonicTag )
