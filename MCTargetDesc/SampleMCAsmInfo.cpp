//===-- SampleMCAsmInfo.cpp - Sample asm properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the SampleMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "SampleMCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

SampleMCAsmInfo::SampleMCAsmInfo(const Target &T, StringRef TT) {
  PointerSize = 4;

  PrivateGlobalPrefix = ".L";
  //WeakRefDirective ="\t.weak\t";
  PCSymbol=".";
  CommentString = ";";

  AlignmentIsInBytes = false;
  AllowNameToStartWithDigit = true;
  UsesELFSectionDirectiveForBSS = true;
}
