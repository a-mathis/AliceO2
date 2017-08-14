// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file PadResponse.cxx
/// \brief Implementation of the Pad Response
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#include "TPCSimulation/PadResponse.h"

#include "TPCBase/Mapper.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "FairLogger.h"

using namespace o2::TPC;

PadResponse::PadResponse()
  : mIROC(),
    mOROC12(),
    mOROC3()
{
  mIROC   = std::make_unique<TGraph2D>();
  mOROC12 = std::make_unique<TGraph2D>();
  mOROC3  = std::make_unique<TGraph2D>();
  
  importPRF("PRF_IROC.dat", mIROC);
  importPRF("PRF_OROC1-2.dat", mOROC12);
  importPRF("PRF_OROC3.dat", mOROC3);
}

bool PadResponse::importPRF(std::string file, std::unique_ptr<TGraph2D> & grPRF) const
{
  std::string inputDir;
  const char* aliceO2env=std::getenv("O2_ROOT");
  if (aliceO2env) inputDir=aliceO2env;
  inputDir+="/share/Detectors/TPC/files/";

  float x, y, normalizedPadResponse;
  int i=0;
  std::ifstream prfFile(inputDir+file, std::ifstream::in);
  if(!prfFile) {
    LOG(FATAL) << "TPC::PadResponse - Input file '" << inputDir+file << "' does not exist! No PRF loaded!" << FairLogger::endl;
    return false;
  }
  for (std::string line; std::getline(prfFile, line); ) {
    std::istringstream is(line);
    while(is >> x >> y >> normalizedPadResponse) {
      grPRF->SetPoint(i++, x, y, normalizedPadResponse);
    }
  }
  return true;
}

float PadResponse::getPadResponse(const GlobalPosition3D &posEle, const DigitPos &digiElePos, const DigitPos &digiPadPos) const
{
  /// Find the local position of the centre of the pad of interest
  const static Mapper &mapper = Mapper::instance();
  LocalPosition2D padCentreLocal = mapper.findPadCentreFromDigitPos(digiPadPos);

  /// Convert the electron position into local coordinates
  LocalPosition3D elePosLocal = Mapper::GlobalToLocal(posEle, digiElePos.getCRU().sector());

  /// Compare the pad centre and the electron avalanche
  const float offsetX = std::fabs(elePosLocal.X() - padCentreLocal.X())*10.f; /// GlobalPosition3D and DigitPos in cm, PRF in mm
  const float offsetY = std::fabs(elePosLocal.Y() - padCentreLocal.Y())*10.f; /// GlobalPosition3D and DigitPos in cm, PRF in mm

  /// Apply the PRF
  const GEMstack gemStack = digiPadPos.getCRU().gemStack();
  return getPadResponse(gemStack, offsetX, offsetY);
}

float PadResponse::getPadResponse(const GEMstack &gemStack, const float offsetX, const float offsetY) const
{
  float normalizedPadResponse = 0.f;
  switch (gemStack) {
  case IROCgem: {
    normalizedPadResponse = mIROC->Interpolate(offsetX, offsetY);
    break;
  }
  case OROC1gem: {
    normalizedPadResponse = mOROC12->Interpolate(offsetX, offsetY);
    break;
  }
  case OROC2gem: {
    normalizedPadResponse = mOROC12->Interpolate(offsetX, offsetY);
    break;
  }
  case OROC3gem: {
    normalizedPadResponse = mOROC3->Interpolate(offsetX, offsetY);
    break;
  }
  }
  if(normalizedPadResponse > 1.f) return 1.f;
  return normalizedPadResponse;
}


