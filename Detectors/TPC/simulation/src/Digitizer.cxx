// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Digitizer.cxx
/// \brief Implementation of the ALICE TPC digitizer
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#include "TPCSimulation/Digitizer.h"
#include "TPCSimulation/ElectronTransport.h"
#include "TPCSimulation/GEMAmplification.h"
#include "TPCSimulation/PadResponse.h"
#include "TPCSimulation/Point.h"
#include "TPCSimulation/SAMPAProcessing.h"

#include "TPCBase/Mapper.h"

#include "FairLogger.h"

ClassImp(o2::TPC::Digitizer)

using namespace o2::TPC;

bool o2::TPC::Digitizer::mDebugFlagPRF = false;
bool o2::TPC::Digitizer::mIsContinuous = true;
bool o2::TPC::Digitizer::mUsePadResponse = true;

Digitizer::Digitizer()
  : mDigitContainer(nullptr),
    mDebugTreePRF(nullptr)
{}

Digitizer::~Digitizer()
{
  delete mDigitContainer;
}

void Digitizer::init()
{
  /// Initialize the task and the output container
  /// \todo get rid of new? check with Mohammad
  mDigitContainer = new DigitContainer();
}

DigitContainer *Digitizer::Process(TClonesArray *points)
{
//  mDigitContainer->reset();
  const static Mapper& mapper = Mapper::instance();
  const static ParameterDetector &detParam = ParameterDetector::defaultInstance();
  const static ParameterElectronics &eleParam = ParameterElectronics::defaultInstance();
  FairRootManager *mgr = FairRootManager::Instance();

  const float eventTime = ( mIsContinuous) ? mgr->GetEventTime() * 0.001 : 0.f; /// transform in us

  /// \todo static_thread for thread savety?
  static GEMAmplification gemAmplification;
  static ElectronTransport electronTransport;
  static PadResponse padResponse;

  const int nShapedPoints = eleParam.getNShapedPoints();
  static std::vector<float> signalArray;
  signalArray.resize(nShapedPoints);

  static size_t hitCounter=0;
  for(auto pointObject : *points) {
#ifdef TPC_GROUPED_HITS
    auto *inputgroup = static_cast<LinkableHitGroup*>(pointObject);
    const int MCTrackID = inputgroup->GetTrackID();
    for(size_t hitindex = 0; hitindex<inputgroup->getSize(); ++hitindex){
      ElementalHit eh = inputgroup->getHit(hitindex);
      auto *inputpoint = &eh;
#else
    Point *inputpoint = static_cast<Point *>(pointObject);
    const int MCTrackID = inputpoint->GetTrackID();
#endif

    const GlobalPosition3D posEle(inputpoint->GetX(), inputpoint->GetY(), inputpoint->GetZ());

    /// The energy loss stored is really nElectrons
    const int nPrimaryElectrons = static_cast<int>(inputpoint->GetEnergyLoss());

    /// Loop over electrons
    /// \todo can be vectorized?
    /// \todo split transport and signal formation in two separate loops?
    for(int iEle=0; iEle < nPrimaryElectrons; ++iEle) {

      /// Drift and Diffusion
      const GlobalPosition3D posEleDiff = electronTransport.getElectronDrift(posEle);

      /// \todo Time management in continuous mode (adding the time of the event?)
      const float driftTime = getTime(posEleDiff.Z()) + inputpoint->GetTime() * 0.001; /// in us
      const float absoluteTime = driftTime + eventTime;

      /// Attachment
      if(electronTransport.isElectronAttachment(driftTime)) continue;

      /// Remove electrons that end up outside the active volume
      if(fabs(posEleDiff.Z()) > detParam.getTPClength()) continue;

      /// Find the pad below the incoming electron
      const DigitPos digiPadPos = mapper.findDigitPosFromGlobalPosition(posEleDiff);
      if(!digiPadPos.isValid()) continue;

      /// Electron multiplication in the GEM stack
      const int nElectronsGEM = gemAmplification.getStackAmplification();
      if ( nElectronsGEM == 0 ) continue;

      const int pad = digiPadPos.getPadPos().getPad();
      const int row = digiPadPos.getPadPos().getRow();
      const int globalRow = digiPadPos.getGlobalPadPos().getRow();
      const CRU cru(digiPadPos.getCRU());
      std::cout << " === CRU " << cru.number() << " Row " << row << " Global Row " << globalRow << " Pad " << pad << "\n";

      if(mUsePadResponse) {
        /// Pad response function
        /// Loop over two neighboring pads and rows around the centre of the electron avalanche and compute the correspondingly induced signal on those pads
        const int nRowsInCRU = mapper.getNumberOfRowsPartition(cru);
        const int cPad = pad - mapper.getNumberOfPadsInRowSector(globalRow)/2;
        for(int ipad = -2; ipad < 3; ++ipad) {
          for(int irow = -2; irow < 3; ++irow) {
            const int currentRow = row + irow;
            const int currentGlobalRow = globalRow + irow;
            if(currentGlobalRow < 0) continue;
            const int currentPad = cPad + ipad + mapper.getNumberOfPadsInRowSector(currentGlobalRow)/2;

            PadPos padPos;
            CRU currentCRU;
            /// check whether we are still in the same CRU
            /// This procedure is not yet optimized and should go to the Mapper soon
            if(currentRow < nRowsInCRU && currentRow >= 0) {
              /// yes, everything remains the same
              padPos = PadPos(currentRow, currentPad);
              currentCRU = cru;
            }
            else if(currentRow >= nRowsInCRU) {
              /// no, moved to the next CRU
              const int rowInNewCRU = currentRow - nRowsInCRU;
              padPos = PadPos(rowInNewCRU, currentPad);
              currentCRU = CRU(cru.number()+1);
            }
            else {
              /// no, moved to the previous CRU
              const int rowInNewCRU = currentRow + mapper.getNumberOfRowsRegion(cru.number()-1);
              padPos = PadPos(rowInNewCRU, currentPad);
              currentCRU = CRU(cru.number()-1);
            }
            if(currentPad >= mapper.getNumberOfPadsInRowROC(currentCRU.roc(), irow)) continue;
            std::cout << "CRU " << currentCRU.number() << " row " << currentRow << " globalRow " << currentGlobalRow << " nPads " << mapper.getNumberOfPadsInRowROC(currentCRU.roc(), irow) << " pad " << currentPad << " ";
            DigitPos digiPos(currentCRU, padPos);
            if (!digiPos.isValid()) continue;
            const int padOut = digiPos.getPadPos().getPad();
            const int rowOut = digiPos.getPadPos().getRow();

            const float normalizedPadResponse = padResponse.getPadResponse(posEleDiff, digiPadPos, digiPos);
            if (normalizedPadResponse <= 0) continue;

            const float ADCsignal = SAMPAProcessing::getADCvalue(nElectronsGEM * normalizedPadResponse);
            SAMPAProcessing::getShapedSignal(ADCsignal, absoluteTime, signalArray);
            for(float i=0; i<nShapedPoints; ++i) {
              const float time = absoluteTime + i * eleParam.getZBinWidth();
              mDigitContainer->addDigit(MCTrackID, currentCRU.number(), getTimeBinFromTime(time), padOut, rowOut, signalArray[i]);
            }
          }
        }
      }
      else {
        /// No application of the Pad Response Function
        /// Just apply the shaping
        const float ADCsignal = SAMPAProcessing::getADCvalue(nElectronsGEM);
        SAMPAProcessing::getShapedSignal(ADCsignal, absoluteTime, signalArray);
        for(float i=0; i<nShapedPoints; ++i) {
          const float time = absoluteTime + i * eleParam.getZBinWidth();
          mDigitContainer->addDigit(MCTrackID, cru.number(), getTimeBinFromTime(time), row, pad, signalArray[i]);
        }
      }
    }
    /// end of loop over electrons
    ++hitCounter;
#ifdef TPC_GROUPED_HITS
    }
#endif
  }
  /// end of loop over points

  return mDigitContainer;
}
