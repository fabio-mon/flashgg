#ifndef flashgg_SingleHReweight
#define flashgg_SingleHReweight

#include "flashgg/DataFormats/interface/DiPhotonTagBase.h"
#include "flashgg/DataFormats/interface/Jet.h"
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "flashgg/Taggers/interface/FunctionHelpers.h"

namespace flashgg {

    class SingleHReweight: public DiPhotonTagBase, public reco::LeafCandidate
    {
    public:

        SingleHReweight()
        {};

        SingleHReweight(const std::vector<float> &kls, const std::vector<float> &kts, const std::vector<float> &weights):
            kls_(kls),
            kts_(kts),
            weights_(weights)
        {};

        ~SingleHReweight()
        {};

        int Appendweight(const float &kl, const float &kt, const float &weight) 
        {
            kls_.push_back(kl);
            kts_.push_back(kt);
            weights_.push_back(weight);
            return weights_.size();
        };

        const std::vector<float>  GetSingleHkl() const
        {
            return kls_;
        };

        const vector<float> GetSingleHkt() const
        {
            return kts_;
        };

        const vector<float> GetSingleHktklWeights() const
        {
            return weights_;
        };

    private:
        vector<float> kls_,kts_,weights_;
        
    };
}

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

