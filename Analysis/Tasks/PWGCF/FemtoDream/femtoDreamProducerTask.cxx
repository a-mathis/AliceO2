// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file femtoDreamProducerTask.cxx
/// \brief Tasks that produces the track tables used for the pairing
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#include "include/FemtoDream/FemtoDreamCollisionSelection.h"
#include "include/FemtoDream/FemtoDreamTrackSelection.h"
#include "include/FemtoDream/FemtoDreamV0Selection.h"
#include "include/FemtoDream/FemtoDerived.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/ASoAHelpers.h"
#include "AnalysisDataModel/TrackSelectionTables.h"
#include "AnalysisDataModel/PID/PIDResponse.h"
#include "AnalysisDataModel/EventSelection.h"
#include "AnalysisDataModel/Multiplicity.h"
#include "ReconstructionDataFormats/Track.h"
#include "AnalysisCore/trackUtilities.h"
#include "AnalysisCore/RecoDecay.h"
#include "AnalysisDataModel/StrangenessTables.h"

using namespace o2;
using namespace o2::analysis::femtoDream;
using namespace o2::framework;
using namespace o2::framework::expressions;

namespace o2::aod
{
using FilteredFullCollision = soa::Filtered<soa::Join<aod::Collisions,
                                                      aod::EvSels,
                                                      aod::Mults>>::iterator;
using FilteredFullTracks = soa::Filtered<soa::Join<aod::FullTracks,
                                                   aod::TracksExtended,
                                                   aod::pidTPCEl, aod::pidTPCMu, aod::pidTPCPi,
                                                   aod::pidTPCKa, aod::pidTPCPr, aod::pidTPCDe,
                                                   aod::pidTOFEl, aod::pidTOFMu, aod::pidTOFPi,
                                                   aod::pidTOFKa, aod::pidTOFPr, aod::pidTOFDe>>;
// using FilteredFullV0s = soa::Filtered<aod::V0Datas>; /// predefined Join table for o2::aod::V0s = soa::Join<o2::aod::TransientV0s, o2::aod::StoredV0s> to be used when we add v0Filter
} // namespace o2::aod

struct femtoDreamProducerTask {

  Produces<aod::FemtoDreamCollisions> outputCollision;
  Produces<aod::FemtoDreamParticles> outputTracks;
  Produces<aod::FemtoDreamDebugParticles> outputDebugTracks;

  Configurable<bool> ConfDebugOutput{"ConfDebugOutput", true, "Debug output"};

  /// Event cuts
  FemtoDreamCollisionSelection colCuts;
  Configurable<float> ConfEvtZvtx{"ConfEvtZvtx", 10.f, "Evt sel: Max. z-Vertex (cm)"};
  Configurable<bool> ConfEvtTriggerCheck{"ConfEvtTriggerCheck", false, "Evt sel: check for trigger"};
  Configurable<int> ConfEvtTriggerSel{"ConfEvtTriggerSel", kINT7, "Evt sel: trigger"};
  Configurable<bool> ConfEvtOfflineCheck{"ConfEvtOfflineCheck", false, "Evt sel: check for offline selection"};

  Filter colFilter = nabs(aod::collision::posZ) < ConfEvtZvtx;

