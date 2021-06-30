// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FemtoDreamV0Selection.h
/// \brief Definition of the FemtoDreamV0Selection
/// \author Valentina Mantovani Sarti, TU München valentina.mantovani-sarti@tum.de and Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMV0SELECTION_H_
#define ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMV0SELECTION_H_

#include "FemtoDreamObjectSelection.h"
#include "FemtoDreamTrackSelection.h"
#include "FemtoDreamSelection.h"

#include "ReconstructionDataFormats/PID.h"
#include "AnalysisCore/RecoDecay.h"
#include "Framework/HistogramRegistry.h"
#include <Rtypes.h>
#include <cmath>

using namespace o2::framework;

namespace o2::analysis
{
namespace femtoDream
{
namespace femtoDreamV0Selection
{
enum V0Sel { kpTV0Min,
             kpTV0Max,
             kDCAV0DaughMax,
             kCPAV0Min,
             kTranRadV0Min,
             kTranRadV0Max,
             kDecVtxMax,
             kInvMassV0Max,
             kInvMassRejMax };
enum ChildTracks { kPosDaughTrack,
                   kNegDaughTrack };
enum class V0Type { kK0s,
                    kLambda,
                    kAntiLambda };
} // namespace femtoDreamV0Selection

/// \class FemtoDreamV0Selection
/// \brief Cut class to contain and execute all cuts applied to V0s
class FemtoDreamV0Selection : public FemtoDreamObjectSelection<float, femtoDreamV0Selection::V0Sel>
{
 public:
  /// Initializes histograms for the task
  void init(HistogramRegistry* registry);

  template <typename T>       ///not sure if needed for now
  void setPIDSpecies(T& pids) ///K0s,Λ and antiΛ
  {
    std::vector<int> tmpPids = pids; // necessary due to some features of the configurable
    for (const o2::track::PID& pid : tmpPids) {
      mPIDspecies.push_back(pid);
    }
  }
  /// basic function to set from the task which kind of V0s has to be analysed
  void setTypev0(femtoDreamV0Selection::V0Type v0type)
  {
    mSelV0 = v0type;
  }

  template <typename C, typename V>
  bool isSelectedMinimal(C const& col, V const& v0);

  template <typename C, typename V, typename T>
  std::vector<uint64_t> getCutContainer(C const& col, V const& v0, T const& posTrack, T const& negTrack);

  template <typename C, typename V>
  void fillQA(C const& col, V const& v0);

  template <typename T1, typename T2>
  void setChildCuts(femtoDreamV0Selection::ChildTracks child, T1 selVal, T2 selVar, femtoDreamSelection::SelectionType selType);

 private:
  FemtoDreamTrackSelection PosDaughTrack;
  FemtoDreamTrackSelection NegDaughTrack;
  std::vector<o2::track::PID> mPIDspecies;
  femtoDreamV0Selection::V0Type mSelV0;
  float mMassV0 = 0.;
  float mMassRej = 0.;
  float mMassPDGV0 = 0.;
  float mMassPDGRej = 0.;

