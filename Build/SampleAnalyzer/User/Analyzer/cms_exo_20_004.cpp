#include "SampleAnalyzer/User/Analyzer/cms_exo_20_004.h"
using namespace MA5;
using namespace std;
#include <cassert>
// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------

vector<int> monojet_bins = {
  250,  280,  310,  340,  370,  400,  430,  470,  510, 550,  590,  640,  690,  740,  790,  840,  900,  960, 1020, 1090, 1160, 1250, 99999,
};

template <class Type>
vector<Type> filter_collection(vector<Type> objects, float ptmin, float etamax){
  // Helper function to select subset of objects passing pt and eta selections
  vector<Type> filtered;
  for(auto & obj:objects){
    if(obj.pt() < ptmin){
      continue;
    }
    if(fabs(obj.eta()) > etamax){
      continue;
    }
    filtered.push_back(obj);
  }
  return filtered;
}

bool has_b_tag(vector<RecJetFormat> jets, float ptmin, float etamax){
  // Returns true if a b jet is found
  for(auto jet : jets) {
    if (jet.btag() and jet.pt() > ptmin and fabs(jet.eta())<etamax) {
      return true;
    }
  }
  return false;
}

float dphi_jet_met(vector<RecJetFormat> jets, const ParticleBaseFormat & MET, float pt_min) {
  float dphijm = 5;
  for(int ijet=0; ijet < min(4, int(jets.size())); ijet++) {
    if(jets.at(ijet).pt() < pt_min){
      continue;
    }
    dphijm = min(dphijm, jets.at(ijet).dphi_0_pi(MET));
  }
  return dphijm;
}

string monojet_bin_name(int ibin, int year){
    char name[50];
    if(ibin<0) {
      sprintf(name, "monojet_SR_%d", year);
    } else {
      sprintf(name, "monojet_SR_%d_bin%d", year, ibin);
    }
    return string(name);
}
void cms_exo_20_004::InitializeRegions() {
  for(auto year : {2016, 2017, 2018}){
    Manager()->AddRegionSelection(monojet_bin_name(-1, year));

    for(size_t ibin=0; ibin<monojet_bins.size()-1; ibin++) {
      Manager()->AddRegionSelection(monojet_bin_name(int(ibin), year));
    }
  }
}
bool cms_exo_20_004::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;

  InitializeRegions();

  vector<string> cut_order;
  vector<string> common = {
    "ptmiss",
    "veto_electron",
    "veto_muon",
    "veto_tau",
    "veto_btag",
    "veto_photon",
    "dphijm",
    "leadak4_pt",
    "leadak4_eta",
  };
  for(auto const & cut : common) {
    Manager()->AddCut(cut);
    cut_order.push_back(cut);
  }

  // Create region selections
  for(auto year : {2016,2017,2018}) {
    for(size_t ibin=0; ibin<monojet_bins.size()-1; ibin++) {
      // Region specific cuts
      auto region_name = monojet_bin_name(ibin, year);
      Manager()->AddCut(region_name,  region_name);
      cut_order.push_back(region_name);
    }
  }

  // HEM veto is for 2018 only
  string regions_2018[] = {
    "monojet_SR_2018",
    "monojet_SR_2018_bin0",
    "monojet_SR_2018_bin1",
    "monojet_SR_2018_bin2",
    "monojet_SR_2018_bin3",
    "monojet_SR_2018_bin4",
    "monojet_SR_2018_bin5",
    "monojet_SR_2018_bin6",
    "monojet_SR_2018_bin7",
    "monojet_SR_2018_bin8",
    "monojet_SR_2018_bin9",
    "monojet_SR_2018_bin10",
    "monojet_SR_2018_bin11",
    "monojet_SR_2018_bin12",
    "monojet_SR_2018_bin13",
    "monojet_SR_2018_bin14",
    "monojet_SR_2018_bin15",
    "monojet_SR_2018_bin16",
    "monojet_SR_2018_bin17",
    "monojet_SR_2018_bin18",
    "monojet_SR_2018_bin19",
    "monojet_SR_2018_bin20",
    "monojet_SR_2018_bin21",
  };
  Manager()->AddCut("hem_veto_jets",  regions_2018);
  cut_order.push_back("hem_veto_jets");
  Manager()->AddCut("hem_veto_met",  regions_2018);
  cut_order.push_back("hem_veto_met");

  // Sanity check: No duplicate cut definitions
  set<string> set_of_cuts( cut_order.begin(), cut_order.end() );
  assert(set_of_cuts.size() == cut_order.size());

  // Manager()->AddHisto("ptmiss", 15,0,1400, "monojet_SR");

  cout << "END   Initialization" << endl;
  return true;
}