  FemtoDreamTrackSelection trackCuts;
  Configurable<std::vector<float>> ConfTrkCharge{"ConfTrkCharge", std::vector<float>{-1, 1}, "Trk sel: Charge"};
  Configurable<std::vector<float>> ConfTrkPtmin{"ConfTrkPtmin", std::vector<float>{0.4f, 0.6f, 0.5f}, "Trk sel: Min. pT (GeV/c)"};
  Configurable<std::vector<float>> ConfTrkPtmax{"ConfTrkPtmax", std::vector<float>{4.05f, 999.f}, "Trk sel: Max. pT (GeV/c)"};
  Configurable<std::vector<float>> ConfTrkEta{"ConfTrkEta", std::vector<float>{0.8f, 0.7f, 0.9f}, "Trk sel: Max. eta"};
  Configurable<std::vector<float>> ConfTrkTPCnclsMin{"ConfTrkTPCnclsMin", std::vector<float>{80.f, 70.f, 60.f}, "Trk sel: Min. nCls TPC"};
  Configurable<std::vector<float>> ConfTrkTPCfCls{"ConfTrkTPCfCls", std::vector<float>{0.7f, 0.83f, 0.9f}, "Trk sel: Min. ratio crossed rows/findable"};
  Configurable<std::vector<float>> ConfTrkTPCsCls{"ConfTrkTPCsCls", std::vector<float>{0.1f, 160.f}, "Trk sel: Max. TPC shared cluster"};
  Configurable<std::vector<float>> ConfTrkDCAxyMax{"ConfTrkDCAxyMax", std::vector<float>{0.1f, 3.5f}, "Trk sel: Max. DCA_xy (cm)"}; /// here we need an open cut to do the DCA fits later on!
  Configurable<std::vector<float>> ConfTrkDCAzMax{"ConfTrkDCAzMax", std::vector<float>{0.2f, 3.5f}, "Trk sel: Max. DCA_z (cm)"};
  /// \todo maybe we need to remove the PID from the general cut container and have a separate one, these are lots and lots of bits we need
  Configurable<std::vector<float>> ConfTrkPIDnSigmaMax{"ConfTrkPIDnSigmaMax", std::vector<float>{3.5f, 3.f, 2.5f}, "Trk sel: Max. PID nSigma"};
  Configurable<std::vector<int>> ConfTrkTPIDspecies{"ConfTrkTPIDspecies", std::vector<int>{o2::track::PID::Electron, o2::track::PID::Pion, o2::track::PID::Kaon, o2::track::PID::Proton, o2::track::PID::Deuteron}, "Trk sel: Particles species for PID"};

  // for now this selection does not work yet, however will very soon
  // \todo once this is the case, set the limits less strict!
  MutableConfigurable<float> TrackMinSelPtMin{"TrackMinSelPtMin", 0.4f, "(automatic) Minimal pT selection for tracks"};
  MutableConfigurable<float> TrackMinSelPtMax{"TrackMinSelPtMax", 10.f, "(automatic) Maximal pT selection for tracks"};
  MutableConfigurable<float> TrackMinSelEtaMax{"TrackMinSelEtaMax", 1.f, "(automatic) Maximal eta selection for tracks"};

  Filter trackFilter = (aod::track::pt > TrackMinSelPtMin.value) &&
                       (aod::track::pt < TrackMinSelPtMax.value) &&
                       (nabs(aod::track::eta) < TrackMinSelEtaMax.value);

  FemtoDreamV0Selection v0Cuts;
  FemtoDreamTrackSelection PosDaughTrack;
  FemtoDreamTrackSelection NegDaughTrack;
  Configurable<std::vector<float>> ConfV0PtMin{"ConfV0PtMin", std::vector<float>{0.24f, 0.3f, 0.36f}, "V0 sel: Min. pT (GeV/c)"};
  Configurable<std::vector<float>> ConfV0PtMax{"ConfV0PtMax", std::vector<float>{4.05f, 999.f}, "V0 sel: Max. pT (GeV/c)"};
  Configurable<float> ConfV0DecVtxMax{"ConfV0DecVtxMax", 100.f, "V0 sel: Max. distance from Vtx (cm)"};
  Configurable<float> ConfTranRadV0Min{"ConfTranRadV0Min", 0.2f, "V0 sel: Min. transverse radius (cm)"};
  Configurable<float> ConfTranRadV0Max{"ConfTranRadV0Max", 100.f, "V0 sel: Max. transverse radius (cm)"};
  Configurable<std::vector<float>> ConfDCAV0DaughMax{"ConfDCAV0DaughMax", std::vector<float>{1.2f, 1.5f}, "V0 sel: Max. DCA daugh from SV (cm)"};
  Configurable<std::vector<float>> ConfCPAV0Min{"ConfCPAV0Min", std::vector<float>{0.9f, 0.995f}, "V0 sel: Min. CPA"};
  Configurable<float> ConfV0InvMassMax{"ConfV0InvMassMax", 0.004f, "V0 sel: Max. IM cut for v0s"};
  Configurable<float> ConfV0InvMassRejMax{"ConfV0InvMassRejMax", 0.035f, "V0 sel: Max. IM cut for rejection"};
  ///To do: add configurables for InvMass and Daughters when possible
  // Configurable<std::vector<float>> ConfV0DaughDCAMax{"ConfV0DaughDCAMax", std::vector<float>{0.05f, 0.06f}, "V0 sel:  Max. DCA Daugh to PV (cm)"};
  // Configurable<std::vector<float>> ConfV0DaughPIDnSigmaMax{"ConfTrkPIDnSigmaMax", std::vector<float>{5.f, 4.f}, "Daugh Trk sel: Max. PID nSigma TPC"};

