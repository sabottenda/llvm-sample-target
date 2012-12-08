//===-- Sample.h - Top-level interface for Sample representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Sample back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_SAMPLE_H
#define TARGET_SAMPLE_H

#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

namespace llvm {
  class SampleTargetMachine;
  class FunctionPass;

  FunctionPass *createSampleISelDag(SampleTargetMachine &TM);
} // end namespace llvm;

#endif
