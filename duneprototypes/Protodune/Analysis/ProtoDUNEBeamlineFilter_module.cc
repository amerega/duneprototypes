////////////////////////////////////////////////////////////////////////
//
// ProtoDUNEBeamlineFilter selects events based on reconstructed 
// beamline info
//
// 2018 Justin Hugon, jhugon@fnal.gov
//
////////////////////////////////////////////////////////////////////////

// Framework includes
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/ParameterSet.h"

// dunetpc includes
#include "dunetpc/dune/Protodune/Analysis/ProtoDUNEDataUtils.h"

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"

namespace protoana{
  class ProtoDUNEBeamlineFilter;
}

class protoana::ProtoDUNEBeamlineFilter : public art::EDFilter{
public:
  
  explicit ProtoDUNEBeamlineFilter(fhicl::ParameterSet const& pset);
  virtual ~ProtoDUNEBeamlineFilter();
  
  void beginJob();
  bool filter(art::Event& evt);
  void reconfigure(fhicl::ParameterSet const& pset);
  void endJob();
  
private:
  
  protoana::ProtoDUNEDataUtils fDataUtils;
  float fNominalBeamMomentum; // GeV/c
  bool fIsElectron;
  bool fIsMuon;
  bool fIsPion;
  bool fIsKaon;
  bool fIsProton;
  bool fAndParticles; // otherwise do or of the allowed particles

  // Histograms
  TH1F* fIsBeamTrigger;
  TH1F* fBITriggerAll;
  TH1F* fBITriggerPass;
  TH1F* fBIAndTimingMatchAll;
  TH1F* fBIAndTimingMatchPass;
  TH1F* fPossiblePartsAll;
  TH1F* fPossiblePartsPass;
  TH1F* fMomentumAll;
  TH1F* fMomentumPass;
  TH1F* fTOFAll;
  TH1F* fTOFPass;
  TH1F* fTOFChannelAll;
  TH1F* fTOFChannelPass;
  TH1F* fCKov0All;
  TH1F* fCKov0Pass;
  TH1F* fCKov1All;
  TH1F* fCKov1Pass;
  TH1F* fCKovAll;
  TH1F* fCKovPass;
  TH1F* fMassAll;
  TH1F* fMassPass;
};
  
//-----------------------------------------------------------------------
protoana::ProtoDUNEBeamlineFilter::ProtoDUNEBeamlineFilter(fhicl::ParameterSet const& pset):
  fDataUtils(pset.get<fhicl::ParameterSet>("DataUtils"))
{

  this->reconfigure(pset);
  
}

//-----------------------------------------------------------------------
protoana::ProtoDUNEBeamlineFilter::~ProtoDUNEBeamlineFilter(){}

//-----------------------------------------------------------------------
void protoana::ProtoDUNEBeamlineFilter::beginJob() {
  
  art::ServiceHandle<art::TFileService> tfs;
  fIsBeamTrigger = tfs->make<TH1F>("IsBeamTrigger", "Is the CTB trigger from the beamline?", 2,0,1);
  fBITriggerAll = tfs->make<TH1F>("BITriggerAll", "Beam Instrumentation Trigger for All Events", 3,-1,1);
  fBITriggerPass = tfs->make<TH1F>("BITriggerPass", "Beam Instrumentation Trigger for Passing Events", 3,-1,1);
  fBIAndTimingMatchAll = tfs->make<TH1F>("BIAndTimingMatchAll", "Beam Instrumentation & Timing Triggers Match for All Events", 2,0,1);
  fBIAndTimingMatchPass = tfs->make<TH1F>("BIAndTimingMatchPass", "Beam Instrumentation & Timing Triggers Match for Passing Events", 2,0,1);
  fPossiblePartsAll = tfs->make<TH1F>("PossiblePartsAll", "Possible Particles for All Events", 5,0,5);
  fPossiblePartsPass = tfs->make<TH1F>("PossiblePartsPass", "Possible Particles for Passing Events", 5,0,5);
  fMomentumAll = tfs->make<TH1F>("MomentumAll", "Momentum for All Events", 100,0,10);
  fMomentumPass = tfs->make<TH1F>("MomentumPass", "Momentum for Passing Events", 100,0,10);
  fTOFAll = tfs->make<TH1F>("TOFAll", "TOF for All Events", 350,-100,250);
  fTOFPass = tfs->make<TH1F>("TOFPass", "TOF for Passing Events", 350,-100,250);
  fTOFChannelAll = tfs->make<TH1F>("TOFChannelAll", "TOF Channel for All Events", 6,-1,5);
  fTOFChannelPass = tfs->make<TH1F>("TOFChannelPass", "TOF Channel  for Passing Events", 6,-1,5);
  fCKov0All = tfs->make<TH1F>("CKov0All", "Cherenkov 0 for All Events", 3,-1,2);
  fCKov0Pass = tfs->make<TH1F>("CKov0Pass", "Cherenkov 0 for Passing Events", 3,-1,2);
  fCKov1All = tfs->make<TH1F>("CKov1All", "Cherenkov 1 for All Events", 3,-1,2);
  fCKov1Pass = tfs->make<TH1F>("CKov1Pass", "Cherenkov 1 for Passing Events", 3,-1,2);
  fCKovAll = tfs->make<TH1F>("CKovAll", "Both Cherenkovs for All Events", 5,-1,4);
  fCKovPass = tfs->make<TH1F>("CKovPass", "Both Cherenkovs for Passing Events", 5,-1,4);
  fMassAll = tfs->make<TH1F>("MassAll", "Beamline Particle Mass for All Events", 200,0,5);
  fMassPass = tfs->make<TH1F>("MassPass", "Beamline Particle Mass for Passing Events", 200,0,5);
  
}

