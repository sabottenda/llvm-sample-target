//===-- SampleTargetInfo.cpp - Sample Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "Sample.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheSampleTarget;

extern "C" void LLVMInitializeSampleTargetInfo() { 
  DEBUG(dbgs() << ">> InitSampleTargetInfo <<\n");
  RegisterTarget<Triple::sample, /*HasJIT=*/false>
    X(TheSampleTarget, "sample", "Sample");
}
