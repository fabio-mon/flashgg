import FWCore.ParameterSet.Config as cms

from flashggDoubleHTag_cfi      import flashggDoubleHTag
from flashggVBFDoubleHTag_cfi   import flashggVBFDoubleHTag
from flashggTags_cff            import flashggTTHHadronicTag
from flashggTags_cff            import flashggTTHLeptonicTag

flashggCustomTagSequence = cms.Sequence( flashggVBFDoubleHTag+flashggDoubleHTag+flashggTTHHadronicTag+flashggTTHLeptonicTag )
