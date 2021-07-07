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
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::analysis::femtoDream;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct femtoDreamTrackTrackPairTask {

  Configurable<int32_t> ConfTPCPart1{"ConfTPCPart1", 0, "Selection bit particle 1 - TPC PID"};
  //  Configurable<uint64_t> ConfTPCTOFPart1{"ConfTPCTOFPart1", 0, "Selection bit particle 1 - TPC+TOF PID"};
  //  Configurable<float> ConfSelMomThresPart1{"ConfSelMomThresPart1", 0.f, "Selection 1 - TPC / TPC+TOF PID momentum threshold"};
  //  Configurable<uint64_t> ConfTPCPart2{"ConfTPCPart2", 0, "Selection bit particle 2 - TPC PID"};
  //  Configurable<uint64_t> ConfTPCTOFPart2{"ConfTPCTOFPart2", 0, "Selection bit particle 2 - TPC+TOF PID"};
  //  Configurable<float> ConfSelMomThresPart2{"ConfSelMomThresPart2", 0.f, "Selection 2 - TPC / TPC+TOF PID momentum threshold"};

  FemtoDreamParticleHisto<aod::femtodreamparticle::ParticleType::kTrack> trackHistoPartOne;
  FemtoDreamParticleHisto<aod::femtodreamparticle::ParticleType::kTrack> trackHistoPartTwo;

  //Partition<o2::aod::FemtoDreamParticle> partOnePartition = (aod::femtodreamparticle::cut == ConfTPCPart1 || aod::femtodreamparticle::cut == ConfTPCTOFPart1);
  // (aod::femtodreamparticle::P <= ConfSelMomThresPart1 && aod::femtodreamparticle::cut == ConfTPCPart1) ||
  //   (aod::femtodreamparticle::P > ConfSelMomThresPart1 && aod::femtodreamparticle::cut == ConfTPCTOFPart1);
  //Partition<o2::aod::FemtoDreamParticle> partTwoPartition = (aod::femtodreamparticle::cut == ConfTPCPart2 || aod::femtodreamparticle::cut == ConfTPCTOFPart2);
  //(aod::femtodreamparticle::P <= ConfSelMomThresPart2 && aod::femtodreamparticle::cut == ConfTPCPart2) ||
  //                                                    (aod::femtodreamparticle::P > ConfSelMomThresPart2 && aod::femtodreamparticle::cut == ConfTPCTOFPart2);

  HistogramRegistry qaRegistry{"QAHistos", {}, OutputObjHandlingPolicy::QAObject};

  void init(InitContext&)
  {
    // here we need to do a couple of things
    // essentially only check whether to pair the same particle with itself or not. The rest is straight forward
    trackHistoPartOne.init(&qaRegistry);
    //trackHistoPartTwo.init(&qaRegistry);
  }

  void process(o2::aod::FemtoDreamCollision const& col,
               o2::aod::FemtoDreamParticles const& parts)
  {
    for (auto& part : parts) {
      trackHistoPartOne.fillQA(part);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<femtoDreamTrackTrackPairTask>(cfgc)};
  return workflow;
}
