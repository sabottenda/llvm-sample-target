//===-- SampleMCTargetDesc.cpp - Sample Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides Sample specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "SampleMCAsmInfo.h"
#include "SampleMCTargetDesc.h"
#include "InstPrinter/SampleInstPrinter.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "SampleGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "SampleGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "SampleGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createSampleMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitSampleMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createSampleMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitSampleMCRegisterInfo(X, Sample::RA);
  return X;
}

static MCSubtargetInfo *createSampleMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                  StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitSampleMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCAsmInfo *createSampleMCAsmInfo(const Target &T, StringRef TT) {
  MCAsmInfo *MAI = new SampleMCAsmInfo(T, TT);

  MachineLocation Dst(MachineLocation::VirtualFP);
  MachineLocation Src(Sample::SP, 0);
  MAI->addInitialFrameState(0, Dst, Src);

  return MAI;
}

static MCCodeGenInfo *createSampleMCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                              CodeModel::Model CM,
                                              CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  X->InitMCCodeGenInfo(RM, CM, OL);
  return X;
}

static MCInstPrinter *createSampleMCInstPrinter(const Target &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI,
                                              const MCSubtargetInfo &STI) {
  return new SampleInstPrinter(MAI, MII, MRI);
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
                                    MCContext &Ctx, MCAsmBackend &MAB,
                                    raw_ostream &_OS,
                                    MCCodeEmitter *_Emitter,
                                    bool RelaxAll,
                                    bool NoExecStack) {
  Triple TheTriple(TT);

  return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

extern "C" void LLVMInitializeSampleTargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(TheSampleTarget, createSampleMCAsmInfo);
  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheSampleTarget,
                                        createSampleMCCodeGenInfo);
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheSampleTarget, createSampleMCInstrInfo);
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheSampleTarget, createSampleMCRegisterInfo);
  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(TheSampleTarget,
                                        createSampleMCCodeEmitter);
  // Register the object streamer.
  TargetRegistry::RegisterMCObjectStreamer(TheSampleTarget, createMCStreamer);
  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheSampleTarget,
                                       createSampleAsmBackend);
  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheSampleTarget,
                                          createSampleMCSubtargetInfo);
  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(TheSampleTarget,
                                        createSampleMCInstPrinter);
}

