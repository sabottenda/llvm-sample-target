//===-- SampleMCInstLower.h - Lower MachineInstr to MCInst ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef SAMPLE_MCINSTLOWER_H
#define SAMPLE_MCINSTLOWER_H

#include "llvm/Support/Compiler.h"
#include "llvm/CodeGen/MachineOperand.h"

namespace llvm {
  class AsmPrinter;
  class MCContext;
  class MCInst;
  class MCOperand;
  class MCSymbol;
  class MachineInstr;
  class MachineModuleInfoMachO;
  class MachineOperand;
  class Mangler;

  /// SampleMCInstLower - This class is used to lower an MachineInstr
  /// into an MCInst.
class LLVM_LIBRARY_VISIBILITY SampleMCInstLower {
  typedef MachineOperand::MachineOperandType MachineOperandType;
  MCContext &Ctx;
  Mangler &Mang;
  const AsmPrinter &Printer;

 public:
  SampleMCInstLower(MCContext &ctx, Mangler &mang, AsmPrinter &printer)
    : Ctx(ctx), Mang(mang), Printer(printer) {}
  void Lower(const MachineInstr *MI, MCInst &OutMI) const;

 private:
  MCOperand LowerOperand(const MachineOperand& MO) const;
  MCOperand LowerSymbolOperand(const MachineOperand &MO,
                               MachineOperandType MOTy) const;
};
} // end of namespace llvm

#endif
