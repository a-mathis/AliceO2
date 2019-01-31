// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file ParameterGas.h
/// \brief Definition of the parameter class for the detector gas
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ALICEO2_TPC_ParameterGas_H_
#define ALICEO2_TPC_ParameterGas_H_

#include <array>
#include "SimConfig/ConfigurableParam.h"
#include "SimConfig/ConfigurableParamHelper.h"

namespace o2 {
namespace TPC {

struct ParameterGas: public o2::conf::ConfigurableParamHelper<ParameterGas>
{
    /// Get effective ionization potential
    /// \return Effective ionization potential [GeV]
    float getWion() const
    {
      return Wion;
    }

    /// Get first ionization potential
    /// \return First ionization potential [GeV]
    float getIpot() const
    {
      return Ipot;
    }

    /// Get maximum energy we are able to deposit [GeV]
    /// \return Maximum energy we are able to deposit [GeV]
    float getEend() const
    {
      return Eend;
    }

    /// Get the exponent of the energy loss
    /// \return Exponent of the energy loss
    float getExp() const
    {
      return Exp;
    }

    /// Get attachment coefficient
    /// \return Attachment coefficient [1/m]
    float getAttachmentCoefficient() const
    {
      return AttCoeff;
    }

    /// Get oxygen content
    /// \return Oxygen content [1E6 ppm]
    float getOxygenContent() const
    {
      return OxyCont;
    }

    /// Get drift velocity
    /// \return Drift velocify [cm/us]
    float getVdrift() const
    {
      return DriftV;
    }

    /// Get Sigma over Mu
    /// \return Sigma over Mu
    float getSigmaOverMu() const
    {
      return SigmaOverMu;
    }

    /// Get transverse diffusion
    /// \return Transverse diffusion [sqrt(cm)]
    float getDiffT() const
    {
      return DiffT;
    }

    /// Get longitudinal diffusion
    /// \return Longitudinal diffusion [sqrt(cm)]
    float getDiffL() const
    {
      return DiffL;
    }

    /// Get number of primary electrons per cm and MIP
    /// \return Number of primary electrons per cm and MIP [1/cm]
    float getNprim() const
    {
      return Nprim;
    }

    /// Get scale factor to tune WION for GEANT4
    /// \return Scale factor to tune WION for GEANT4
    float getScaleG4() const
    {
      return ScaleFactorG4;
    }

    /// Get parameter for smearing the number of ionizations (nel) using GEANT4
    /// \return Parameter for smearing the number of ionizations (nel) using GEANT4
    float getFanoFactorG4() const
    {
      return FanoFactorG4;
    }

    /// Get Bethe-Bloch parameters
    /// \return Bethe-Bloch parameter
    float getBetheBlochParam(int param) const
    {
      return BetheBlochParam[param];
    }

    float Wion = 37.3e-9f;                                                    ///< Effective ionization potential [GeV]
    float Ipot = 20.77e-9f;                                                       ///< First ionization potential [GeV]
    float Eend = 1e-5f;                                                          ///< Maximum allowed energy loss [GeV]
    float Exp = 2.2f;                                                                  ///< Exponent of the energy loss
    float AttCoeff = 250.f;                                                          ///< Attachement coefficient [1/m]
    float OxyCont = 5.e-6f;                                                             ///< Oxygen content [1E6 ppm]
    float DriftV = 2.58f;                                                               ///< Drift velocity [cm/us]
    float SigmaOverMu = 0.78f;                   ///< Sigma over mu, gives deviation from exponential gain fluctuations
    float DiffT = 0.0209f;                                                         ///< Transverse diffusion [sqrt(cm)]
    float DiffL = 0.0221f;                                                       ///< Longitudinal diffusion [sqrt(cm)]
    float Nprim = 14.f;                                          ///< Number of primary electrons per MIP and cm [1/cm]
    float ScaleFactorG4 = 0.85f;                                              ///< Scale factor to tune WION for GEANT4
    float FanoFactorG4 = 0.7f;                 ///< Parameter for smearing the number of ionizations (nel) using GEANT4
    float BetheBlochParam[5] = { 0.76176e-1f, 10.632f, 0.13279e-4f, 1.8631f, 1.9479f }; ///< Parametrization of Bethe-Bloch

  O2ParamDef(ParameterGas, "TPCGasParam")
    ;
};

} // namespace TPC
} // namespace o2

#endif // ALICEO2_TPC_ParameterGas_H_