  ClassDefNV(FemtoDreamV0Selection, 1);
}; // namespace femtoDream

void FemtoDreamV0Selection::init(HistogramRegistry* registry)
{
  if (registry) {
    mHistogramRegistry = registry;
    fillSelectionHistogram("V0Cuts/cuthist"); ///For now empty since I need to understand the next nSelections

    /// \todo this should be an automatic check in the parent class, and the return type should be templated
    // int nSelections = 2 + getNSelections() + mPIDspecies.size() * (getNSelections(femtoDreamTrackSelection::kPIDnSigmaMax) - 1);
    // if (8 * sizeof(uint64_t) < nSelections) {
    //   LOGF(error, "Number of selections to large for your container - quitting!");
    // }
    mHistogramRegistry->add("V0Cuts/pThist", "; #it{p}_{T} (GeV/#it{c}); Entries", kTH1F, {{1000, 0, 10}});
    mHistogramRegistry->add("V0Cuts/etahist", "; #eta; Entries", kTH1F, {{1000, -1, 1}});
    mHistogramRegistry->add("V0Cuts/phihist", "; #phi; Entries", kTH1F, {{1000, 0, 2. * M_PI}});
    mHistogramRegistry->add("V0Cuts/dcaDauToVtx", "; DCADaug_{Vtx} (cm); Entries", kTH1F, {{1000, 0, 10}});
    mHistogramRegistry->add("V0Cuts/transRadius", "; #it{r}_{xy} (cm); Entries", kTH1F, {{1500, 0, 150}});
    mHistogramRegistry->add("V0Cuts/decayVtxXPV", "; #it{iVtx}_{x} (cm); Entries", kTH1F, {{2000, 0, 200}});
    mHistogramRegistry->add("V0Cuts/decayVtxYPV", "; #it{iVtx}_{y} (cm)); Entries", kTH1F, {{2000, 0, 200}});
    mHistogramRegistry->add("V0Cuts/decayVtxZPV", "; #it{iVtx}_{z} (cm); Entries", kTH1F, {{2000, 0, 200}});
    mHistogramRegistry->add("V0Cuts/invMass", "; #it{M}_{inv} (GeV/#it{c}^{2}); Entries", kTH1F, {{400, 1., 1.2}});
    mHistogramRegistry->add("V0Cuts/cpa", "; #it{cos(#alpha)}; Entries", kTH1F, {{1000, 0.9, 1.}});
    mHistogramRegistry->add("V0Cuts/invMassPt", "; #it{p}_{T} (GeV/#it{c}); #it{M}_{inv} (GeV/#it{c}^{2})", kTH2F, {{8, 0.3, 4.3}, {40, 1., 1.2}});
    mHistogramRegistry->add("V0Cuts/cpapTBins", "; #it{p}_{T} (GeV/#it{c}); #it{cos(#alpha)}", kTH2F, {{8, 0.3, 4.3}, {1000, 0.9, 1.}});

    const int nPtV0MinSel = getNSelections(femtoDreamV0Selection::kpTV0Min);
    const int nPtV0MaxSel = getNSelections(femtoDreamV0Selection::kpTV0Max);
    const int nDCAV0DaughMax = getNSelections(femtoDreamV0Selection::kDCAV0DaughMax);
    const int nCPAV0Min = getNSelections(femtoDreamV0Selection::kCPAV0Min);
    const int nTranRadV0Min = getNSelections(femtoDreamV0Selection::kTranRadV0Min);
    const int nTranRadV0Max = getNSelections(femtoDreamV0Selection::kTranRadV0Max);
    const int nDecVtxMax = getNSelections(femtoDreamV0Selection::kDecVtxMax);
    const int nInvMassV0Max = getNSelections(femtoDreamV0Selection::kInvMassV0Max);
    const int nInvMassRejMax = getNSelections(femtoDreamV0Selection::kInvMassRejMax);
  }
}

template <typename C, typename V>
bool FemtoDreamV0Selection::isSelectedMinimal(C const& col, V const& v0)
{
  const float pT = v0.pt();
  const std::vector<float> decVtx = {v0.x(), v0.y(), v0.z()};
  const float tranRad = v0.v0radius();
  const float dcaDaughv0 = v0.dcaV0daughters();
  const float cpav0 = v0.v0cosPA(col.posX(), col.posY(), col.posZ());
  switch (mSelV0) {
    case (femtoDreamV0Selection::V0Type::kK0s):
      mMassV0 = v0.mK0Short();
      mMassPDGV0 = RecoDecay::getMassPDG(310);
      mMassRej = v0.mLambda();
      mMassPDGRej = RecoDecay::getMassPDG(3122);
      break;
    case (femtoDreamV0Selection::V0Type::kLambda):
      mMassV0 = v0.mLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
    case (femtoDreamV0Selection::V0Type::kAntiLambda):
      mMassV0 = v0.mAntiLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
  }

  /// check whether the most open cuts are fulfilled - most of this should have already be done by the filters
  const static int nPtV0MinSel = getNSelections(femtoDreamV0Selection::kpTV0Min);
  const static int nPtV0MaxSel = getNSelections(femtoDreamV0Selection::kpTV0Max);
  const static int nDCAV0DaughMax = getNSelections(femtoDreamV0Selection::kDCAV0DaughMax);
  const static int nCPAV0Min = getNSelections(femtoDreamV0Selection::kCPAV0Min);
  const static int nTranRadV0Min = getNSelections(femtoDreamV0Selection::kTranRadV0Min);
  const static int nTranRadV0Max = getNSelections(femtoDreamV0Selection::kTranRadV0Max);
  const static int nDecVtxMax = getNSelections(femtoDreamV0Selection::kDecVtxMax);
  const static int nInvMassV0Max = getNSelections(femtoDreamV0Selection::kInvMassV0Max);
  const static int nInvMassRejMax = getNSelections(femtoDreamV0Selection::kInvMassRejMax);

  const static float pTV0Min = getMinimalSelection(femtoDreamV0Selection::kpTV0Min, femtoDreamSelection::kLowerLimit);
  const static float pTV0Max = getMinimalSelection(femtoDreamV0Selection::kpTV0Max, femtoDreamSelection::kUpperLimit);
  const static float DCAV0DaughMax = getMinimalSelection(femtoDreamV0Selection::kDCAV0DaughMax, femtoDreamSelection::kUpperLimit);
  const static float CPAV0Min = getMinimalSelection(femtoDreamV0Selection::kCPAV0Min, femtoDreamSelection::kLowerLimit);
  const static float TranRadV0Min = getMinimalSelection(femtoDreamV0Selection::kTranRadV0Min, femtoDreamSelection::kLowerLimit);
  const static float TranRadV0Max = getMinimalSelection(femtoDreamV0Selection::kTranRadV0Max, femtoDreamSelection::kUpperLimit);
  const static float DecVtxMax = getMinimalSelection(femtoDreamV0Selection::kDecVtxMax, femtoDreamSelection::kAbsUpperLimit);
  const static float InvMassV0Max = getMinimalSelection(femtoDreamV0Selection::kInvMassV0Max, femtoDreamSelection::kUpperLimit);
  const static float InvMassRejMax = getMinimalSelection(femtoDreamV0Selection::kInvMassRejMax, femtoDreamSelection::kUpperLimit);

  if (nPtV0MinSel > 0 && pT < pTV0Min) {
    return false;
  }
  if (nPtV0MaxSel > 0 && pT > pTV0Max) {
    return false;
  }
  if (nDCAV0DaughMax > 0 && dcaDaughv0 > DCAV0DaughMax) {
    return false;
  }
  if (nCPAV0Min > 0 && cpav0 < CPAV0Min) {
    return false;
  }
  if (nTranRadV0Min > 0 && tranRad < TranRadV0Min) {
    return false;
  }
  if (nTranRadV0Max > 0 && tranRad > TranRadV0Max) {
    return false;
  }
  for (int i = 0; i < decVtx.size(); i++) {
    if (nDecVtxMax > 0 && decVtx.at(i) > DecVtxMax) {
      return false;
    }
  }
  if (nInvMassV0Max > 0 && std::abs(mMassPDGV0 - mMassV0) > InvMassV0Max) {
    return false;
  }
  if (nInvMassRejMax > 0 && std::abs(mMassPDGRej - mMassRej) > InvMassRejMax) {
    return false;
  }
  return true;
}

template <typename T1, typename T2>
void FemtoDreamV0Selection::setChildCuts(femtoDreamV0Selection::ChildTracks child, T1 selVal, T2 selVar, femtoDreamSelection::SelectionType selType)
{
  if (child == femtoDreamV0Selection::kPosDaughTrack)
    PosDaughTrack.setSelection(selVal, selVar, selType);
  else if (child == femtoDreamV0Selection::kNegDaughTrack)
    NegDaughTrack.setSelection(selVal, selVar, selType);
}

/// the CosPA of V0 needs as argument the posXYZ of collisions vertex so we need to pass the collsion as well
template <typename C, typename V, typename T>
std::vector<uint64_t> FemtoDreamV0Selection::getCutContainer(C const& col, V const& v0, T const& posTrack, T const& negTrack)
{
  posTrack = PosDaughTrack;
  negTrack = NegDaughTrack;
  uint64_t outputPosTrack = posTrack.getCutContainer(posTrack);
  uint64_t outputNegTrack = negTrack.getCutContainer(negTrack);
  uint64_t output = 0;
  size_t counter = 0;
  const auto signPos = posTrack.sign();
  const auto signNeg = negTrack.sign();
  if (signPos < 0 || signNeg > 0) {
    printf("ERROR - Wrong sign for V0 daughters");
  }
  const auto pT = v0.pt();
  const auto tranRad = v0.v0radius();
  const auto dcaDaughv0 = v0.dcaV0daughters();
  const auto cpav0 = v0.v0cosPA(col.posX(), col.posY(), col.posZ());
  const std::vector<float> decVtx = {v0.x(), v0.y(), v0.z()};

  switch (mSelV0) {
    case (femtoDreamV0Selection::V0Type::kK0s):
      mMassV0 = v0.mK0Short();
      mMassPDGV0 = RecoDecay::getMassPDG(310);
      mMassRej = v0.mLambda();
      mMassPDGRej = RecoDecay::getMassPDG(3122);
      break;
    case (femtoDreamV0Selection::V0Type::kLambda):
      mMassV0 = v0.mLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
    case (femtoDreamV0Selection::V0Type::kAntiLambda):
      mMassV0 = v0.mAntiLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
  }

  float observable;
  for (auto& sel : mSelections) {
    const auto selVariable = sel.getSelectionVariable();
    if (selVariable == femtoDreamV0Selection::kDecVtxMax) {
      for (size_t i = 0; i < decVtx.size(); ++i) {
        auto decVtxValue = decVtx.at(i);
        sel.checkSelectionSetBit(decVtxValue, output, counter);
      }
    } else {
      switch (selVariable) {
        case (femtoDreamV0Selection::kpTV0Min):
        case (femtoDreamV0Selection::kpTV0Max):
          observable = pT;
          break;
        case (femtoDreamV0Selection::kDCAV0DaughMax):
          observable = dcaDaughv0;
          break;
        case (femtoDreamV0Selection::kCPAV0Min):
          observable = cpav0;
          break;
        case (femtoDreamV0Selection::kTranRadV0Min):
        case (femtoDreamV0Selection::kTranRadV0Max):
          observable = tranRad;
          break;
        case (femtoDreamV0Selection::kInvMassV0Max):
          observable = std::abs(mMassPDGV0 - mMassV0);
          break;
        case (femtoDreamV0Selection::kInvMassRejMax):
          observable = std::abs(mMassPDGRej - mMassRej);
          break;
        case (femtoDreamV0Selection::kDecVtxMax):
          break;
      }
      sel.checkSelectionSetBit(observable, output, counter);
    }
  }
  return {{outputPosTrack, outputNegTrack, output}};
}

template <typename C, typename V>
void FemtoDreamV0Selection::fillQA(C const& col, V const& v0)
{
  switch (mSelV0) {
    case (femtoDreamV0Selection::V0Type::kK0s):
      mMassV0 = v0.mK0Short();
      mMassPDGV0 = RecoDecay::getMassPDG(310);
      mMassRej = v0.mLambda();
      mMassPDGRej = RecoDecay::getMassPDG(3122);
      break;
    case (femtoDreamV0Selection::V0Type::kLambda):
      mMassV0 = v0.mLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
    case (femtoDreamV0Selection::V0Type::kAntiLambda):
      mMassV0 = v0.mAntiLambda();
      mMassPDGV0 = RecoDecay::getMassPDG(3122);
      mMassRej = v0.mK0Short();
      mMassPDGRej = RecoDecay::getMassPDG(310);
      break;
  }
  if (mHistogramRegistry) {
    mHistogramRegistry->fill(HIST("V0Cuts/pThist"), v0.pt());
    mHistogramRegistry->fill(HIST("V0Cuts/etahist"), v0.eta());
    mHistogramRegistry->fill(HIST("V0Cuts/phihist"), v0.phi());
    mHistogramRegistry->fill(HIST("V0Cuts/dcaDauToVtx"), v0.dcaV0daughters());
    mHistogramRegistry->fill(HIST("V0Cuts/transRadius"), v0.v0radius());
    mHistogramRegistry->fill(HIST("V0Cuts/dcaPosDauToPV"), v0.dcapostopv());
    mHistogramRegistry->fill(HIST("V0Cuts/dcaNegDauToPV"), v0.dcanegtopv());
    mHistogramRegistry->fill(HIST("V0Cuts/decayVtxXPV"), v0.x());
    mHistogramRegistry->fill(HIST("V0Cuts/decayVtxYPV"), v0.y());
    mHistogramRegistry->fill(HIST("V0Cuts/decayVtxZPV"), v0.z());
    mHistogramRegistry->fill(HIST("V0Cuts/invMass"), mMassV0);
    mHistogramRegistry->fill(HIST("V0Cuts/cpa"), v0.v0cosPA(col.posX(), col.posY(), col.posZ()));
    mHistogramRegistry->fill(HIST("V0Cuts/invMassPt"), v0.pt(), mMassV0);
    mHistogramRegistry->fill(HIST("V0Cuts/cpapTBins"), v0.pt(), v0.v0cosPA(col.posX(), col.posY(), col.posZ()));
  }
}

} // namespace femtoDream
} // namespace o2::analysis

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMV0SELECTION_H_ */
