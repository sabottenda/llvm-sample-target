//===-- SampleMachineFunctionInfo.h - Private data used for Sample ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the Sample specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_MACHINE_FUNCTION_INFO_H
#define SAMPLE_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include <utility>

namespace llvm {

/// SampleFunctionInfo - This class is derived from MachineFunction private
/// Sample target-specific information for each MachineFunction.
class SampleMachineFunctionInfo : public MachineFunctionInfo {
  virtual void anchor();

public:
  SampleMachineFunctionInfo(MachineFunction& MF) {}
};
} // end of namespace llvm

#endif // SAMPLE_MACHINE_FUNCTION_INFO_H