  // MutableConfigurable<float> V0MinSelPtMin{"V0MinSelPtMin", 0.3f, "(automatic) Minimal pT selection for v0s"};
  // MutableConfigurable<float> V0DaughMinSelEtaMax{"V0DaughMinSelEtaMax", 0.8f, "(automatic) Maximal eta selection for daughter tracks of v0s"};

  ///to do: add cuts on fiducial volume and trans radius
  // Filter v0Filter = (aod::V0Datas::pt > V0MinSelPtMin.value) &&
  //                   (nabs(aod::track::eta) < V0DaughMinSelEtaMax.value);

  HistogramRegistry qaRegistry{"QAHistos", {}, OutputObjHandlingPolicy::QAObject};

  void init(InitContext&)
  {
    colCuts.setCuts(ConfEvtZvtx, ConfEvtTriggerCheck, ConfEvtTriggerSel, ConfEvtOfflineCheck);
    colCuts.init(&qaRegistry);

    trackCuts.setSelection(ConfTrkCharge, femtoDreamTrackSelection::kSign, femtoDreamSelection::kEqual);
    trackCuts.setSelection(ConfTrkPtmin, femtoDreamTrackSelection::kpTMin, femtoDreamSelection::kLowerLimit);
    trackCuts.setSelection(ConfTrkPtmax, femtoDreamTrackSelection::kpTMax, femtoDreamSelection::kUpperLimit);
    trackCuts.setSelection(ConfTrkEta, femtoDreamTrackSelection::kEtaMax, femtoDreamSelection::kAbsUpperLimit);
    trackCuts.setSelection(ConfTrkTPCnclsMin, femtoDreamTrackSelection::kTPCnClsMin, femtoDreamSelection::kLowerLimit);
    trackCuts.setSelection(ConfTrkTPCfCls, femtoDreamTrackSelection::kTPCfClsMin, femtoDreamSelection::kLowerLimit);
    trackCuts.setSelection(ConfTrkTPCsCls, femtoDreamTrackSelection::kTPCsClsMax, femtoDreamSelection::kUpperLimit);
    trackCuts.setSelection(ConfTrkDCAxyMax, femtoDreamTrackSelection::kDCAxyMax, femtoDreamSelection::kAbsUpperLimit);
    trackCuts.setSelection(ConfTrkDCAzMax, femtoDreamTrackSelection::kDCAzMax, femtoDreamSelection::kAbsUpperLimit);
    trackCuts.setSelection(ConfTrkPIDnSigmaMax, femtoDreamTrackSelection::kPIDnSigmaMax, femtoDreamSelection::kAbsUpperLimit);
    trackCuts.setPIDSpecies(ConfTrkTPIDspecies);
    trackCuts.init(&qaRegistry);

    if (trackCuts.getNSelections(femtoDreamTrackSelection::kpTMin) > 0) {
      TrackMinSelPtMin.value = trackCuts.getMinimalSelection(femtoDreamTrackSelection::kpTMin, femtoDreamSelection::kLowerLimit);
    }
    if (trackCuts.getNSelections(femtoDreamTrackSelection::kpTMax) > 0) {
      TrackMinSelPtMax.value = trackCuts.getMinimalSelection(femtoDreamTrackSelection::kpTMax, femtoDreamSelection::kUpperLimit);
    }
    if (trackCuts.getNSelections(femtoDreamTrackSelection::kEtaMax) > 0) {
      TrackMinSelEtaMax.value = trackCuts.getMinimalSelection(femtoDreamTrackSelection::kEtaMax, femtoDreamSelection::kAbsUpperLimit);
    }

    v0Cuts.setTypev0(femtoDreamV0Selection::V0Type::kLambda);
    v0Cuts.setSelection(ConfV0PtMin, femtoDreamV0Selection::kpTV0Min, femtoDreamSelection::kLowerLimit);
    v0Cuts.setSelection(ConfV0PtMax, femtoDreamV0Selection::kpTV0Max, femtoDreamSelection::kUpperLimit);
    // v0Cuts.setChildCuts(femtoDreamV0Selection::kPosDaughTrack, ConfTrkCharge, femtoDreamTrackSelection::kSign, femtoDreamSelection::kEqual);
    // v0Cuts.setChildCuts(femtoDreamV0Selection::kPosDaughTrack, ConfTrkEta, femtoDreamTrackSelection::kEtaMax, femtoDreamSelection::kAbsUpperLimit);
    // v0Cuts.setChildCuts(femtoDreamV0Selection::kNegDaughTrack, ConfTrkCharge, femtoDreamTrackSelection::kSign, femtoDreamSelection::kEqual);
    // v0Cuts.setChildCuts(femtoDreamV0Selection::kNegDaughTrack, ConfTrkEta, femtoDreamTrackSelection::kEtaMax, femtoDreamSelection::kAbsUpperLimit);
    v0Cuts.init(&qaRegistry);
  }

