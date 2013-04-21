//===-- SampleELFObjectWriter.cpp - Sample ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/SampleBaseInfo.h"
#include "MCTargetDesc/SampleFixupKinds.h"
#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <list>

using namespace llvm;

namespace {
struct RelEntry {
  RelEntry(const ELFRelocationEntry &R, const MCSymbol *S, int64_t O) :
      Reloc(R), Sym(S), Offset(O) {}
  ELFRelocationEntry Reloc;
  const MCSymbol *Sym;
  int64_t Offset;
};

typedef std::list<RelEntry> RelLs;
typedef RelLs::iterator RelLsIter;

class SampleELFObjectWriter : public MCELFObjectTargetWriter {
 public:
  SampleELFObjectWriter(uint8_t OSABI);
  virtual ~SampleELFObjectWriter();

  // オブジェクトを生成するときやリンク時にアドレス解決するために
  // ELFObjectWriterなどから参照される
  virtual unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                                bool IsPCRel, bool IsRelocWithSymbol,
                                int64_t Addend) const;
};
}

SampleELFObjectWriter::
SampleELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*_is64Bit*/ false, OSABI, ELF::EM_NONE,
                              /*HasRelocationAddend*/ false) {}

SampleELFObjectWriter::~SampleELFObjectWriter() {}

unsigned SampleELFObjectWriter::
GetRelocType(const MCValue &Target,
             const MCFixup &Fixup,
             bool IsPCRel,
             bool IsRelocWithSymbol,
             int64_t Addend) const {
  // determine the type of the relocation
  unsigned Type = (unsigned)ELF::R_MIPS_NONE;
  unsigned Kind = (unsigned)Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case Sample::fixup_Sample_24:
    Type = ELF::R_MIPS_26;
    break;
  }

  return Type;
}

MCObjectWriter *llvm::createSampleELFObjectWriter(raw_ostream &OS,
                                                  uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new SampleELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*isLittleEndian*/ true);
}
