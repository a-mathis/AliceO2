// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file femtoDreamReaderTask.cxx
/// \brief Tasks that reads the track tables used for the pairing
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#include "include/FemtoDream/FemtoDerived.h"
#include "include/FemtoDream/FemtoDreamParticleHisto.h"
#include "include/FemtoDream/FemtoDreamContainer.h"
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::analysis::femtoDream;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct femtoDreamPairTaskTrackTrack {

  /// Particle selection part

  /// Particle 1
  Configurable<int> ConfPDGCodePartOne{"ConfPDGCodePartOne", 2212, "Particle 1 - PDG code"};
  Configurable<aod::femtodreamparticle::cutContainerType> ConfTPCPartOne{"ConfTPCPartOne", 0, "Particle 1 - Selection bit"};
  Configurable<float> ConfPtMinPartOne{"ConfPtMinPartOne", 0.f, "Particle 1 - min. pT selection (GeV/c)"};
  Configurable<float> ConfPtMaxPartOne{"ConfPtMaxPartOne", 999.f, "Particle 1 - max. pT selection (GeV/c)"};
  Configurable<float> ConfEtaMaxPartOne{"ConfEtaMaxPartOne", 999.f, "Particle 1 - max. eta selection"};

  //  Configurable<uint64_t> ConfTPCTOFPart1{"ConfTPCTOFPart1", 0, "Selection bit particle 1 - TPC+TOF PID"};
  //  Configurable<float> ConfSelMomThresPart1{"ConfSelMomThresPart1", 0.f, "Selection 1 - TPC / TPC+TOF PID momentum threshold"};
  //  Configurable<uint64_t> ConfTPCPart2{"ConfTPCPart2", 0, "Selection bit particle 2 - TPC PID"};
  //  Configurable<uint64_t> ConfTPCTOFPart2{"ConfTPCTOFPart2", 0, "Selection bit particle 2 - TPC+TOF PID"};
  //  Configurable<float> ConfSelMomThresPart2{"ConfSelMomThresPart2", 0.f, "Selection 2 - TPC / TPC+TOF PID momentum threshold"};

  Partition<aod::FemtoDreamParticles> partsOne = (aod::femtodreamparticle::pt > ConfPtMinPartOne) &&
                                                 (aod::femtodreamparticle::pt < ConfPtMaxPartOne) &&
                                                 (nabs(aod::femtodreamparticle::eta) < ConfEtaMaxPartOne);
  //      (aod::femtodreamparticle::cut == ConfTPCPart1 || aod::femtodreamparticle::cut == ConfTPCTOFPart1);
  // (aod::femtodreamparticle::P <= ConfSelMomThresPart1 && aod::femtodreamparticle::cut == ConfTPCPart1) ||
  //   (aod::femtodreamparticle::P > ConfSelMomThresPart1 && aod::femtodreamparticle::cut == ConfTPCTOFPart1);
  //Partition<o2::aod::FemtoDreamParticle> partTwoPartition = (aod::femtodreamparticle::cut == ConfTPCPart2 || aod::femtodreamparticle::cut == ConfTPCTOFPart2);
  //(aod::femtodreamparticle::P <= ConfSelMomThresPart2 && aod::femtodreamparticle::cut == ConfTPCPart2) ||
  //                                                    (aod::femtodreamparticle::P > ConfSelMomThresPart2 && aod::femtodreamparticle::cut == ConfTPCTOFPart2);

  FemtoDreamParticleHisto<aod::femtodreamparticle::ParticleType::kTrack, 1> trackHistoPartOne;

  /// Particle 2
  Configurable<bool> ConfIsSame{"ConfIsSame", false, "Pairs of the same particle"};
  Configurable<int> ConfPDGCodePartTwo{"ConfPDGCodePartTwo", 2212, "Particle 2 - PDG code"};
  Configurable<aod::femtodreamparticle::cutContainerType> ConfTPCPartTwo{"ConfTPCPartTwo", 0, "Particle 2 - Selection bit"};
  Configurable<float> ConfPtMinPartTwo{"ConfPtMinPartTwo", 0.f, "Particle 2 - min. pT selection (GeV/c)"};
  Configurable<float> ConfPtMaxPartTwo{"ConfPtMaxPartTwo", 999.f, "Particle 2 - max. pT selection (GeV/c)"};
  Configurable<float> ConfEtaMaxPartTwo{"ConfEtaMaxPartTwo", 999.f, "Particle 2 - max. eta selection"};
  //
  Partition<aod::FemtoDreamParticles> partsTwo = (aod::femtodreamparticle::pt > ConfPtMinPartTwo) &&
                                                 (aod::femtodreamparticle::pt < ConfPtMaxPartTwo) &&
                                                 (nabs(aod::femtodreamparticle::eta) < ConfEtaMaxPartTwo);

  FemtoDreamParticleHisto<aod::femtodreamparticle::ParticleType::kTrack, 2> trackHistoPartTwo;

  /// Correlation part
  ConfigurableAxis CfgMultBins{"CfgMultBins", {VARIABLE_WIDTH, 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 200.0f}, "Mixing bins - multiplicity"};
  ConfigurableAxis CfgkstarBins{"CfgkstarBins", {5000, 0., 5.}, "binning kstar"};
  ConfigurableAxis CfgkTBins{"CfgkTBins", {70, 0., 7.}, "binning kT"};
  ConfigurableAxis CfgmTBins{"CfgmTBins", {70, 0., 7.}, "binning mT"};

  FemtoDreamContainer sameEventCont;

  /// Histograms
  HistogramRegistry qaRegistry{"TrackQA", {}, OutputObjHandlingPolicy::AnalysisObject};
  HistogramRegistry resultRegistry{"Correlations", {}, OutputObjHandlingPolicy::AnalysisObject};

  void init(InitContext&)
  {
    // here we need to do a couple of things
    // essentially only check whether to pair the same particle with itself or not. The rest is straight forward
    trackHistoPartOne.init(&qaRegistry);
    if (!ConfIsSame) {
      trackHistoPartTwo.init(&qaRegistry);
    }

    sameEventCont.init(&resultRegistry, femtoDreamContainer::Observable::kstar, CfgkstarBins, CfgMultBins, CfgkTBins, CfgmTBins);
    sameEventCont.setPDGCodes(ConfPDGCodePartOne, ConfPDGCodePartTwo);
  }

  void process(o2::aod::FemtoDreamCollision const& col,
               o2::aod::FemtoDreamParticles const& parts)
  {
    const int multCol = col.multV0M();

    for (auto& part : partsOne) {
      trackHistoPartOne.fillQA(part);
    }

    if (!ConfIsSame) {
      for (auto& part : partsTwo) {
        trackHistoPartTwo.fillQA(part);
      }
    }

    /// Correlation part
    for (auto& [p1, p2] : combinations(partsOne, partsTwo)) {

      // track cleaning
      if (p1.globalIndex() == p2.globalIndex())
        continue;

      // delta eta delta phi*

      sameEventCont.setPair(p1, p2, multCol);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<femtoDreamPairTaskTrackTrack>(cfgc)};
  return workflow;
}
