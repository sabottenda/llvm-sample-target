//===-- SampleFrameLowering.cpp - Sample Frame Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Sample implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "SampleFrameLowering.h"
#include "SampleInstrInfo.h"
#include "SampleMachineFunction.h"
#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

bool SampleFrameLowering::hasFP(const MachineFunction &MF) const {
  return false;
}

void SampleFrameLowering::emitPrologue(MachineFunction &MF) const {
  DEBUG(dbgs() << ">> SampleFrameLowering::emitPrologue <<\n");
  DEBUG(dbgs() << ">>> Not implemented yet\n");
}
void SampleFrameLowering::emitEpilogue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
  DEBUG(dbgs() << ">> SampleFrameLowering::emitEpilogue <<\n");
}
