// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FemtoDreamTrackCuts.h
/// \brief Definition of the FemtoDreamTrackCuts
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMTRACKSELECTION_H_
#define ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMTRACKSELECTION_H_

#include "FemtoDerived.h"
#include "FemtoDreamObjectSelection.h"

#include "ReconstructionDataFormats/PID.h"
#include "Framework/HistogramRegistry.h"
#include <Rtypes.h>
#include <cmath>

using namespace o2::framework;

namespace o2::analysis
{
namespace femtoDream
{
namespace femtoDreamTrackSelection
{
enum TrackSel { kSign,
                kpTMin,
                kpTMax,
                kEtaMax,
                kTPCnClsMin,
                kTPCfClsMin,
                kTPCcRowsMin,
                kTPCsClsMax,
                kDCAxyMax,
                kDCAzMax,
                kDCAMin,
                kPIDnSigmaMax };
}

/// \class FemtoDreamTrackCuts
/// \brief Cut class to contain and execute all cuts applied to tracks
class FemtoDreamTrackSelection : public FemtoDreamObjectSelection<float, femtoDreamTrackSelection::TrackSel>
{
 public:
  /// Initializes histograms for the task
  template <o2::aod::femtodreamparticle::ParticleType part, typename T>
  void init(HistogramRegistry* registry);

  template <typename T>
  void setPIDSpecies(T& pids)
  {
    std::vector<int> tmpPids = pids; // necessary due to some features of the configurable
    for (const o2::track::PID& pid : tmpPids) {
      mPIDspecies.push_back(pid);
    }
  }

  template <typename T>
  auto getNsigmaTPC(T const& track, o2::track::PID pid);

  template <typename T>
  auto getNsigmaTOF(T const& track, o2::track::PID pid);

  template <typename T>
  bool isSelectedMinimal(T const& track);

  template <typename T1, typename T2>
  T1 getCutContainer(T2 const& track);

  template <typename T>
  void fillQA(T const& track);

  static std::string getSelectionName(femtoDreamTrackSelection::TrackSel iSel, std::string_view prefix = "", std::string_view appendix = "")
  {
    std::string outString = static_cast<std::string>(prefix);
    outString += static_cast<std::string>(mSelectionNames[iSel]);
    outString += appendix;
    return outString;
  }

  static std::string getSelectionHelper(femtoDreamTrackSelection::TrackSel iSel, std::string_view prefix = "")
  {
    std::string outString = static_cast<std::string>(prefix);
    outString += static_cast<std::string>(mSelectionHelper[iSel]);
    return outString;
  }

 private:
  std::vector<o2::track::PID> mPIDspecies;
  static constexpr std::string_view mSelectionNames[11] = {"Sign", "PtMin", "PtMax", "EtaMax", "TPCnClsMin", "TPCfClsMin", "TPCcRowsMin", "TPCsClsMax", "DCAxyMax", "DCAzMax", "PIDnSigmaMax"};
  static constexpr std::string_view mSelectionHelper[11] = {"Sign of the track", "Minimal pT (GeV/c)", "Maximal pT (GeV/c)", "Maximal eta", "Minimum number of TPC clusters", "Minimum fraction of findable clusters", "Minimum number of crossed TPC rows", "Maximal number of shared TPC cluster", "Maximal DCA_xy (cm)", "Maximal DCA_z (cm)", "Maximal PID (nSigma)"};

