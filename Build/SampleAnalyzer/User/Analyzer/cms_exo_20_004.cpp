#include "SampleAnalyzer/User/Analyzer/cms_exo_20_004.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------

vector<int> monojet_bins = {
  250,  280,  310,  340,  370,  400,  430,  470,  510, 550,  590,  640,  690,  740,  790,  840,  900,  960, 1020, 1090, 1160, 1250, 1400, 999999
};

template <class Type>
vector<const Type*> filter_collection(vector<Type> objects, float ptmin, float etamax){
  // Helper function to select subset of objects passing pt and eta selections
  vector<const Type*> filtered;
  for(auto obj:objects){
    if(obj.pt() < ptmin){
      continue;
    }
    if(fabs(obj.eta()) > etamax){
      continue;
    }
    filtered.push_back(&obj);
  }
  SORTER->sort(filtered, PTordering);
  return filtered;
}

bool has_b_tag(vector<const RecJetFormat*> jets, float ptmin, float etamax){
  // Returns true if a b jet is found
  for(auto jet : jets) {
    if (jet->btag() and jet->pt() > ptmin and fabs(jet->eta())<etamax) {
      return true;
    }
  }
  return false;
}

float dphi_jet_met(vector<const RecJetFormat*> jets, const ParticleBaseFormat & MET, float pt_min) {
  float dphijm = 5;
  for(int ijet=0; ijet < min(4, int(jets.size())); ijet++) {
    if(jets.at(ijet)->pt() < pt_min){
      continue;
    }
    dphijm = min(dphijm, jets.at(ijet)->dphi_0_pi(MET));
  }
  return dphijm;
}
bool cms_exo_20_004::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;

  vector<string> common = {
    "veto_electron",
    "veto_muon",
    "veto_tau",
    "veto_btag",
    "veto_photon",
    "dphijm",
    "ptmiss",
  };
  vector<string> monojet = {
    "leadak4_pt",
    "leadak4_eta",
    // "leadak4_id"
  };
  monojet.insert(monojet.begin(), common.begin(), common.end());

  // Create region selections
  Manager()->AddRegionSelection("monojet_SR");
  for(int ibin=0; ibin<23; ibin++) {
    char name[50];
    sprintf(name, "monojet_SR_bin%d", ibin);
    Manager()->AddRegionSelection(name);

    // Region specific cuts
    Manager()->AddCut(name,  name);
  }

  for(auto cut : monojet) {
    Manager()->AddCut(cut);
  }

  Manager()->AddHisto("ptmiss", 15,0,1400, "monojet_SR");

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
  
  vector<const RecJetFormat*> jets, bjets;
  vector<const RecLeptonFormat*> muons, electrons;
  vector<const RecTauFormat*> taus;
  vector<const RecPhotonFormat*> photons;

  // Filtered collections
  electrons = filter_collection<RecLeptonFormat>(event.rec()->electrons(), CUT_ELECTRON_PT_MIN, CUT_ELECTRON_ETA_MAX);
  muons     = filter_collection<RecLeptonFormat>(event.rec()->muons(),     CUT_MUON_PT_MIN,     CUT_MUON_ETA_MAX    );
  taus      = filter_collection<RecTauFormat   >(event.rec()->taus(),      CUT_TAU_PT_MIN,      CUT_TAU_ETA_MAX     );
  photons   = filter_collection<RecPhotonFormat>(event.rec()->photons(),   CUT_PHO_PT_MIN,      CUT_PHO_ETA_MAX     );
  jets      = filter_collection<RecJetFormat   >(event.rec()->jets(),      20.0,                999                 );

  
  // Quantities to cut on
  float dphijm      = dphi_jet_met(jets, event.rec()->MET(), 30);
  float leadak4_pt  = jets.size() > 0 ? jets.at(0)->pt() : 0;
  float leadak4_eta = jets.size() > 0 ? jets.at(0)->eta() : -999;
  bool veto_btag = not has_b_tag(jets, CUT_BTAG_PT_MIN, CUT_BTAG_ETA_MAX);
  float ptmiss = event.rec()->MET().pt();



  // Histogram filling
  Manager()->FillHisto("ptmiss", event.rec()->MET().pt() );
  // Cut application
  for(int ibin=0; ibin<23; ibin++){
    char name[50];
    sprintf(name, "monojet_SR_bin%d", ibin);
    bool pass = (ptmiss > monojet_bins.at(ibin)) && (ptmiss < monojet_bins.at(ibin+1));
    // std::cout << ibin << " " << name << " " << event.rec()->MET().pt() << " " << pass << std::endl;
    if (not Manager()->ApplyCut(pass, name)) return true;
  }
  if (not Manager()->ApplyCut(electrons.size() == 0,             "veto_electron")) return true;
  if (not Manager()->ApplyCut(muons.size() == 0,                 "veto_muon")) return true;
  if (not Manager()->ApplyCut(taus.size() == 0,                  "veto_tau")) return true;
  if (not Manager()->ApplyCut(veto_btag,                         "veto_btag")) return true;
  if (not Manager()->ApplyCut(photons.size() == 0,               "veto_photon")) return true;
  if (not Manager()->ApplyCut(dphijm>DPHI_JET_MET_MIN,           "dphijm")) return true;
  if (not Manager()->ApplyCut(ptmiss>250,       "ptmiss")) return true;
  if (not Manager()->ApplyCut(leadak4_pt>LEADAK4_PT_MIN,         "leadak4_pt")) return true;
  if (not Manager()->ApplyCut(fabs(leadak4_eta)<LEADAK4_ETA_MAX, "leadak4_eta")) return true;


  return true;
}

