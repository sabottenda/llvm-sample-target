//===-- SampleRegisterInfo.cpp - Sample Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the Sample implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sample-reg-info"

#include "SampleRegisterInfo.h"
#include "Sample.h"
#include "llvm/Constants.h"
#include "llvm/Type.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/DebugInfo.h"

#include "MCTargetDesc/SampleMCTargetDesc.h"

#define GET_REGINFO_TARGET_DESC
#include "SampleGenRegisterInfo.inc"

using namespace llvm;

SampleRegisterInfo::SampleRegisterInfo(const TargetInstrInfo &tii)
  : SampleGenRegisterInfo(Sample::RA), TII(tii) { }

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//

/// Sample Callee Saved Registers
const uint16_t* SampleRegisterInfo::
getCalleeSavedRegs(const MachineFunction *MF) const
{
    return CSR_SingleFloatOnly_SaveList;
}

const uint32_t*
SampleRegisterInfo::getCallPreservedMask(CallingConv::ID) const
{  
    return CSR_SingleFloatOnly_RegMask;
}

BitVector SampleRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
  static const uint16_t ReservedCPURegs[] = {
      Sample::ZERO, Sample::SP, Sample::RA,
  };

  BitVector Reserved(getNumRegs());
  typedef TargetRegisterClass::iterator RegIter;

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

// This function eliminate ADJCALLSTACKDOWN,
// ADJCALLSTACKUP pseudo instructions
void SampleRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const {
  DEBUG(dbgs() << ">> SampleRegisterInfo::eliminateCallFramePseudoInstr <<\n";);
  llvm_unreachable("not implemented");
}

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void SampleRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    RegScavenger *RS) const {
  DEBUG(dbgs() << ">> SampleRegisterInfo::eliminateFrameIndex <<\n";);

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineFrameInfo *MFI = MF.getFrameInfo();

  unsigned i = 0;
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() &&
           "Instr doesn't have FrameIndex operand!");
  }

  DEBUG(errs() << "\nFunction : " << MF.getFunction()->getName() << "\n";
        errs() << "<--------->\n" << MI);

  int FrameIndex = MI.getOperand(i).getIndex();
  uint64_t stackSize = MF.getFrameInfo()->getStackSize();
  int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  int64_t Offset = spOffset + stackSize + MI.getOperand(i+1).getImm();
  unsigned FrameReg = Sample::SP;

  DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
               << "spOffset   : " << spOffset << "\n"
               << "stackSize  : " << stackSize << "\n"
               << "Offset     : " << Offset << "\n" << "<--------->\n");

  DEBUG(errs() << "Before:" << MI);
  MI.getOperand(i).ChangeToRegister(FrameReg, false);
  MI.getOperand(i+1).ChangeToImmediate(Offset);
  DEBUG(errs() << "After:" << MI);
}

unsigned SampleRegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
    return Sample::SP;
}
