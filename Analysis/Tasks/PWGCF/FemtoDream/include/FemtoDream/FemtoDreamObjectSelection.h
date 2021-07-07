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

/// \file FemtoDreamObjectSelection.h
/// \brief FemtoDreamObjectSelection - Partent class of all selections
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMOBJECTSELECTION_H_
#define ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMOBJECTSELECTION_H_

#include "FemtoDreamSelection.h"

#include "ReconstructionDataFormats/PID.h"
#include "Framework/HistogramRegistry.h"
#include <Rtypes.h>

#include <iostream>
#include <cmath>

using namespace o2::framework;

namespace o2::analysis
{
namespace femtoDream
{

/// \class FemtoDreamObjectSelection
/// \brief Cut class to contain and execute all cuts applied to tracks
/// \todo In principle all cuts that fulfill the getMinimalSelection are done implicitly and can be removed from the vector containing all cuts

template <class T1, class T2>
class FemtoDreamObjectSelection
{
 public:
  /// Destructor
  virtual ~FemtoDreamObjectSelection() = default;

  template <o2::aod::femtodreamparticle::ParticleType part>
  void fillSelectionHistogram()
  {
    int nBins = mSelections.size();
    mHistogramRegistry->add((static_cast<std::string>(o2::aod::femtodreamparticle::ParticleTypeName[part]) + "/cuthist").c_str(), "; Cut; Value", kTH1F, {{nBins, 0, static_cast<double>(nBins)}});
    auto hist = mHistogramRegistry->get<TH1>(HIST(o2::aod::femtodreamparticle::ParticleTypeName[part]) + HIST("/cuthist"));
    for (size_t i = 0; i < mSelections.size(); ++i) {
      hist->GetXaxis()->SetBinLabel(i + 1, Form("%u", mSelections.at(i).getSelectionVariable()));
      hist->SetBinContent(i + 1, mSelections.at(i).getSelectionValue());
    }
  }

  template <typename T>
  void setSelection(T& selVals, T2 selVar, femtoDreamSelection::SelectionType selType)
  {
    std::vector<T1> tmpSelVals = selVals; // necessary due to some features of the Configurable
    std::vector<FemtoDreamSelection<T1, T2>> tempVec;
    for (const T1 selVal : tmpSelVals) {
      tempVec.push_back(FemtoDreamSelection<T1, T2>(selVal, selVar, selType));
    }
    setSelection(tempVec);
  }

  void setSelection(std::vector<FemtoDreamSelection<T1, T2>>& sels)
  {
    switch (sels.at(0).getSelectionType()) {
      case (femtoDreamSelection::SelectionType::kUpperLimit):
      case (femtoDreamSelection::SelectionType::kAbsUpperLimit):
        std::sort(sels.begin(), sels.end(), [](FemtoDreamSelection<T1, T2> a, FemtoDreamSelection<T1, T2> b) {
          return a.getSelectionValue() > b.getSelectionValue();
        });
        break;
      case (femtoDreamSelection::SelectionType::kLowerLimit):
      case (femtoDreamSelection::SelectionType::kAbsLowerLimit):
      case (femtoDreamSelection::SelectionType::kEqual):
        std::sort(sels.begin(), sels.end(), [](FemtoDreamSelection<T1, T2> a, FemtoDreamSelection<T1, T2> b) {
          return a.getSelectionValue() < b.getSelectionValue();
        });
        break;
    }
    for (const auto sel : sels) {
      mSelections.push_back(sel);
    }
  }

  T1 getMinimalSelection(T2 selVar, femtoDreamSelection::SelectionType selType)
  {
    T1 minimalSel;
    switch (selType) {
      case (femtoDreamSelection::SelectionType::kUpperLimit):
      case (femtoDreamSelection::SelectionType::kAbsUpperLimit):
        minimalSel = -999.e9;
        break;
      case (femtoDreamSelection::SelectionType::kLowerLimit):
      case (femtoDreamSelection::SelectionType::kAbsLowerLimit):
      case (femtoDreamSelection::SelectionType::kEqual):
        minimalSel = 999.e9;
        break;
    }

    for (auto sel : mSelections) {
      if (sel.getSelectionVariable() == selVar) {
        switch (sel.getSelectionType()) {
          case (femtoDreamSelection::SelectionType::kUpperLimit):
          case (femtoDreamSelection::SelectionType::kAbsUpperLimit):
            if (minimalSel < sel.getSelectionValue()) {
              minimalSel = sel.getSelectionValue();
            }
            break;
          case (femtoDreamSelection::SelectionType::kLowerLimit):
          case (femtoDreamSelection::SelectionType::kAbsLowerLimit):
          case (femtoDreamSelection::SelectionType::kEqual):
            if (minimalSel > sel.getSelectionValue()) {
              minimalSel = sel.getSelectionValue();
            }
            break;
        }
      }
    }
    return minimalSel;
  }

  size_t getNSelections()
  {
    return mSelections.size();
  }

  size_t getNSelections(T2 selVar)
  {
    return getSelections(selVar).size();
  }

  std::vector<FemtoDreamSelection<T1,T2>> getSelections(T2 selVar)
  {
    std::vector<FemtoDreamSelection<T1,T2>> selValVec;
    for (auto it : mSelections) {
      if (it.getSelectionVariable() == selVar) {
        selValVec.push_back(it);
      }
    }
    return selValVec;
  }

  std::vector<T2> getSelectionTypes()
  {
    std::vector<T2> selVarVec;
    for (auto it : mSelections) {
      auto selVar = it.getSelectionVariable();
      if (std::none_of(selVarVec.begin(), selVarVec.end(), [selVar](T2 a) { return a == selVar; })) {
        selVarVec.push_back(selVar);
      }
    }
    return selVarVec;
  }

 protected:
  std::vector<FemtoDreamSelection<T1, T2>> mSelections; ///< Vector containing all selections
  HistogramRegistry* mHistogramRegistry;                ///< For QA output

  ClassDefNV(FemtoDreamObjectSelection, 1);
};

} // namespace femtoDream
} // namespace o2::analysis

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMOBJECTSELECTION_H_ */
