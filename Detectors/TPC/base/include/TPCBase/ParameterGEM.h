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

/// \file ParameterGEM.h
/// \brief Definition of the parameter class for the GEM stack
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

// Remark: This file has been modified by Viktor Ratza in order to
// implement the electron efficiency models for the collection and the
// extraction efficiency.

#ifndef ALICEO2_TPC_ParameterGEM_H_
#define ALICEO2_TPC_ParameterGEM_H_

#include <array>
#include <cmath>
#include "CommonUtils/ConfigurableParam.h"
#include "CommonUtils/ConfigurableParamHelper.h"

namespace o2
{
namespace tpc
{

enum class AmplificationMode : char {
  FullMode = 0,      ///< Full 4-GEM simulation of all efficiencies etc.
  EffectiveMode = 1, ///< Effective amplification mode using one polya distribution only
};

enum class PadResponseMode : char {
  FullPadResponse = 0,       ///< Pad response considering the charge spread during the amplification stage and induction effects
  ProjectivePadResponse = 1, ///< Purely projective pad response
};

struct ParameterGEM : public o2::conf::ConfigurableParamHelper<ParameterGEM> {

  /// Get the effective gain of a given GEM in the stack
  /// \param GEM GEM of interest in the stack (1 - 4)
  /// \return Effective gain of a given GEM in the stack
  float getEffectiveGain(int gem) const
  {
    return CollectionEfficiency[gem] * AbsoluteGain[gem] * ExtractionEfficiency[gem];
  }

  static constexpr int nGEMS = 4;                                   ///< Number of GEMs in stack
  int Geometry[nGEMS] = {0, 2, 2, 0};                               ///< GEM geometry (0 standard, 1 medium, 2 large)
  float Distance[nGEMS + 1] = {4.f, 0.2f, 0.2f, 0.2f, 0.2f};        ///< Distances between cathode/anode and stages (in cm)
  float Potential[nGEMS] = {270.f, 250.f, 270.f, 340.f};            ///< Potential (in Volts)
  float ElectricField[nGEMS + 1] = {0.4f, 4.f, 2.f, 0.1f, 4.f};     ///< Electric field configuration (in kV/cm)
  float AbsoluteGain[nGEMS] = {14.f, 8.f, 53.f, 240.f};             ///< Absolute gain
  float CollectionEfficiency[nGEMS] = {1.f, 0.2f, 0.25f, 1.f};      ///< Collection efficiency
  float ExtractionEfficiency[nGEMS] = {0.65f, 0.55f, 0.12f, 0.6f};  ///< Extraction efficiency
  float TotalGainStack = 2000.f;                                    ///< Total gain of the stack for the EffectiveMode
  float KappaStack = 1.205f;                                        ///< Variable steering the energy resolution of the full stack for the EffectiveMode
  float EfficiencyStack = 0.528f;                                   ///< Variable steering the single electron efficiency of the full stack for the EffectiveMode
  AmplificationMode AmplMode = AmplificationMode::EffectiveMode;    ///< Amplification mode [FullMode / EffectiveMode]
  PadResponseMode PRFmode = PadResponseMode::ProjectivePadResponse; ///< Treatment of the pad response [FullPadResponse / ProjectivePadResponse]

  O2ParamDef(ParameterGEM, "TPCGEMParam");
};
} // namespace tpc
} // namespace o2

#endif // ALICEO2_TPC_ParameterGEM_H_
