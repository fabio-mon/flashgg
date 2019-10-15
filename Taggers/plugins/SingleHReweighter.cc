#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "flashgg/DataFormats/interface/DiPhotonCandidate.h"
#include "flashgg/DataFormats/interface/Jet.h"
#include "flashgg/DataFormats/interface/DiPhotonMVAResult.h"
#include "flashgg/DataFormats/interface/SingleHReweight.h"
#include "flashgg/DataFormats/interface/TagTruthBase.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "TLorentzVector.h"


#include <vector>
#include <algorithm>
#include "TH2F.h"
#include "TFile.h"

using namespace std;
using namespace edm;

namespace flashgg {

    class SingleHReweighter : public EDProducer
    {

    public:
        SingleHReweighter( const ParameterSet & );
    private:
        void produce( Event &, const EventSetup & ) override;
        float getWeight(const string &process, const float &pTH, const float &kl, const float &kt);
        int FindBin(const float &pTH);
        EDGetTokenT<View<reco::GenParticle> > genParticleToken_;
        int doReweight_;
        unsigned int Nkl_;
        float klmin_;
        float klmax_;
        unsigned int Nkt_;
        float ktmin_;
        float ktmax_;
        float dZH_;
        edm::ParameterSet C1_process_pTH_map_; // C1_process_pTH_map ["process_name"] [#pTHbin]  
        edm::ParameterSet EWK_process_pTH_map_; // EWK_process_pTH_map ["process_name"] [#pTHbin]  
    };

    SingleHReweighter::SingleHReweighter( const ParameterSet &iConfig ) :
        genParticleToken_( consumes<View<reco::GenParticle> >( iConfig.getParameter<InputTag> ( "GenParticleTag" ) ) ),
        doReweight_( iConfig.getParameter         <int>               ( "doReweight" ) ),
        Nkl_(iConfig.getParameter                 <unsigned int>      ( "Nkl" )),
        klmin_(iConfig.getParameter               <double>             ( "klmin" )),
        klmax_(iConfig.getParameter               <double>             ( "klmax" )),
        Nkt_(iConfig.getParameter                 <unsigned int>      ( "Nkt" )),
        ktmin_(iConfig.getParameter               <double>             ( "ktmin" )),
        ktmax_(iConfig.getParameter               <double>             ( "ktmax" )),
        dZH_(iConfig.getParameter                 <double>             ( "dZH" )),
        C1_process_pTH_map_(iConfig.getParameter  <edm::ParameterSet> ("C1_process_pTH_map")),
        EWK_process_pTH_map_(iConfig.getParameter <edm::ParameterSet> ("EWK_process_pTH_map"))
    {
        produces<float>("kl");
        produces<float>("kt");
        produces<float>("ktklWeights");
    }   

    int SingleHReweighter::FindBin(const float &pTH)
    {
        //to do
        return 0;
    }
    
    float SingleHReweighter::getWeight(const string &process, const float &pTH, const float &kl, const float &kt)
    {
        float w = 0.;
        int pTHbin = FindBin(pTH);
        double C1 = C1_process_pTH_map_.getParameter<vector<double> >(process.c_str())[pTHbin];
        double EWK = EWK_process_pTH_map_.getParameter<vector<double> >(process.c_str())[pTHbin];
        w = (kt*kt+(kl-1)*C1/EWK) / ((1-(kl*kl-1)*dZH_));
        return w;
    }

    void SingleHReweighter::produce( Event &evt, const EventSetup & )
    {
        // prepare output
        std::vector<edm::Ptr<reco::GenParticle> > selHiggses;
        Handle<View<reco::GenParticle> > genParticles;
        evt.getByToken( genParticleToken_, genParticles );
        for( unsigned int genLoop = 0 ; genLoop < genParticles->size(); genLoop++ ) {
            edm::Ptr<reco::GenParticle> genPar = genParticles->ptrAt(genLoop);
            if (selHiggses.size()>1) break;
            if (genPar->pdgId()==25 && genPar->isHardProcess()){
                selHiggses.push_back(genPar);
            }   
        }
       
        //figure out which single higgs process this MC event is
        //to do

        string process = "ttH";

        //find out pT(H) gen
        //to do
        float pTH = 50.;

        //compute the reweights for a given kl,kt granularity
        std::vector<float> weights;
        std::vector<float> kls;
        std::vector<float> kts;
        if (selHiggses.size()==1){
            for(unsigned int ikl=0; ikl<Nkl_; ++ikl){
                float kl = klmin_+ (ikl+0.5) * (klmax_-klmin_);
                for(unsigned int ikt=0;ikt<Nkt_; ++ikt){
                    float kt = ktmin_+ (ikt+0.5) * (ktmax_-ktmin_);
                    kls.push_back(kl);
                    kts.push_back(kt);
                    weights.push_back(getWeight(process,pTH,kl,kt));
                }
            }
        }
        
        //prepare output object
        SingleHReweight singlehrew_obj(kls, kts, weights);

        //add the collection to the event
        std::unique_ptr<float>  final_weights( new float( weights[Nkl_]) );        
        std::unique_ptr<float>  final_kls(     new float(     kls[Nkl_]) );        
        std::unique_ptr<float>  final_kts(     new float(     kts[Nkl_]) );        
        evt.put( std::move(final_weights) , "ktklWeights");
        evt.put( std::move(final_kls)     , "kl");
        evt.put( std::move(final_kts)     , "kt");
    }
}



typedef flashgg::SingleHReweighter FlashggSingleHReweighter;
DEFINE_FWK_MODULE( FlashggSingleHReweighter );
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
