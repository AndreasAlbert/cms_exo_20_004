
#include <stdlib.h>
#include "SampleAnalyzer/User/Analyzer/cms_exo_20_004.h"
using namespace MA5;
using namespace std;

// -----------------------------------------------------------------------------
// Initialize
// function called one time at the beginning of the analysis
// -----------------------------------------------------------------------------

vector<int> monojet_bins = {
  250,  280,  310,  340,  370,  400,  430,  470,  510, 550,  590,  640,  690,  740,  790,  840,  900,  960, 1020, 1090, 1160, 1250, 99999,
};
vector<int> monov_bins = {
  250, 300, 350, 400, 500, 600, 750, 99999
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

bool has_two_quark_daughters(MCParticleFormat & particle) {
  int n_quark_daughters = 0;
  for(auto * daughter : particle.daughters()){
    int pdg = fabs(daughter->pdgid());
    if(pdg==0 or pdg>7) {
      return false;
    } else {
      n_quark_daughters++;
    }
  }
  return n_quark_daughters==2;
}
vector<MCParticleFormat> select_gen_vqq(vector<MCParticleFormat> gen_particles) {
  vector<MCParticleFormat> gen_vqq;
  for( auto & part : gen_particles ){
    int pdg = fabs(part.pdgid());
    bool valid_pdg = (pdg==22) or (pdg==23);
    if(not valid_pdg){
      continue;
    }
    if(not has_two_quark_daughters(part)){
      continue;
    }
  gen_vqq.push_back(part);
  }
  return gen_vqq;
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

bool jet_match_to_gen_vqq(RecJetFormat jet, vector<MCParticleFormat> gen_vqq, float dphi_max, float dpt_max){
  for(auto v : gen_vqq) {
    float dpt = jet.pt()/v.pt() - 1;
    if(dpt > dpt_max){
      continue;
    }
    float dphi = jet.dphi_0_pi(v);
    if(dphi>dphi_max){
      continue;
    }
    return true;
  }
  return false;
}

float fatjet_loose_tag_efficiency(float pt, bool gen_match){
  if (gen_match){
    return 0;
  } else {
    return 0.05;
  }
}

float fatjet_tight_tag_efficiency(float pt, bool gen_match){
  if (gen_match){
    return 0;
  } else {
    return 0.005;
  }
}

float fatjet_tag_efficiency(float pt, int tagger, bool gen_match) {
  if(tagger==0) {
    return fatjet_loose_tag_efficiency(pt, gen_match);
  } else if (tagger==1) {
    return fatjet_tight_tag_efficiency(pt, gen_match);
  }
  throw;
}


bool cms_exo_20_004::Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters)
{
  cout << "BEGIN Initialization" << endl;

  vector<string> common = {
    "ptmiss",
    "veto_electron",
    "veto_muon",
    "veto_tau",
    "veto_btag",
    "veto_photon",
    "dphijm",
  };
  vector<string> monojet = {
    "leadak4_pt",
    "leadak4_eta",
  };
  vector<string> monov_loose = {
    "leadak8_pt",
    "leadak8_eta",
    "leadak8_mass",
    "leadak8_loose_tag",
  };
  vector<string> monov_tight = {
    "leadak8_pt",
    "leadak8_eta",
    "leadak8_mass",
    "leadak8_tight_tag",
  };
  
  
  // Create region selections
  Manager()->AddRegionSelection("monojet_SR");
  for(size_t ibin=0; ibin<monojet_bins.size()-1; ibin++) {
    char region_name[50];
    sprintf(region_name, "monojet_SR_bin%d", int(ibin));
    Manager()->AddRegionSelection(region_name);

    // Region specific cuts
    auto cut_name = region_name;
    Manager()->AddCut(cut_name,  region_name);
    for(auto cut : monojet) {
      Manager()->AddCut(cut, region_name);
    }
  }

  // Mono-V
  Manager()->AddRegionSelection("monov_loose_SR");
  Manager()->AddRegionSelection("monov_tight_SR");
  for(size_t ibin=0; ibin<monov_bins.size()-1; ibin++) {
    char cut_name[50];
    char region_name[50];

    sprintf(cut_name, "monov_SR_bin%d", int(ibin));
    
    // Loose
    sprintf(region_name, "monov_loose_SR_bin%d", int(ibin));
    Manager()->AddRegionSelection(region_name);
    Manager()->AddCut(cut_name,  region_name);
    for(auto cut : monov_loose) {
      Manager()->AddCut(cut, region_name);
    }

    // Tight
    sprintf(region_name, "monov_tight_SR_bin%d", int(ibin));
    Manager()->AddRegionSelection(region_name);
    Manager()->AddCut(cut_name,  region_name);
    for(auto cut : monov_tight) {
      Manager()->AddCut(cut, region_name);
    }
  }

  for(auto cut : monov_loose) {
    Manager()->AddCut(cut, "monov_loose_SR");
  }
  for(auto cut : monov_tight) {
    Manager()->AddCut(cut, "monov_tight_SR");
  }

  // Common cuts apply to all regions
  for(auto cut : common) {
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

  float const DPHI_JET_MET_MIN = 0.5;

  float const LEADAK4_ETA_MAX = 2.4;
  float const LEADAK4_PT_MIN = 100;
  float const LEADAK8_ETA_MAX = 2.4;
  float const LEADAK8_PT_MIN = 250;

  float const LEADAK8_MASS_MIN = 65;
  float const LEADAK8_MASS_MAX = 120;


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
  vector<RecTauFormat>    taus      = filter_collection<RecTauFormat   >(event.rec()->taus(),      CUT_TAU_PT_MIN,      CUT_TAU_ETA_MAX     );
  vector<RecPhotonFormat> photons   = filter_collection<RecPhotonFormat>(event.rec()->photons(),   CUT_PHO_PT_MIN,      CUT_PHO_ETA_MAX     );
  vector<RecJetFormat>    jets      = filter_collection<RecJetFormat   >(event.rec()->jets(),      20.0,                999                 );
  vector<RecJetFormat>    fatjets   = filter_collection<RecJetFormat   >(event.rec()->fatjets(),   200,                 2.4                 );
  
  vector<MCParticleFormat> gen_vqq = select_gen_vqq(event.mc()->particles());


  // Quantities to cut on

  // Event level quantities
  float dphijm      = dphi_jet_met(jets, event.rec()->MET(), 30);
  bool  veto_btag   = not has_b_tag(jets, CUT_BTAG_PT_MIN, CUT_BTAG_ETA_MAX);
  float ptmiss      = event.rec()->MET().pt();

  // Leading AK4 jet for monojet
  float leadak4_pt   = jets.size() > 0 ? jets.at(0).pt()  : 0;
  float leadak4_eta  = jets.size() > 0 ? jets.at(0).eta() : -999;

  // Leading AK8 jet for mono-V
  float leadak8_pt   = fatjets.size() > 0 ? fatjets.at(0).pt()  : 0;
  float leadak8_eta  = fatjets.size() > 0 ? fatjets.at(0).eta() : -999;
  float leadak8_mass = fatjets.size() > 0 ? fatjets.at(0).m()   : -999;


  // Tagging of fat jets is implemented via a probabilistic 
  // efficiency reweighting. 
  bool is_loose_tag = false;
  bool is_tight_tag = false;
  if(fatjets.size() > 0) {
      bool is_gen_matched =  jet_match_to_gen_vqq(fatjets.at(0), gen_vqq, 0.4, 0.25);
      float loose_eff = fatjet_tag_efficiency(fatjets.at(0).pt(), 0, is_gen_matched);
      float tight_eff = fatjet_tag_efficiency(fatjets.at(0).pt(), 1, is_gen_matched);
      float random = 0.01 * (rand() % 100); // Between 0 and 1
      is_tight_tag = tight_eff > random;
      is_loose_tag = (not is_tight_tag) and ((loose_eff-tight_eff) > random);
      // cout << "TIGHT " << random << " " << tight_eff  << " " << is_tight_tag << endl;
      // cout << "LOOSE " << random << " " << loose_eff  << " " << is_loose_tag << endl;
  }

  // Histogram filling
  Manager()->FillHisto("ptmiss", event.rec()->MET().pt() );
  
  std::map<string, bool> cuts;
  // Ptmiss binning for monojet
  for(size_t ibin=0; ibin<monojet_bins.size()-1; ibin++){
    char name[50];
    sprintf(name, "monojet_SR_bin%d", int(ibin));
    bool pass = (ptmiss > monojet_bins.at(ibin)) && (ptmiss < monojet_bins.at(ibin+1));
    cuts[name] = pass;
  }
  
  // Ptmiss binning for mono-V
  for(size_t ibin=0; ibin<monov_bins.size()-1; ibin++){
    char name[50];
    bool pass = (ptmiss > monov_bins.at(ibin)) && (ptmiss < monov_bins.at(ibin+1));
    sprintf(name, "monov_SR_bin%d", int(ibin));
    cuts[name] = pass;
  }


  cuts["ptmiss"]            = ptmiss > 250;
  cuts["veto_electron"]     = electrons.size() == 0;
  cuts["veto_muon"]         = muons.size() == 0;
  cuts["veto_tau"]          = taus.size() == 0;
  cuts["veto_btag"]         = veto_btag;
  cuts["veto_photon"]       = photons.size() == 0;
  cuts["dphijm"]            = dphijm>DPHI_JET_MET_MIN;
  cuts["leadak4_pt"]        = leadak4_pt>LEADAK4_PT_MIN;
  cuts["leadak4_eta"]       = fabs(leadak4_eta)<LEADAK4_ETA_MAX;
  cuts["leadak8_pt"]        = leadak8_pt>LEADAK8_PT_MIN;
  cuts["leadak8_eta"]       = fabs(leadak8_eta)<LEADAK8_ETA_MAX;
  cuts["leadak8_mass"]      = (leadak8_mass>LEADAK8_MASS_MIN) && (leadak8_mass<LEADAK8_MASS_MAX);
  cuts["leadak8_loose_tag"] = is_loose_tag;
  cuts["leadak8_tight_tag"] = is_tight_tag;

  // Sanity checks
  if(cuts["monov_SR_bin0"] and not cuts["ptmiss"]) {
    throw;
  }
  // for( auto const & item : cuts) {
  for( auto item = cuts.rbegin(); item != cuts.rend(); item++) {
    string cut_name = (*item).first;
    bool cut_passes = (*item).second;
    if (not Manager()->ApplyCut(cut_passes, cut_name)){
      return true;
    };
  }
  return true;
}

