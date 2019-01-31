// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file testTPCParameters.cxx
/// \brief This task tests the Parameter handling
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#define BOOST_TEST_MODULE Test TPC Parameters
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "TPCBase/ParameterDetector.h"
#include "TPCBase/ParameterElectronics.h"
#include "TPCBase/ParameterGEM.h"
#include "TPCBase/ParameterGas.h"
#include <SimConfig/ConfigurableParam.h>
#include <SimConfig/ConfigurableParamHelper.h>
#include <SimConfig/SimConfig.h>
namespace o2
{
namespace TPC
{

/// \brief Trivial test of the default initialization of Parameter Detector
/// Precision: 1E-3 %
BOOST_AUTO_TEST_CASE(ParameterDetector_test1) {
  o2::conf::ConfigurableParam::printAllKeyValuePairs();

	BOOST_CHECK_CLOSE(ParameterDetector::Instance().getPadCapacitance(), 0.1f,
			1E-3);
	BOOST_CHECK_CLOSE(ParameterDetector::Instance().getTPClength(), 250.f,
			1E-3);
	BOOST_CHECK_CLOSE(ParameterDetector::Instance().getMaxTimeBinTriggered(),
			550.f, 1E-12);

	BOOST_CHECK_CLOSE(ParameterDetector::Instance().getPadCapacitance(),
			o2::conf::ConfigurableParam::getValueAs<float>(
					"TPCDetParam.PadCapacitance"), 1E-3);
	BOOST_CHECK_CLOSE(ParameterDetector::Instance().getTPClength(),
			o2::conf::ConfigurableParam::getValueAs<float>(
					"TPCDetParam.TPClength"), 1E-3);
	BOOST_CHECK(
			ParameterDetector::Instance().getMaxTimeBinTriggered()
					== o2::conf::ConfigurableParam::getValueAs<TimeBin>(
							"TPCDetParam.TmaxTriggered"));
}

/// \brief Trivial test of the initialization of Parameter Detector
/// Precision: 1E-12 %
BOOST_AUTO_TEST_CASE(ParameterDetector_test2)
{
  // this leads to a crash (Inexistant ConfigurableParam key: TPCDetParam.PadCapacitance )
//  o2::conf::ConfigurableParam::updateFromString(
//      "TPCDetParam.PadCapacitance = 1.f;TPCDetParam.TPClength = 2.f;TPCDetParam.TmaxTriggered = 3.f");

  o2::conf::ConfigurableParam::setValue<float>("TPCDetParam", "PadCapacitance", 1.f);
  o2::conf::ConfigurableParam::setValue<float>("TPCDetParam", "TPClength", 2.f);
  o2::conf::ConfigurableParam::setValue<float>("TPCDetParam", "TmaxTriggered", 3.f);

  // here the value seems to be not updated
  o2::conf::ConfigurableParam::printAllKeyValuePairs();

  // Check the access via ConfigParam - this works nevertheless
  BOOST_CHECK_CLOSE(o2::conf::ConfigurableParam::getValueAs<float>("TPCDetParam.PadCapacitance"), 1.f, 1E-12);
  BOOST_CHECK_CLOSE(o2::conf::ConfigurableParam::getValueAs<float>("TPCDetParam.TPClength"), 2.f, 1E-12);
  BOOST_CHECK_CLOSE(o2::conf::ConfigurableParam::getValueAs<float>("TPCDetParam.TmaxTriggered"), 3.f, 1E-12);

  // Check the access via the Parameter class - this does not work!
//  BOOST_CHECK_CLOSE(ParameterDetector::Instance().getPadCapacitance(), 1.f, 1E-12);
//  BOOST_CHECK_CLOSE(ParameterDetector::Instance().getTPClength(), 2.f, 1E-12);
//  BOOST_CHECK_CLOSE(ParameterDetector::Instance().getMaxTimeBinTriggered(), 3.f, 1E-12);
}

///// \brief Trivial test of the default initialization of Parameter Electronics
///// Precision: 1E-3 %
//BOOST_AUTO_TEST_CASE(ParameterElectronics_test1)
//{
//  auto& eleParam = ParameterElectronics::Instance();
//  BOOST_CHECK(eleParam.getNShapedPoints() == 8);
//  BOOST_CHECK_CLOSE(eleParam.getPeakingTime(), 160e-3, 1e-3);
//  BOOST_CHECK_CLOSE(eleParam.getChipGain(), 20, 1e-3);
//  BOOST_CHECK_CLOSE(eleParam.getADCDynamicRange(), 2200, 1e-3);
//  BOOST_CHECK_CLOSE(eleParam.getADCSaturation(), 1024, 1e-3);
//  BOOST_CHECK_CLOSE(eleParam.getZBinWidth(), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(eleParam.getElectronCharge(), 1.602e-19, 1e-3);
//}
//
///// \brief Trivial test of the initialization of Parameter Detector
///// Precision: 1E-12 %
//BOOST_AUTO_TEST_CASE(ParameterElectronics_test2)
//{
//  auto& eleParam = ParameterElectronics::Instance();
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.NShapedPoints = 1");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.PeakingTime = 2");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.ChipGain = 3");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.ADCdynamicRange = 4");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.ADCsaturation = 5");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.ZbinWidth = 6");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.ElectronCharge = 7");
////  o2::conf::ConfigurableParam::setValue("TPCEleParam.Digitization = 0");
//  BOOST_CHECK(eleParam.getNShapedPoints() == 1);
//  BOOST_CHECK_CLOSE(eleParam.getPeakingTime(), 2.f, 1e-12);
//  BOOST_CHECK_CLOSE(eleParam.getChipGain(), 3.f, 1e-12);
//  BOOST_CHECK_CLOSE(eleParam.getADCDynamicRange(), 4.f, 1e-12);
//  BOOST_CHECK_CLOSE(eleParam.getADCSaturation(), 5.f, 1e-12);
//  BOOST_CHECK_CLOSE(eleParam.getZBinWidth(), 6.f, 1e-12);
//  BOOST_CHECK_CLOSE(eleParam.getElectronCharge(), 7.f, 1e-12);
//  BOOST_CHECK(eleParam.getDigitizationMode() == DigitzationMode::FullMode);
//}
//
///// \brief Trivial test of the default initialization of Parameter Gas
///// Precision: 1E-3 %
//BOOST_AUTO_TEST_CASE(ParameterGas_test1)
//{
//  auto& gasParam = ParameterGas::Instance();
//  BOOST_CHECK_CLOSE(gasParam.getWion(), 37.3e-9, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getIpot(), 20.77e-9, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getAttachmentCoefficient(), 250.f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getOxygenContent(), 5e-6, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getVdrift(), 2.58f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getSigmaOverMu(), 0.78f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getDiffT(), 0.0209f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getDiffL(), 0.0221f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getNprim(), 14.f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getScaleG4(), 0.85f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getFanoFactorG4(), 0.7f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(0), 0.76176e-1, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(1), 10.632, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(2), 0.13279e-4, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(3), 1.8631, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(4), 1.9479, 1e-3);
//}
//
///// \brief Trivial test of the initialization of Parameter Gas
///// Precision: 1E-12 %
//BOOST_AUTO_TEST_CASE(ParameterGas_test2)
//{
//  auto& gasParam = ParameterGas::Instance();
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.Wion = 1");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.Ipot = 2");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.AttCoeff = 3");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.OxyCont = 4");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.DriftV = 5");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.SigmaOverMu = 6");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.DiffT = 7");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.DiffL = 8");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.Nprim = 9");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.ScaleFactorG4 = 10");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.FanoFactorG4 = 11");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.BetheBlochParam[0] = 12");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.BetheBlochParam[1] = 13");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.BetheBlochParam[2] = 14");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.BetheBlochParam[3] = 15");
////  o2::conf::ConfigurableParam::setValue("TPCGasParam.BetheBlochParam[4] = 16");
//  BOOST_CHECK_CLOSE(gasParam.getWion(), 1.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getIpot(), 2.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getAttachmentCoefficient(), 3.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getOxygenContent(), 4.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getVdrift(), 5.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getSigmaOverMu(), 6.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getDiffT(), 7.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getDiffL(), 8.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getNprim(), 9.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getScaleG4(), 10.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getFanoFactorG4(), 11.f, 1e-3);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(0), 12.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(1), 13.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(2), 14.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(3), 15.f, 1e-12);
//  BOOST_CHECK_CLOSE(gasParam.getBetheBlochParam(4), 16.f, 1e-12);
//}
//
///// \brief Trivial test of the default initialization of Parameter GEM
///// Precision: 1E-3 %
/////
//BOOST_AUTO_TEST_CASE(ParameterGEM_test1)
//{
//  auto& gemParam = ParameterGEM::Instance();
//  BOOST_CHECK(gemParam.getGeometry(1) == 0);
//  BOOST_CHECK(gemParam.getGeometry(2) == 2);
//  BOOST_CHECK(gemParam.getGeometry(3) == 2);
//  BOOST_CHECK(gemParam.getGeometry(4) == 0);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(1), 4.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(2), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(3), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(4), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(5), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(1), 270.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(2), 250.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(3), 270.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(4), 340.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(1), 0.4f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(2), 4.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(3), 2.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(4), 0.1f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(5), 4.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(1), 14.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(2), 8.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(3), 53.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(4), 240.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getTotalGainStack(), 1644.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getKappaStack(), 1.2295f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getEfficiencyStack(), 0.473805f, 1e-3);
//  BOOST_CHECK(gemParam.getAmplificationMode() == AmplificationMode::EffectiveMode);
//
//  // For fixed values
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(1), 1.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(2), 0.2f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(3), 0.25f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(4), 1.f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(1), 0.65f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(2), 0.55f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(3), 0.12f, 1e-3);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(4), 0.6f, 1e-3);
//
//  for (int i = 1; i < 5; ++i) {
//    BOOST_CHECK_CLOSE(
//      gemParam.getEffectiveGain(i),
//      gemParam.getAbsoluteGain(i) * gemParam.getCollectionEfficiency(i) * gemParam.getExtractionEfficiency(i), 1e-3);
//  }
//}
//
///// \brief Trivial test of the initialization of Parameter GEM
///// Precision: 1E-12 %
//BOOST_AUTO_TEST_CASE(ParameterGEM_test2)
//{
//  auto& gemParam = ParameterGEM::Instance();
//  o2::conf::ConfigurableParam::setValue("TPCGEMParam", "Geometry[0]", 0);
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Geometry[1] = 2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Geometry[2] = 2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Geometry[3] = 0");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Distance[0] = 4");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Distance[1] = 0.2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Distance[2] = 0.2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Distance[3] = 0.2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Distance[4] = 0.2");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Potential[0] = 4");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Potential[1] = 5");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Potential[2] = 6");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Potential[3] = 7");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ElectricField[0] = 8");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ElectricField[1] = 9");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ElectricField[2] = 10");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ElectricField[3] = 11");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ElectricField[4] = 12");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.AbsoluteGain[0] = 13");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.AbsoluteGain[1] = 14");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.AbsoluteGain[2] = 15");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.AbsoluteGain[3] = 16");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.CollectionEfficiency[0] = 17");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.CollectionEfficiency[1] = 18");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.CollectionEfficiency[2] = 19");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.CollectionEfficiency[3] = 20");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ExtractionEfficiency[0] = 21");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ExtractionEfficiency[1] = 22");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ExtractionEfficiency[2] = 23");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.ExtractionEfficiency[3] = 24");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.TotalGainStack = 25");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.KappaStack = 26");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.EfficiencyStack = 27");
////  o2::conf::ConfigurableParam::setValue("TPCGEMParam.Amplification = 1");
//  BOOST_CHECK(gemParam.getGeometry(1) == 0);
//  BOOST_CHECK(gemParam.getGeometry(2) == 2);
//  BOOST_CHECK(gemParam.getGeometry(3) == 2);
//  BOOST_CHECK(gemParam.getGeometry(4) == 0);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(1), 4.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(2), 0.2f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(3), 0.2f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(4), 0.2f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getDistance(5), 0.2f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(1), 4.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(2), 5.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(3), 6.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getPotential(4), 7.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(1), 8.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(2), 9.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(3), 10.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(4), 11.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getElectricField(5), 12.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(1), 13.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(2), 14.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(3), 15.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getAbsoluteGain(4), 16.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(1), 17.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(2), 18.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(3), 19.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getCollectionEfficiency(4), 20.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(1), 21.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(2), 22.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(3), 23.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getExtractionEfficiency(4), 24.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getTotalGainStack(), 25.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getKappaStack(), 26.f, 1e-12);
//  BOOST_CHECK_CLOSE(gemParam.getEfficiencyStack(), 27., 1e-3);
//  BOOST_CHECK(gemParam.getAmplificationMode() == AmplificationMode::EffectiveMode);
//}
} // namespace TPC
} // namespace o2
