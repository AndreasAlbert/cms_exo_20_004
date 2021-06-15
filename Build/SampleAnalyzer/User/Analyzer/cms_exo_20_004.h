#ifndef analysis_cms_exo_20_004_h
#define analysis_cms_exo_20_004_h
#include <random>
#include "SampleAnalyzer/Process/Analyzer/AnalyzerBase.h"

namespace MA5
{
class cms_exo_20_004 : public AnalyzerBase
{
  INIT_ANALYSIS(cms_exo_20_004,"cms_exo_20_004")

 public:
  virtual bool Initialize(const MA5::Configuration& cfg, const std::map<std::string,std::string>& parameters);
  virtual void Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files);
  virtual bool Execute(SampleFormat& sample, const EventFormat& event);
  void InitializeRegions();
 private:
  // Random number generation is needed to model
  // per-region event weights
  // (stochastic selection)
  std::random_device rand_device;
  std::uniform_real_distribution<> rand_dist;
  std::mt19937 rand_eng;
};
}

#endif