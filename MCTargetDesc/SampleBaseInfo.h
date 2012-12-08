//===-- SampleBaseInfo.h - Top level definitions for MIPS MC ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the Sample target useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef SAMPLEBASEINFO_H
#define SAMPLEASEINFO_H

//#include "SampleFixupKinds.h"
#include "SampleMCTargetDesc.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// getSampleRegisterNumbering - Given the enum value for some register,
/// return the number that it corresponds to.
inline static unsigned getSampleRegisterNumbering(unsigned RegEnum)
{
  switch (RegEnum) {
  case Sample::ZERO:
    return 0;
  case Sample::V0:
    return 1;
  case Sample::A0:
    return 2;
  case Sample::A1:
    return 3;
  case Sample::A2:
    return 4;
  case Sample::A3:
    return 5;
  case Sample::T0:
    return 6;
  case Sample::T1:
    return 7;
  case Sample::T2:
    return 8;
  case Sample::T3:
    return 9;
  case Sample::S0:
    return 10;
  case Sample::S1:
    return 11;
  case Sample::S2:
    return 12;
  case Sample::S3:
    return 13;
  case Sample::SP:
    return 14;
  case Sample::RA:
    return 15;
  default: llvm_unreachable("Unknown register number!");
  }
}
}

#endif
