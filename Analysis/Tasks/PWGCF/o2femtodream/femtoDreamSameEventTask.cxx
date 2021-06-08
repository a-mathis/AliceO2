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

#include "include/o2femtodream/FemtoDerived.h"
#include "include/o2femtodream/FemtoDreamContainer.h"

#include "Framework/AnalysisDataModel.h"
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "Framework/HistogramRegistry.h"
#include "Framework/ASoAHelpers.h"

#include "TDatabasePDG.h"

using namespace o2;
using namespace o2::analysis::femtoDream;
using namespace o2::framework;

struct femtoDreamSameEventTask {

  //bool isSame = false;

  O2_DEFINE_CONFIGURABLE(CfgPDGCodePartOne, int, 2212, "PDG Code of particle one");
  O2_DEFINE_CONFIGURABLE(CfgPDGCodePartTwo, int, 2212, "PDG Code of particle two");
  //O2_DEFINE_CONFIGURABLE(CfgCutParticle1, unsigned long int, 2212, "Cut particle 1");
  //O2_DEFINE_CONFIGURABLE(CfgCutParticle2, unsigned long int, 2212, "Cut particle 2");

  //Partition<aod::FemtoDreamParticles> particles1 = o2::aod::femtodreamparticle::cut == CfgCutParticle1;
  //Partition<aod::FemtoDreamParticles> particles2 = o2::aod::femtodreamparticle::cut == CfgCutParticle2;

  /// Histograms
  //FemtoDreamContainer sameEventCont;
  //HistogramRegistry resultRegistry{"Correlations", {}, OutputObjHandlingPolicy::AnalysisObject};

  void init(InitContext&)
  {
    //if (CfgCutParticle1 == CfgCutParticle2) {
    //  isSame = true;
    //} else {
    // be sure that you don't pair two objects with themselves
    // check the row of the table? (works for track-track / v0-v0 / c-c

    // how to handle shared children - we need some ID of the tracks assigned to an object (encoded in row)
    //}
    // check CfgCutParticle1 and 2 for their type (T, V, C)
    // configure the PairCleaner accordingly

    //sameEventCont.init(&resultRegistry);
    //sameEventCont.setMasses(TDatabasePDG::Instance()->GetParticle(CfgPDGCodePartOne)->Mass(),
    //                        TDatabasePDG::Instance()->GetParticle(CfgPDGCodePartTwo)->Mass());
  }

  void process(aod::FemtoDreamCollision const& col, aod::FemtoDreamParticles const& parts)
  {

    // template variables to be stored

    //for (auto& p1 : particles1) {
    // for (auto& p2 : particles2) {
    // do some PairCleaning
    // if (p1 == p2) continue;
    //    sameEventCont.setPair(p1, p2);
    //  }
    //}

    //for (auto& [p1, p2] : combinations(parts, parts)) {
    // sameEventCont.setPair(p1, p2);
    //}
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<femtoDreamSameEventTask>(cfgc)};
  return workflow;
}