// -----------------------------------------------------------------------------
// Finalize
// function called one time at the end of the analysis
// -----------------------------------------------------------------------------
void cms_exo_20_004::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  cout << "BEGIN Finalization" << endl;
  // saving histos
  cout << "END   Finalization" << endl;
}
/*
  In the 2018 data set, an eta-phi sector of
  the HCAL is broken. We therefore have to
  veto any event with a jet in this region
  for this part of the data set.
*/
bool hem_veto_jets(vector<RecJetFormat> jets) {
  for(auto const jet : jets) {
     if(jet.pt()<30){
       continue;
     }
     if(jet.eta()<-3.0 or jet.eta() > -1.3){
       continue;
     }
     if (jet.phi()<-1.57 or jet.phi()> -0.87) {
       continue;
     }
    return false;
  }
  return true;
}
/*
  In the 2018 data set, an eta-phi sector of
  the HCAL is broken. We therefore have to
  veto any event with where MET phi points 
  in this direction.
*/
bool hem_veto_met(float met_pt, float met_phi) {
  return (met_pt > 470) or (met_phi > -0.62) or (met_phi<-1.62);
}
// -----------------------------------------------------------------------------
// Execute
// function called each time one event is read
// -----------------------------------------------------------------------------
bool cms_exo_20_004::Execute(SampleFormat& sample, const EventFormat& event)
{
   // Cut definitions
  float const CUT_ELECTRON_PT_MIN  = 10;
  float const CUT_ELECTRON_ETA_MAX = 2.4;

  float const CUT_MUON_PT_MIN      = 10;
  float const CUT_MUON_ETA_MAX     = 2.5;

  float const CUT_TAU_PT_MIN      = 18;
  float const CUT_TAU_ETA_MAX     = 2.3;

  float const CUT_PHO_PT_MIN      = 15;
  float const CUT_PHO_ETA_MAX     = 2.5;
  
  float const CUT_BTAG_PT_MIN      = 20;
  float const CUT_BTAG_ETA_MAX     = 2.4;

  float const LEADAK4_ETA_MAX = 2.4;
  float const LEADAK4_PT_MIN = 100;
  float const DPHI_JET_MET_MIN = 0.5;

  // Event weight
  double weight=1.;
  if (!Configuration().IsNoEventWeight() && event.mc()!=0) {
    weight=event.mc()->weight();
  }

  Manager()->InitializeForNewEvent(weight);

  if (event.rec()==0) {return true;}
  
  // Filtered collections
  vector<RecLeptonFormat> electrons = filter_collection<RecLeptonFormat>(event.rec()->electrons(), CUT_ELECTRON_PT_MIN, CUT_ELECTRON_ETA_MAX);
  vector<RecLeptonFormat> muons     = filter_collection<RecLeptonFormat>(event.rec()->muons(),     CUT_MUON_PT_MIN,     CUT_MUON_ETA_MAX    );
  vector<RecTauFormat> taus      = filter_collection<RecTauFormat   >(event.rec()->taus(),      CUT_TAU_PT_MIN,      CUT_TAU_ETA_MAX     );
  vector<RecPhotonFormat> photons   = filter_collection<RecPhotonFormat>(event.rec()->photons(),   CUT_PHO_PT_MIN,      CUT_PHO_ETA_MAX     );
  vector<RecJetFormat> jets      = filter_collection<RecJetFormat   >(event.rec()->jets(),      20.0,                999                 );

  
  // Quantities to cut on
  float dphijm      = dphi_jet_met(jets, event.rec()->MET(), 30);
  float leadak4_pt  = jets.size() > 0 ? jets.at(0).pt() : 0;
  float leadak4_eta = jets.size() > 0 ? jets.at(0).eta() : -999;
  bool veto_btag = not has_b_tag(jets, CUT_BTAG_PT_MIN, CUT_BTAG_ETA_MAX);
  float met_pt = event.rec()->MET().pt();
  float met_phi = event.rec()->MET().phi();

  // Histogram filling
  // Manager()->FillHisto("ptmiss", event.rec()->MET().pt() );
  // Cut application
  if (not Manager()->ApplyCut(met_pt>250,       "ptmiss")) return true;
  if (not Manager()->ApplyCut(electrons.size() == 0,             "veto_electron")) return true;
  if (not Manager()->ApplyCut(muons.size() == 0,                 "veto_muon")) return true;
  if (not Manager()->ApplyCut(taus.size() == 0,                  "veto_tau")) return true;
  if (not Manager()->ApplyCut(veto_btag,                         "veto_btag")) return true;
  if (not Manager()->ApplyCut(photons.size() == 0,               "veto_photon")) return true;
  if (not Manager()->ApplyCut(dphijm>DPHI_JET_MET_MIN,           "dphijm")) return true;
  if (not Manager()->ApplyCut(leadak4_pt>LEADAK4_PT_MIN,         "leadak4_pt")) return true;
  if (not Manager()->ApplyCut(fabs(leadak4_eta)<LEADAK4_ETA_MAX, "leadak4_eta")) return true;

  for(auto year : {2016, 2017, 2018}) {
    for(size_t ibin=0; ibin<monojet_bins.size()-1; ibin++){
      auto cut_name = monojet_bin_name(ibin, year);
      bool pass = (met_pt > monojet_bins.at(ibin)) && (met_pt < monojet_bins.at(ibin+1));
      if (not Manager()->ApplyCut(pass, cut_name)) return true;
    }
  }
  
  // HEM veto (2018 only)
  if (not Manager()->ApplyCut(hem_veto_met(met_pt, met_phi), "hem_veto_met")) return true;
  if (not Manager()->ApplyCut(hem_veto_jets(jets), "hem_veto_jets")) return true;

  return true;
}

