// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FemtoDreamParticleHisto.h
/// \brief FemtoDreamParticleHisto - Histogram class for tracks, vos and cascades
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_O2FEMTODREAM_INCLUDE_O2FEMTODREAM_FEMTODREAMPARTICLEHISTO_H_
#define ANALYSIS_TASKS_PWGCF_O2FEMTODREAM_INCLUDE_O2FEMTODREAM_FEMTODREAMPARTICLEHISTO_H_

#include "Framework/HistogramRegistry.h"
#include "FemtoDerived.h"
#include <Rtypes.h>

#include <iostream>

using namespace o2::framework;

namespace o2::analysis::femtoDream
{

template <o2::aod::femtodreamparticle::ParticleType particleType>
class FemtoDreamParticleHisto
{
 public:
  virtual ~FemtoDreamParticleHisto() = default;

  void init(HistogramRegistry* registry)
  {
    if (registry) {
      mHistogramRegistry = registry;
      /// \todo how to do the naming for track - track combinations?
      std::string folderName = static_cast<std::string>(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]);
      mHistogramRegistry->add((folderName + "/pThist").c_str(), "; #it{p}_{T} (GeV/#it{c}); Entries", kTH1F, {{1000, 0, 10}});
      mHistogramRegistry->add((folderName + "/etahist").c_str(), "; #eta; Entries", kTH1F, {{1000, -1, 1}});
      mHistogramRegistry->add((folderName + "/phihist").c_str(), "; #phi; Entries", kTH1F, {{1000, 0, 2. * M_PI}});
      if constexpr (mParticleType == o2::aod::femtodreamparticle::ParticleType::kTrack) {
        mHistogramRegistry->add((folderName + "/dcaXYhist").c_str(), "; #it{p}_{T} (GeV/#it{c}); DCA_{xy} (cm)", kTH2F, {{100, 0, 10}, {501, -3, 3}});
      } else if constexpr (mParticleType == o2::aod::femtodreamparticle::ParticleType::kV0) {
        mHistogramRegistry->add((folderName + "/cpahist").c_str(), "; #it{p}_{T} (GeV/#it{c}); cos#alpha", kTH2F, {{100, 0, 10}, {500, 0, 1}});
      }
    }
  }

  template <typename T>
  void fillQA(T const& part)
  {
    if (mHistogramRegistry) {
      mHistogramRegistry->fill(HIST(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]) + HIST("/pThist"), part.pt());
      mHistogramRegistry->fill(HIST(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]) + HIST("/etahist"), part.eta());
      mHistogramRegistry->fill(HIST(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]) + HIST("/phihist"), part.phi());
      if constexpr (mParticleType == o2::aod::femtodreamparticle::ParticleType::kTrack) {
        mHistogramRegistry->fill(HIST(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]) + HIST("/dcaXYhist"), part.pt(), part.tempFitVar());
      } else if constexpr (mParticleType == o2::aod::femtodreamparticle::ParticleType::kV0) {
        mHistogramRegistry->fill(HIST(o2::aod::femtodreamparticle::ParticleTypeName[mParticleType]) + HIST("/cpahist"), part.pt(), part.tempFitVar());
      }
    }
  }

 private:
  HistogramRegistry* mHistogramRegistry;                                                   ///< For QA output
  static constexpr o2::aod::femtodreamparticle::ParticleType mParticleType = particleType; ///< Type of the particle under analysis
};
} // namespace o2::analysis::femtoDream

#endif /* ANALYSIS_TASKS_PWGCF_O2FEMTODREAM_INCLUDE_O2FEMTODREAM_FEMTODREAMPARTICLEHISTO_H_ */
