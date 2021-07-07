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
    try {
      boost::property_tree::ptree& signSelections = mConfigTree.get_child(name);
      boost::property_tree::ptree& signSelectionsValue = signSelections.get_child("values");
      std::vector<float> tmpVec;
      for (boost::property_tree::ptree::value_type& val : signSelectionsValue) {
        tmpVec.push_back(std::stof(val.second.data()));
      }
      return tmpVec;
    } catch (const boost::property_tree::ptree_error& e) {
      LOG(WARNING) << "Selection " << name << " not available (" << e.what() << ")";
      return {};
    }
  }

  void setTrackSelection(femtoDreamTrackSelection::TrackSel obs, femtoDreamSelection::SelectionType type, const char* prefix)
  {
    auto tmpVec = setSelection(FemtoDreamTrackSelection::getSelectionName(obs, prefix));
    if (tmpVec.size() > 0) {
      mTrackSel.setSelection(tmpVec, obs, type);
    }
  }

  template <typename T1, typename T2, typename T3>
  void checkForSelection(T1& output, size_t& counter, T2 objectSelection, T3 selectionType)
  {
    std::cout << "Selection: " << objectSelection.getSelectionHelper(selectionType) << " - (";
    auto selVec = objectSelection.getSelections(selectionType);
    for (auto selIt : selVec) {
      std::cout << selIt.getSelectionValue() << " ";
    }
    std::cout << ")\n > ";
    std::string in;
    std::cin >> in;
    const float input = std::stof(in);
    bool inputSane = false;
    for (auto sel : selVec) {
      if (std::abs(sel.getSelectionValue() - input) < std::abs(1.e-6 * input)) {
        inputSane = true;
      }
    }

    if (inputSane) {
      for (auto sel : selVec) {
        if (sel.getSelectionType() == femtoDreamSelection::SelectionType::kEqual) {
          if (sel.isSelected(input)) {
            output |= 1UL << counter;
          }
        } else {
          if (!sel.isSelected(input)) {
            output |= 1UL << counter;
          }
        }
        ++counter;
      }
    } else {
      std::cout << "Choice " << in << " not recognized - repeating\n";
      checkForSelection<T1>(output, counter, objectSelection, selectionType);
    }
  }

  template <typename T1, typename T2>
  T1 iterateSelection(T2 objectSelection)
  {
    T1 output = 0;
    size_t counter = 0;
    auto selectionTypes = objectSelection.getSelectionTypes();
    for (auto selIt : selectionTypes) {
      checkForSelection<T1>(output, counter, objectSelection, selIt);
    }
    return output;
  }

  template <typename T>
  void analyseCuts()
  {
    std::cout << "Do you want to work with tracks/v0/cascade (T/V/C)?\n";
    std::cout << " > ";
    std::string in;
    std::cin >> in;
    T output = -1;
    if (in.compare("T") == 0) {
      output = iterateSelection<T>(mTrackSel);
    } else if (in.compare("V") == 0) {
      //output=  iterateSelection<nBins>(mV0Sel);
    } else if (in.compare("C") == 0) {
      //output =  iterateSelection<nBins>(mCascadeSel);
    } else {
      std::cout << "Option " << in << " not recognized - available options are (T/V/C) \n";
      analyseCuts<T>();
    }
    std::bitset<8 * sizeof(T)> bitOutput = output;
    std::cout << "+++++++++++++++++++++++++++++++++\n";
    std::cout << "CutCulator has spoken - your selection bit is\n";
    std::cout << bitOutput << " (bitwise)\n";
    std::cout << output << " (number representation)\n";
  }

 private:
  boost::property_tree::ptree mConfigTree;
  FemtoDreamTrackSelection mTrackSel;
};
} // namespace o2::analysis::femtoDream

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCUTCULATOR_H_ */
