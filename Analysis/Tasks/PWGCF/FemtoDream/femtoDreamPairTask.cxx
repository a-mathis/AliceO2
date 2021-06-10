// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file femtoDreamSameEventTask.cxx
/// \brief Analysis task for particle pairing in the same event
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#include "include/FemtoDream/FemtoDerived.h"
#include "include/FemtoDream/FemtoDreamContainer.h"

#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/ASoAHelpers.h"

using namespace o2;
using namespace o2::analysis::femtoDream;
using namespace o2::framework;

struct femtoDreamPairTask {

  Configurable<int> CfgPDGCodePartOne{"CfgPDGCodePartOne", 2212, "PDG Code of particle one"};
  Configurable<int> CfgPDGCodePartTwo{"CfgPDGCodePartTwo", 2212, "PDG Code of particle two"};
  ConfigurableAxis CfgMultBins{"CfgMultBins", {VARIABLE_WIDTH, 0.0f, 20.0f, 40.0f, 60.0f, 80.0f, 100.0f, 200.0f}, "Mixing bins - multiplicity"};
  ConfigurableAxis CfgkstarBins{"CfgkstarBins", {5000, 0., 5.}, "binning kstar"};
  ConfigurableAxis CfgkTBins{"CfgkTBins", {70, 0., 7.}, "binning kT"};
  ConfigurableAxis CfgmTBins{"CfgmTBins", {70, 0., 7.}, "binning mT"};

  /// Histograms
  FemtoDreamContainer sameEventCont;
  HistogramRegistry resultRegistry{"Correlations", {}, OutputObjHandlingPolicy::AnalysisObject};

  void init(InitContext&)
  {
    sameEventCont.init(&resultRegistry, femtoDreamContainer::Observable::kstar, CfgkstarBins, CfgMultBins, CfgkTBins, CfgmTBins);
    sameEventCont.setPDGCodes(CfgPDGCodePartOne, CfgPDGCodePartTwo);
  }

  void process(aod::FemtoDreamCollision const& col, aod::FemtoDreamParticles const& parts)
  {
    int multCol = col.multV0M();

    for (auto& [p1, p2] : combinations(parts, parts)) {
      sameEventCont.setPair(p1, p2, multCol);
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<femtoDreamPairTask>(cfgc)};
  return workflow;
}
