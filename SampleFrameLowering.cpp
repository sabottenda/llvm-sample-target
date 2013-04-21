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
#include "llvm/DataLayout.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

bool SampleFrameLowering::
hasFP(const MachineFunction &MF) const {
  return false;
}

void SampleFrameLowering::
emitPrologue(MachineFunction &MF) const {
  DEBUG(dbgs() << ">> SampleFrameLowering::emitPrologue <<\n");

  MachineBasicBlock &MBB   = MF.front();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  const SampleInstrInfo &TII =
    *static_cast<const SampleInstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  // allocate fixed size for simplicity
  uint64_t StackSize = 4 * 16;

   // Update stack size
  MFI->setStackSize(StackSize);

  BuildMI(MBB, MBBI, dl, TII.get(Sample::MOVE), Sample::T0)
      .addImm(-StackSize);
  BuildMI(MBB, MBBI, dl, TII.get(Sample::ADD), Sample::SP)
      .addReg(Sample::SP)
      .addReg(Sample::T0);
}

void SampleFrameLowering::
emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
  DEBUG(dbgs() << ">> SampleFrameLowering::emitEpilogue <<\n");

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  const SampleInstrInfo &TII =
    *static_cast<const SampleInstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl = MBBI->getDebugLoc();

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  // Adjust stack.
  BuildMI(MBB, MBBI, dl, TII.get(Sample::MOVE), Sample::T0)
      .addImm(StackSize);
  BuildMI(MBB, MBBI, dl, TII.get(Sample::ADD), Sample::SP)
      .addReg(Sample::SP)
      .addReg(Sample::T0);
}
