// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file testSAMPAProcessing.cxx
/// \brief This task tests the Pad Response module of the TPC digitization
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#define BOOST_TEST_MODULE Test TPC Pad Response
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "TPCSimulation/PadResponse.h"
#include "TPCBase/Mapper.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "FairLogger.h"

namespace o2 {
namespace TPC {

  /// \brief Test of the import and interpolation
  /// read in the file on which the PRFs are based and compare the final value from the function to the contents of the file
  BOOST_AUTO_TEST_CASE(PadResponse_test)
  {
    static PadResponse padResponse;
    const Mapper& mapper = Mapper::instance();

    /// Provide proper path to the PRF files
    std::string inputDir;
    const char* aliceO2env=std::getenv("O2_ROOT");
    if (aliceO2env) inputDir=aliceO2env;
    inputDir+="/share/Detectors/TPC/files/";
    const std::vector<std::string> files = {{"PRF_IROC.dat", "PRF_OROC1-2.dat", "PRF_OROC3.dat"}};

    /// use exemplary CRU for IROC, OROC 1-2 and OROC3
    const std::vector<CRU> cru = {{CRU(2), CRU(6), CRU(8)}};
    /// use exemplary pad position
    const PadPos padPos(10, 10);

    for(int i=0; i<static_cast<int>(files.size()); ++i) {
      DigitPos digiPos(cru[i], padPos);
      const GEMstack gemStack = digiPos.getCRU().gemStack();
      LocalPosition2D padCentre = mapper.findPadCentreFromDigitPos(digiPos);

      float x, y, normalizedPadResponse;
      std::ifstream prfFile(inputDir+files[i], std::ifstream::in);
      if(!prfFile) {
        std::cout << "TPC::PadResponse - Input file '" << inputDir+files[i] << "' does not exist! No PRF loaded!" << FairLogger::endl;
        BOOST_CHECK(false);
      }
      for (std::string line; std::getline(prfFile, line); ) {
        std::istringstream is(line);
        while(is >> x >> y >> normalizedPadResponse) {
          /// check direct call via GEMstack and offset
          BOOST_CHECK_CLOSE(padResponse.getPadResponse(gemStack, x, y), normalizedPadResponse, 1E-12);

          /// check indirect call via pad centre
          LocalPosition3D padCentreShiftLocal(padCentre.X()+x*0.1f, padCentre.Y()+y*0.1f, 10.f);
          GlobalPosition3D padCentreShiftGlobal = Mapper::LocalToGlobal(padCentreShiftLocal, cru[i].sector());
          const DigitPos digiPadPos = mapper.findDigitPosFromGlobalPosition(padCentreShiftGlobal);
          const float padResp = padResponse.getPadResponse(padCentreShiftGlobal, digiPadPos, digiPos);
          if(padResp < 1e-8f || normalizedPadResponse < 1e-8f) continue;
          /// \todo we currently have some floating point problems with the shifted position, therefore the precision cannot be better than 1%
          /// \todo this should be fixed soon
          BOOST_CHECK_CLOSE(padResp, normalizedPadResponse, 1);
        }
      }
    }
  }
}
}
