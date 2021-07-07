// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FemtoDreamCutculator.h
/// \brief FemtoDreamCutculator - small class to match bit-wise encoding and
/// actual physics cuts
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCUTCULATOR_H_
#define ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCUTCULATOR_H_

#include "FemtoDreamSelection.h"
#include "FemtoDreamTrackSelection.h"

#include <bitset>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace o2::analysis::femtoDream
{

/// \class FemtoDreamMath
/// \brief Container for math calculations of quantities related to pairs

class FemtoDreamCutculator
{
 public:
  void init(const char* configFile)
  {
    std::cout << "Welcome to the CutCulator!\n";

    boost::property_tree::ptree root;
    try {
      boost::property_tree::read_json(configFile, root);
    } catch (const boost::property_tree::ptree_error& e) {
      LOG(FATAL) << "Failed to read JSON config file " << configFile << " (" << e.what() << ")";
    }
    mConfigTree = root.get_child("femto-dream-producer-task");
  };

  std::vector<float> setSelection(std::string name)
  {
    boost::property_tree::ptree& signSelections = mConfigTree.get_child(name);
    boost::property_tree::ptree& signSelectionsValue = signSelections.get_child("values");

    std::vector<float> tmpVec;
    for (boost::property_tree::ptree::value_type& val : signSelectionsValue) {
      tmpVec.push_back(std::stof(val.second.data()));
    }
    return tmpVec;
  }

  void setTrackSelection(femtoDreamTrackSelection::TrackSel obs, femtoDreamSelection::SelectionType type, const char* prefix)
  {
    auto tmpVec = setSelection(FemtoDreamTrackSelection::getSelectionName(obs, prefix));
    mTrackSel.setSelection(tmpVec, obs, type);
  }

  template <typename T1, typename T2>
  size_t checkForSelection(T1 sel, T2 selType)
  {
    std::cout << "Selection: " << sel.getSelectionHelper(selType) << " - (";
    auto selVec = sel.getSelections(selType);
    for (auto selIt : selVec) {
      std::cout << selIt << " ";
    }
    std::cout << ")\n > ";
    std::string in;
    std::cin >> in;
    for (size_t i = 0; i < selVec.size(); ++i) {
      if (selVec.at(i) == std::stof(in)) {
        return i;
      }
    }
    std::cout << "Choice " << in << " not recognized - repeating\n";
    checkForSelection(sel, selType);
    return -1;
  }

  template <int nBins, typename T2>
  std::bitset<nBins> iterateSelection(T2 sel)
  {
    std::bitset<nBins> bit = 0;
    size_t counter = 0;
    auto selTypes = sel.getSelectionTypes();
    for (auto selIt : selTypes) {
      size_t useSel = checkForSelection(sel, selIt);
      bit.set(counter + useSel, 1);
      counter += sel.getNSelections(selIt);
    }
    return bit;
  }

  template <int nBins>
  void analyseCuts()
  {
    std::cout << "Do you want to work with tracks/v0/cascade (T/V/C)?\n";
    std::cout << " > ";
    std::string in;
    std::cin >> in;
    std::bitset<nBins> output = -1;
    if (in.compare("T") == 0) {
      output = iterateSelection<nBins>(mTrackSel);
    } else if (in.compare("V") == 0) {
      //output=  iterateSelection<nBins>(mV0Sel);
    } else if (in.compare("C") == 0) {
      //output =  iterateSelection<nBins>(mCascadeSel);
    } else {
      std::cout << "Option " << in << " not recognized - available options are (T/V/C) \n";
      analyseCuts<nBins>();
    }
    std::cout << "+++++++++++++++++++++++++++++++++\n";
    std::cout << "CutCulator has spoken - your selection bit is\n";
    std::cout << output << " (bitwise)\n";
    std::cout << output.to_ullong() << " (number representation)\n";
  }

 private:
  boost::property_tree::ptree mConfigTree;
  FemtoDreamTrackSelection mTrackSel;
};
} // namespace o2::analysis::femtoDream

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCUTCULATOR_H_ */