//-----------------------------------------------------------------------
void protoana::ProtoDUNEBeamlineFilter::reconfigure(fhicl::ParameterSet const& pset){
  fNominalBeamMomentum = pset.get<float>("NominalBeamMomentum"); // GeV/c
  fIsElectron = pset.get<bool>("IsElectron");
  fIsMuon = pset.get<bool>("IsMuon");
  fIsPion = pset.get<bool>("IsPion");
  fIsKaon = pset.get<bool>("IsKaon");
  fIsProton = pset.get<bool>("IsProton");
  fAndParticles = pset.get<bool>("AndParticles");
}

//-----------------------------------------------------------------------
bool protoana::ProtoDUNEBeamlineFilter::filter(art::Event& evt){

  if(fDataUtils.IsBeamTrigger(evt))
  {
    fIsBeamTrigger->Fill(1);
  }
  else
  {
    fIsBeamTrigger->Fill(0);
    return false;
  }

  const auto possibleParts = fDataUtils.GetBeamlineParticleID(evt,fNominalBeamMomentum);
  bool result = false;
  if(fAndParticles)
  {
    if(fIsElectron && !possibleParts.electron) result = false;
    else if(fIsMuon && !possibleParts.muon) result = false;
    else if(fIsPion && !possibleParts.pion) result = false;
    else if(fIsKaon && !possibleParts.kaon) result = false;
    else if(fIsProton && !possibleParts.proton) result = false;
    else result = true;
  }
  else // or
  {
    if(fIsElectron && possibleParts.electron) result = true;
    else if(fIsMuon && possibleParts.muon) result = true;
    else if(fIsPion && possibleParts.pion) result = true;
    else if(fIsKaon && possibleParts.kaon) result = true;
    else if(fIsProton && possibleParts.proton) result = true;
    else result = false;
  }

  if (possibleParts.electron) fPossiblePartsAll->Fill(0);
  if (possibleParts.muon)     fPossiblePartsAll->Fill(1);
  if (possibleParts.pion)     fPossiblePartsAll->Fill(2);
  if (possibleParts.kaon)     fPossiblePartsAll->Fill(3);
  if (possibleParts.proton)   fPossiblePartsAll->Fill(4);
  if(result)
  {
    if (possibleParts.electron) fPossiblePartsPass->Fill(0);
    if (possibleParts.muon)     fPossiblePartsPass->Fill(1);
    if (possibleParts.pion)     fPossiblePartsPass->Fill(2);
    if (possibleParts.kaon)     fPossiblePartsPass->Fill(3);
    if (possibleParts.proton)   fPossiblePartsPass->Fill(4);
  }

  const auto [momentum, tof, tofChannel,ckov0,ckov1,timingTrigger,BITrigger,areBIAndTimingMatched] = fDataUtils.GetBeamlineVarsAndStatus(evt);
  mf::LogWarning("protoana::ProtoDUNEBeamlineFilter::filter") << "momentum: " << momentum << " GeV/c, TOF: "<<tof 
                << " ns, tofChannel: " << tofChannel << " cherenkov 0: " << ckov0 << " cherenkov 1: " << ckov1
                << " timingTrigger: "<<timingTrigger<<" BITrigger: "<<BITrigger
                << " BIAndTimingMatched: "<<areBIAndTimingMatched;
  fMomentumAll->Fill(momentum);
  if(result) fMomentumPass->Fill(momentum);
  fTOFAll->Fill(momentum);
  if(result) fTOFPass->Fill(tof);
  fTOFChannelAll->Fill(tofChannel);
  if(result) fTOFChannelPass->Fill(tofChannel);

  fCKov0All->Fill(ckov0);
  if(result) fCKov0Pass->Fill(ckov0);
  fCKov1All->Fill(ckov1);
  if(result) fCKov1Pass->Fill(ckov1);

  int ckov = -1;
  if (ckov0 && ckov1 >=0)
  {
    ckov = ckov0;
    ckov += (ckov1 << 1);
  }
  fCKovAll->Fill(ckov);
  if(result) fCKovPass->Fill(ckov);

  std::vector<double> masses = fDataUtils.GetBeamlineMass(evt);
  for (const auto& mass: masses)
  {
    mf::LogWarning("protoana::ProtoDUNEBeamlineFilter::filter") << "mass: " << mass << " GeV/c^2";
   fMassAll->Fill(mass);
   if(result) fMassPass->Fill(mass);
  }
  fBITriggerAll->Fill(BITrigger);
  if(result) fBITriggerPass->Fill(BITrigger);
  fBIAndTimingMatchAll->Fill(areBIAndTimingMatched);
  if(result) fBIAndTimingMatchPass->Fill(areBIAndTimingMatched);

  return result;
}

void protoana::ProtoDUNEBeamlineFilter::endJob() {}
 
DEFINE_ART_MODULE(protoana::ProtoDUNEBeamlineFilter)
