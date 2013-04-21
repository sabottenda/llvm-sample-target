//===-- SampleFrameLowering.h - Define frame lowering for Sample ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_FRAMELOWERING_H
#define SAMPLE_FRAMELOWERING_H

#include "Sample.h"
#include "SampleSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class SampleSubtarget;

class SampleFrameLowering : public TargetFrameLowering {
 protected:
  const SampleSubtarget &STI;

 public:
  explicit SampleFrameLowering(const SampleSubtarget &sti)
      : TargetFrameLowering(StackGrowsDown, 8, 0), STI(sti) {
  }

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const /*override*/;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const /*override*/;
  bool hasFP(const MachineFunction &MF) const /*override*/;
};
} // End llvm namespace

#endif