  ClassDefNV(FemtoDreamTrackSelection, 1);
}; // namespace femtoDream

template <o2::aod::femtodreamparticle::ParticleType part, typename T>
void FemtoDreamTrackSelection::init(HistogramRegistry* registry)
{
  if (registry) {
    mHistogramRegistry = registry;
    fillSelectionHistogram<part>();

    /// \todo this should be an automatic check in the parent class, and the return type should be templated
    int nSelections = getNSelections() + mPIDspecies.size() * (getNSelections(femtoDreamTrackSelection::kPIDnSigmaMax) - 1);
    if (8 * sizeof(T) < nSelections) {
      LOG(FATAL) << "FemtoDreamTrackCuts: Number of selections to large for your container - quitting!";
    }

    mHistogramRegistry->add("TrackCuts/pThist", "; #it{p}_{T} (GeV/#it{c}); Entries", kTH1F, {{1000, 0, 10}});
    mHistogramRegistry->add("TrackCuts/etahist", "; #eta; Entries", kTH1F, {{1000, -1, 1}});
    mHistogramRegistry->add("TrackCuts/phihist", "; #phi; Entries", kTH1F, {{1000, 0, 2. * M_PI}});
    mHistogramRegistry->add("TrackCuts/tpcnclshist", "; TPC Cluster; Entries", kTH1F, {{163, 0, 163}});
    mHistogramRegistry->add("TrackCuts/tpcfclshist", "; TPC ratio findable; Entries", kTH1F, {{100, 0.5, 1.5}});
    mHistogramRegistry->add("TrackCuts/tpcnrowshist", "; TPC crossed rows; Entries", kTH1F, {{163, 0, 163}});
    mHistogramRegistry->add("TrackCuts/tpcnsharedhist", "; TPC shared clusters; Entries", kTH1F, {{163, 0, 163}});
    mHistogramRegistry->add("TrackCuts/dcaXYhist", "; #it{p}_{T} (GeV/#it{c}); DCA_{xy} (cm)", kTH2F, {{100, 0, 10}, {301, -1.5, 1.5}});
    mHistogramRegistry->add("TrackCuts/dcaZhist", "; #it{p}_{T} (GeV/#it{c}); DCA_{z} (cm)", kTH2F, {{100, 0, 10}, {301, -1.5, 1.5}});
    mHistogramRegistry->add("TrackCuts/dcahist", "; #it{p}_{T} (GeV/#it{c}); DCA (cm)", kTH1F, {{301, 0., 1.5}});
    mHistogramRegistry->add("TrackCuts/tpcdEdx", "; #it{p} (GeV/#it{c}); TPC Signal", kTH2F, {{100, 0, 10}, {1000, 0, 1000}});
    mHistogramRegistry->add("TrackCuts/tofSignal", "; #it{p} (GeV/#it{c}); TOF Signal", kTH2F, {{100, 0, 10}, {1000, 0, 100e3}});

    const int nChargeSel = getNSelections(femtoDreamTrackSelection::kSign);
    const int nPtMinSel = getNSelections(femtoDreamTrackSelection::kpTMin);
    const int nPtMaxSel = getNSelections(femtoDreamTrackSelection::kpTMax);
    const int nEtaMaxSel = getNSelections(femtoDreamTrackSelection::kEtaMax);
    const int nTPCnMinSel = getNSelections(femtoDreamTrackSelection::kTPCnClsMin);
    const int nTPCfMinSel = getNSelections(femtoDreamTrackSelection::kTPCfClsMin);
    const int nTPCcMinSel = getNSelections(femtoDreamTrackSelection::kTPCcRowsMin);
    const int nTPCsMaxSel = getNSelections(femtoDreamTrackSelection::kTPCsClsMax);
    const int nDCAxyMaxSel = getNSelections(femtoDreamTrackSelection::kDCAxyMax);
    const int nDCAzMaxSel = getNSelections(femtoDreamTrackSelection::kDCAzMax);
    const int nDCAMinSel = getNSelections(femtoDreamTrackSelection::kDCAMin);
    const int nPIDnSigmaSel = getNSelections(femtoDreamTrackSelection::kPIDnSigmaMax);
  }
}

template <typename T>
auto FemtoDreamTrackSelection::getNsigmaTPC(T const& track, o2::track::PID pid)
{
  switch (pid) {
    case o2::track::PID::Electron:
      return track.tpcNSigmaEl();
      break;
    case o2::track::PID::Muon:
      return track.tpcNSigmaMu();
      break;
    case o2::track::PID::Pion:
      return track.tpcNSigmaPi();
      break;
    case o2::track::PID::Kaon:
      return track.tpcNSigmaKa();
      break;
    case o2::track::PID::Proton:
      return track.tpcNSigmaPr();
      break;
    case o2::track::PID::Deuteron:
      return track.tpcNSigmaDe();
      break;
    default:
      return 999.f;
      break;
  }
}

template <typename T>
auto FemtoDreamTrackSelection::getNsigmaTOF(T const& track, o2::track::PID pid)
{
  /// skip tracks without TOF signal
  /// \todo not sure what the error flags mean...
  if (track.tofSignal() <= 0.f || std::abs(track.tofSignal() - 99998) < 0.01 || std::abs(track.tofSignal() - 99999) < 0.01) {
    return 999.f;
  }

  switch (pid) {
    case o2::track::PID::Electron:
      return track.tofNSigmaEl();
      break;
    case o2::track::PID::Muon:
      return track.tofNSigmaMu();
      break;
    case o2::track::PID::Pion:
      return track.tofNSigmaPi();
      break;
    case o2::track::PID::Kaon:
      return track.tofNSigmaKa();
      break;
    case o2::track::PID::Proton:
      return track.tofNSigmaPr();
      break;
    case o2::track::PID::Deuteron:
      return track.tofNSigmaDe();
      break;
    default:
      return 999.f;
      break;
  }
}

template <typename T>
bool FemtoDreamTrackSelection::isSelectedMinimal(T const& track)
{
  const auto pT = track.pt();
  const auto eta = track.eta();
  const auto tpcNClsF = track.tpcNClsFound();
  const auto tpcRClsC = track.tpcCrossedRowsOverFindableCls();
  const auto tpcNClsC = track.tpcNClsCrossedRows();
  const auto tpcNClsS = track.tpcNClsShared();
  const auto dcaXY = track.dcaXY();
  const auto dcaZ = track.dcaZ();
  const auto dca = std::sqrt(pow(dcaXY, 2.) + pow(dcaZ, 2.));
  /// check whether the most open cuts are fulfilled - most of this should have already be done by the filters

  const static int nPtMinSel = getNSelections(femtoDreamTrackSelection::kpTMin);
  const static int nPtMaxSel = getNSelections(femtoDreamTrackSelection::kpTMax);
  const static int nEtaSel = getNSelections(femtoDreamTrackSelection::kEtaMax);
  const static int nTPCnMinSel = getNSelections(femtoDreamTrackSelection::kTPCnClsMin);
  const static int nTPCfMinSel = getNSelections(femtoDreamTrackSelection::kTPCfClsMin);
  const static int nTPCcMinSel = getNSelections(femtoDreamTrackSelection::kTPCcRowsMin);
  const static int nTPCsMaxSel = getNSelections(femtoDreamTrackSelection::kTPCsClsMax);
  const static int nDCAxyMaxSel = getNSelections(femtoDreamTrackSelection::kDCAxyMax);
  const static int nDCAzMaxSel = getNSelections(femtoDreamTrackSelection::kDCAzMax);
  const static int nDCAMinSel = getNSelections(femtoDreamTrackSelection::kDCAMin);

  const static float pTMin = getMinimalSelection(femtoDreamTrackSelection::kpTMin, femtoDreamSelection::kLowerLimit);
  const static float pTMax = getMinimalSelection(femtoDreamTrackSelection::kpTMax, femtoDreamSelection::kUpperLimit);
  const static float etaMax = getMinimalSelection(femtoDreamTrackSelection::kEtaMax, femtoDreamSelection::kAbsUpperLimit);
  const static float nClsMin = getMinimalSelection(femtoDreamTrackSelection::kTPCnClsMin, femtoDreamSelection::kLowerLimit);
  const static float fClsMin = getMinimalSelection(femtoDreamTrackSelection::kTPCfClsMin, femtoDreamSelection::kLowerLimit);
  const static float cTPCMin = getMinimalSelection(femtoDreamTrackSelection::kTPCcRowsMin, femtoDreamSelection::kLowerLimit);
  const static float sTPCMax = getMinimalSelection(femtoDreamTrackSelection::kTPCsClsMax, femtoDreamSelection::kUpperLimit);
  const static float dcaXYMax = getMinimalSelection(femtoDreamTrackSelection::kDCAxyMax, femtoDreamSelection::kAbsUpperLimit);
  const static float dcaZMax = getMinimalSelection(femtoDreamTrackSelection::kDCAzMax, femtoDreamSelection::kAbsUpperLimit);
  const static float dcaMin = getMinimalSelection(femtoDreamTrackSelection::kDCAMin, femtoDreamSelection::kAbsLowerLimit);

  if (nPtMinSel > 0 && pT < pTMin) {
    return false;
  }
  if (nPtMaxSel > 0 && pT > pTMax) {
    return false;
  }
  if (nEtaSel > 0 && std::abs(eta) > etaMax) {
    return false;
  }
  if (nTPCnMinSel > 0 && tpcNClsF < nClsMin) {
    return false;
  }
  if (nTPCfMinSel > 0 && tpcRClsC < fClsMin) {
    return false;
  }
  if (nTPCcMinSel > 0 && tpcNClsC < cTPCMin) {
    return false;
  }
  if (nTPCsMaxSel > 0 && tpcNClsS > sTPCMax) {
    return false;
  }
  if (nDCAxyMaxSel > 0 && std::abs(dcaXY) > dcaXYMax) {
    return false;
  }
  if (nDCAzMaxSel > 0 && std::abs(dcaZ) > dcaZMax) {
    return false;
  }
  if (nDCAMinSel > 0 && std::abs(dca) < dcaMin) {
    return false;
  }
  return true;
}

template <typename T1, typename T2>
T1 FemtoDreamTrackSelection::getCutContainer(T2 const& track)
{
  T1 output = 0;
  size_t counter = 0;
  const auto sign = track.sign();
  const auto pT = track.pt();
  const auto eta = track.eta();
  const auto tpcNClsF = track.tpcNClsFound();
  const auto tpcRClsC = track.tpcCrossedRowsOverFindableCls();
  const auto tpcNClsC = track.tpcNClsCrossedRows();
  const auto tpcNClsS = track.tpcNClsShared();
  const auto dcaXY = track.dcaXY();
  const auto dcaZ = track.dcaZ();
  const auto dca = std::sqrt(pow(dcaXY, 2.) + pow(dcaZ, 2.));

  std::vector<float> pidTPC, pidTOF;
  for (auto it : mPIDspecies) {
    pidTPC.push_back(getNsigmaTPC(track, it));
    pidTOF.push_back(getNsigmaTOF(track, it));
  }

  float observable;
  for (auto& sel : mSelections) {
    const auto selVariable = sel.getSelectionVariable();
    if (selVariable == femtoDreamTrackSelection::kPIDnSigmaMax) {
      /// PID needs to be handled a bit differently since we may need more than one species
      for (size_t i = 0; i < pidTPC.size(); ++i) {
        auto pidTPCVal = pidTPC.at(i);
        auto pidTOFVal = pidTOF.at(i);
        sel.checkSelectionSetBit(pidTPCVal, output, counter);
        auto pidComb = std::sqrt(pidTPCVal * pidTPCVal + pidTOFVal * pidTOFVal);
        sel.checkSelectionSetBit(pidComb, output, counter);
      }

    } else {
      /// for the rest it's all the same
      switch (selVariable) {
        case (femtoDreamTrackSelection::kSign):
          observable = sign;
          break;
        case (femtoDreamTrackSelection::kpTMin):
        case (femtoDreamTrackSelection::kpTMax):
          observable = pT;
          break;
        case (femtoDreamTrackSelection::kEtaMax):
          observable = eta;
          break;
        case (femtoDreamTrackSelection::kTPCnClsMin):
          observable = tpcNClsF;
          break;
        case (femtoDreamTrackSelection::kTPCfClsMin):
          observable = tpcRClsC;
          break;
        case (femtoDreamTrackSelection::kTPCcRowsMin):
          observable = tpcNClsC;
          break;
        case (femtoDreamTrackSelection::kTPCsClsMax):
          observable = tpcNClsS;
          break;
        case (femtoDreamTrackSelection::kDCAxyMax):
          observable = dcaXY;
          break;
        case (femtoDreamTrackSelection::kDCAzMax):
          observable = dcaZ;
          break;
        case (femtoDreamTrackSelection::kDCAMin):
          observable = dca;
          break;
        case (femtoDreamTrackSelection::kPIDnSigmaMax):
          break;
      }
      sel.checkSelectionSetBit(observable, output, counter);
    }
  }
  return output;
}

template <typename T>
void FemtoDreamTrackSelection::fillQA(T const& track)
{
  if (mHistogramRegistry) {
    mHistogramRegistry->fill(HIST("TrackCuts/pThist"), track.pt());
    mHistogramRegistry->fill(HIST("TrackCuts/etahist"), track.eta());
    mHistogramRegistry->fill(HIST("TrackCuts/phihist"), track.phi());
    mHistogramRegistry->fill(HIST("TrackCuts/tpcnclshist"), track.tpcNClsFound());
    mHistogramRegistry->fill(HIST("TrackCuts/tpcfclshist"), track.tpcCrossedRowsOverFindableCls());
    mHistogramRegistry->fill(HIST("TrackCuts/tpcnrowshist"), track.tpcNClsCrossedRows());
    mHistogramRegistry->fill(HIST("TrackCuts/tpcnsharedhist"), track.tpcNClsShared());
    mHistogramRegistry->fill(HIST("TrackCuts/dcaXYhist"), track.pt(), track.dcaXY());
    mHistogramRegistry->fill(HIST("TrackCuts/dcaZhist"), track.pt(), track.dcaZ());
    mHistogramRegistry->fill(HIST("TrackCuts/dcahist"), std::sqrt(pow(track.dcaXY(), 2.) + pow(track.dcaZ(), 2.)));
    mHistogramRegistry->fill(HIST("TrackCuts/tpcdEdx"), track.tpcInnerParam(), track.tpcSignal());
    mHistogramRegistry->fill(HIST("TrackCuts/tofSignal"), track.p(), track.tofSignal());
  }
}

} // namespace femtoDream
} // namespace o2::analysis

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMTRACKSELECTION_H_ */
