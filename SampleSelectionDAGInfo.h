//===-- SampleSelectionDAGInfo.h - Sample SelectionDAG Info ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the Sample subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_SELECTIONDAGINFO_H
#define SAMPLE_SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class SampleTargetMachine;

class SampleSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit SampleSelectionDAGInfo(const SampleTargetMachine &TM);
  ~SampleSelectionDAGInfo();
};
} // end of namespace llvm

#endif
