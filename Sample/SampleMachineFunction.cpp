//===-- SampleMachineFunctionInfo.cpp - Private data used for Sample ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SampleMachineFunction.h"
#include "SampleInstrInfo.h"
#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void SampleMachineFunctionInfo::anchor() { }
