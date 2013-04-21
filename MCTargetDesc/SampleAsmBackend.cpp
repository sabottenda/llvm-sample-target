//===-- SampleASMBackend.cpp - Sample Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the SampleAsmBackend and SampleELFObjectWriter classes.
//
//===----------------------------------------------------------------------===//
//

#include "SampleFixupKinds.h"
#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Debug.h" 
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

using namespace llvm;

// Prepare value for the target space for it
static unsigned adjustFixupValue(unsigned Kind, uint64_t Value) {
  DEBUG(dbgs() << ">> adjustFixupValue: kind:" << Kind << " Value:" << Value << "\n");

  switch (Kind) {
  default:
    return 0;
  case Sample::fixup_Sample_24:
    Value >>= 2;
    break;
  }

  return Value;
}

namespace {
class SampleAsmBackend : public MCAsmBackend {
  Triple::OSType OSType;

public:
  SampleAsmBackend(const Target &T,  Triple::OSType _OSType)
    :MCAsmBackend(), OSType(_OSType) {}

  MCObjectWriter *createObjectWriter(raw_ostream &OS) const {
    return createSampleELFObjectWriter(OS, OSType);
  }

  /// ApplyFixup - Apply the \arg Value for given \arg Fixup into the provided
  /// data fragment, at the offset specified by the fixup and following the
  /// fixup kind as appropriate.
  void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
                  uint64_t Value) const;

  unsigned getNumFixupKinds() const { return Sample::NumTargetFixupKinds; }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const;

  /// @name Target Relaxation Interfaces
  /// @{

  /// MayNeedRelaxation - Check whether the given instruction may need
  /// relaxation.
  ///
  /// \param Inst - The instruction to test.
  bool mayNeedRelaxation(const MCInst &Inst) const {
    return false;
  }

  /// fixupNeedsRelaxation - Target specific predicate for whether a given
  /// fixup requires the associated instruction to be relaxed.
  bool fixupNeedsRelaxation(const MCFixup &Fixup,
                            uint64_t Value,
                            const MCInstFragment *DF,
                            const MCAsmLayout &Layout) const {
    // FIXME.
    assert(0 && "RelaxInstruction() unimplemented");
    return false;
  }

  /// RelaxInstruction - Relax the instruction in the given fragment
  /// to the next wider instruction.
  ///
  /// \param Inst - The instruction to relax, which may be the same
  /// as the output.
  /// \parm Res [output] - On return, the relaxed instruction.
  void relaxInstruction(const MCInst &Inst, MCInst &Res) const {
  }

  /// @}

  /// WriteNopData - Write an (optimal) nop sequence of Count bytes
  /// to the given output. If the target cannot generate such a sequence,
  /// it should return an error.
  ///
  /// \return - True on success.
  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const {
    return true;
  }
}; // class SampleAsmBackend

void SampleAsmBackend::
applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
           uint64_t Value) const {
  MCFixupKind Kind = Fixup.getKind();
  Value = adjustFixupValue((unsigned)Kind, Value);

  if (!Value)
    return; // Doesn't change encoding.

  // Where do we start in the object
  unsigned Offset = Fixup.getOffset();
  // Number of bytes we need to fixup
  unsigned NumBytes = (getFixupKindInfo(Kind).TargetSize + 7) / 8;

  DEBUG(dbgs() << "  Offset: " << Offset << "\n");
  DEBUG(dbgs() << "  NumBytes: " << NumBytes << "\n");
  DEBUG(dbgs() << "  TargetSize: " << getFixupKindInfo(Kind).TargetSize << "\n");
  DEBUG(dbgs() << format("  Data: 0x%08x\n", (uint32_t)Data[Offset]));

  // Grab current value, if any, from bits.
  uint64_t CurVal = 0;

  for (unsigned i = 0; i != NumBytes; ++i) {
    unsigned Idx = i;
    CurVal |= (uint64_t)((uint8_t)Data[Offset + Idx]) << (i*8);
  }
  DEBUG(dbgs() << format("  CurVal: 0x%08x\n", CurVal));

  uint64_t Mask = ((uint64_t)(-1) >> (64 - getFixupKindInfo(Kind).TargetSize));
  CurVal |= Value & Mask;
  DEBUG(dbgs() << format("  CurVal: 0x%08x\n", CurVal));

  // Write out the fixed up bytes back to the code/data bits.
  for (unsigned i = 0; i != NumBytes; ++i) {
    unsigned Idx = i;
    Data[Offset + Idx] = (uint8_t)((CurVal >> (i*8)) & 0xff);
  }

  DEBUG(dbgs() << format("  Data: 0x%08x\n", (uint32_t)Data[Offset]));
}

const MCFixupKindInfo &SampleAsmBackend::
getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[Sample::NumTargetFixupKinds] = {
    // This table *must* be in same the order of fixup_* kinds in
    // SampleFixupKinds.h.
    //
    // name                  offset    bits  flags
    {"fixup_Sample_24",           0,     24,     0}
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

} // namespace

// MCAsmBackend
MCAsmBackend *llvm::createSampleAsmBackend(const Target &T, StringRef TT,
                                           StringRef CPU) {
  return new SampleAsmBackend(T, Triple(TT).getOS());
}
