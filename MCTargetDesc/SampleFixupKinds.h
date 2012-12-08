//===-- SampleFixupKinds.h - Sample Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SAMPLE_FIXUPKINDS_H
#define LLVM_SAMPLE_FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Sample {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.
  //
  // This table *must* be in the save order of
  // MCFixupKindInfo Infos[Sample::NumTargetFixupKinds]
  // in SampleAsmBackend.cpp.
  //
  enum Fixups {
    fixup_Sample_24 = FirstTargetFixupKind,
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
} // namespace Sample
} // namespace llvm


#endif // LLVM_SAMPLE_SAMPLEFIXUPKINDS_H
