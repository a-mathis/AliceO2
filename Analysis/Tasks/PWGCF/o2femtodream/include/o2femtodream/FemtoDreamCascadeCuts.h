// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file FemtoDreamCascadeCuts.h
/// \brief Definition of the FemtoDreamCascadeCuts
/// \author Andi Mathis, TU München, andreas.mathis@ph.tum.de

#ifndef ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCASCADECUTS_H_
#define ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCASCADECUTS_H_

#include <Rtypes.h>
#include <string>

namespace o2::analysis
{
namespace femtoDream
{

class FemtoDreamCascadeCuts
{
 public:
  static std::string getCutHelp();

 private:
  ClassDefNV(FemtoDreamCascadeCuts, 1);
};

} /* namespace femtoDream */
} /* namespace o2::analysis */

#endif /* ANALYSIS_TASKS_PWGCF_FEMTODREAM_FEMTODREAMCASCADECUTS_H_ */