  void process(aod::FilteredFullCollision const& col,
               aod::FilteredFullTracks const& tracks,
               o2::aod::V0Datas const& fullV0s) /// to check: not working when putting FilteredFullV0s
  {
    if (!colCuts.isSelected(col)) {
      return;
    }
    const auto vtxZ = col.posZ();
    const auto mult = col.multV0M();
    const auto spher = colCuts.computeSphericity(col, tracks);
    colCuts.fillQA(col);
    outputCollision(vtxZ, mult, spher);

    int childIDs[2] = {0,0};
    for (auto& v0 : fullV0s) {
      if (!v0Cuts.isSelectedMinimal(col, v0)) {
        continue;
      }
      v0Cuts.fillQA(col, v0);
      ///to do: how to fill some QA for daughter tracks!
    }

    for (auto& track : tracks) {
      if (!trackCuts.isSelectedMinimal(track)) {
        continue;
      }
      trackCuts.fillQA(track);
      auto cutContainer = trackCuts.getCutContainer(track);
      if (cutContainer > 0) {
        trackCuts.fillCutQA(track, cutContainer);
        outputTracks(outputCollision.lastIndex(), track.pt(), track.eta(), track.phi(), aod::femtodreamparticle::ParticleType::kTrack, cutContainer, track.dcaXY(), childIDs);
        if (ConfDebugOutput) {
          outputDebugTracks(outputCollision.lastIndex(),
                            track.sign(), track.tpcNClsFound(),
                            track.tpcNClsFindable(),
                            track.tpcNClsCrossedRows(), track.tpcNClsShared(), track.dcaXY(), track.dcaZ(),
                            track.tpcNSigmaEl(), track.tpcNSigmaPi(), track.tpcNSigmaKa(), track.tpcNSigmaPr(), track.tpcNSigmaDe(),
                            track.tofNSigmaEl(), track.tofNSigmaPi(), track.tofNSigmaKa(), track.tofNSigmaPr(), track.tofNSigmaDe());
        }
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<femtoDreamProducerTask>(cfgc)};
  return workflow;
}
