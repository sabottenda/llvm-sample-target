//===-- SampleSelectionDAGInfo.cpp - Sample SelectionDAG Info -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SampleSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sample-selectiondag-info"
#include "SampleTargetMachine.h"
using namespace llvm;

SampleSelectionDAGInfo::SampleSelectionDAGInfo(const SampleTargetMachine &TM)
    : TargetSelectionDAGInfo(TM) {}

SampleSelectionDAGInfo::~SampleSelectionDAGInfo() {}
