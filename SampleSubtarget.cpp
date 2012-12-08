//===-- SampleSubtarget.cpp - Sample Subtarget Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Sample specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "SampleSubtarget.h"
#include "Sample.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "SampleGenSubtargetInfo.inc"

using namespace llvm;

SampleSubtarget::SampleSubtarget(const std::string &TT,
                                 const std::string &CPU,
                                 const std::string &FS)
    : SampleGenSubtargetInfo(TT, CPU, FS) {
  std::string CPUName = "generic";

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
}
