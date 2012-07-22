//===-- Sample.h - Top-level interface for Sample representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM Sample back-end.
//
//===----------------------------------------------------------------------===//
#ifndef SAMPLE_TARGETMACHINE_H
#define SAMPLE_TARGETMACHINE_H

#include "SampleFrameLowering.h"
#include "SampleInstrInfo.h"
#include "SampleISelLowering.h"
#include "SampleSelectionDAGInfo.h"
#include "SampleRegisterInfo.h"
#include "SampleSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Support/Debug.h"

namespace llvm {

    class Module;

    class SampleTargetMachine : public LLVMTargetMachine {
        const TargetData DataLayout;
        SampleSubtarget Subtarget;
        SampleInstrInfo InstrInfo;
        SampleFrameLowering FrameLowering;
        SampleTargetLowering TLInfo;
        SampleSelectionDAGInfo TSInfo;

    public:
        SampleTargetMachine(const Target &T, StringRef TT,
                            StringRef CPU, StringRef FS, const TargetOptions &Options,
                            Reloc::Model RM, CodeModel::Model CM,
                            CodeGenOpt::Level OL);

        virtual const SampleInstrInfo *getInstrInfo() const {
            return &InstrInfo;
        }
        virtual const SampleSubtarget *getSubtargetImpl() const {
            return &Subtarget;
        }
        virtual const TargetRegisterInfo *getRegisterInfo() const {
            return &InstrInfo.getRegisterInfo();
        }
        virtual const TargetData *getTargetData() const {
            return &DataLayout;
        }
        virtual const SampleTargetLowering *getTargetLowering() const {
            return &TLInfo;
        }
        virtual const SampleFrameLowering *getFrameLowering() const{
            return &FrameLowering;
        }
        virtual const SampleSelectionDAGInfo* getSelectionDAGInfo() const {
            return &TSInfo;
        }

        // Pass Pipeline Configuration
        virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
    };

} // end namespace llvm

#endif
