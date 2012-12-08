//===-- SampleISelLowering.cpp - Sample DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that Sample uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "sample-lower"
#include "SampleISelLowering.h"
#include "SampleMachineFunction.h"
#include "SampleTargetMachine.h"
#include "SampleSubtarget.h"
#include "InstPrinter/SampleInstPrinter.h"
#include "MCTargetDesc/SampleMCTargetDesc.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/GlobalVariable.h"
#include "llvm/Intrinsics.h"
#include "llvm/CallingConv.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
using namespace llvm;

std::string getFlagsString(const ISD::ArgFlagsTy &Flags) {
    if (Flags.isZExt()) {
        return "ZExt";
    } else if (Flags.isSExt()) {
        return "SExt";
    } else if (Flags.isInReg()) {
        return "Reg";
    } else if (Flags.isSRet()) {
        return "SRet";
    } else if (Flags.isByVal()) {
        return "ByVal";
    } else if (Flags.isNest()) {
        return "Nest";
    } else {
        return "No Flags";
    }
}

SampleTargetLowering::
SampleTargetLowering(SampleTargetMachine &TM)
  : TargetLowering(TM, new TargetLoweringObjectFileELF()),
    Subtarget(*TM.getSubtargetImpl()) {
  DEBUG(dbgs() << "InitSampleTargetLowering\n");

  // Sample does not have i1 type, so use i32 for
  // setcc operations results (slt, sgt, ...).
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent);

  // Set up the register classes
  addRegisterClass(MVT::i32, Sample::CPURegsRegisterClass);

  // Load extented operations for i1 types must be promoted
  setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i8,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i16, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Promote);

  // Used by legalize types to correctly generate the setcc result.
  // Without this, every float setcc comes with a AND/OR with the result,
  // we don't want this, since the fpcmp result goes to a flag register,
  // which is used implicitly by brcond and select operations.
  AddPromotedToType(ISD::SETCC, MVT::i1, MVT::i32);

  setMinFunctionAlignment(2);

  setStackPointerRegisterToSaveRestore(Sample::SP);
  computeRegisterProperties();
}

SDValue SampleTargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  return SDValue();
}

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "SampleGenCallingConv.inc"

SDValue SampleTargetLowering::
LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                     bool isVarArg,
                     const SmallVectorImpl<ISD::InputArg> &Ins,
                     DebugLoc dl, SelectionDAG &DAG,
                     SmallVectorImpl<SDValue> &InVals) const {
    DEBUG(dbgs() << ">> SampleTargetLowering::LowerFormalArguments <<\n");
    dbgs() << "Chain:"; 
    Chain->printrFull(dbgs(), 0);
    dbgs() << "\n";

    MachineFunction &MF = DAG.getMachineFunction();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
                   getTargetMachine(), ArgLocs, *DAG.getContext());
    CCInfo.AnalyzeFormalArguments(Ins, CC_Sample);

    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        CCValAssign &VA = ArgLocs[i];
        if (VA.isRegLoc()) {
            // Arguments passed in registers
            EVT RegVT = VA.getLocVT();
            const TargetRegisterClass *RC = Sample::CPURegsRegisterClass;

            dbgs() << "Reg N" << i << " LowerFormalArguments Unhandled argument type: "
                   << RegVT.getSimpleVT().SimpleTy << "\n";
            if (VA.getLocInfo() != CCValAssign::Full) {
                llvm_unreachable("hoge full");
            }


            unsigned VReg = RegInfo.createVirtualRegister(RC);
            RegInfo.addLiveIn(VA.getLocReg(), VReg);
            SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
            InVals.push_back(ArgValue);
        } else { // VA.isRegLoc()
            // Sanity check
            assert(VA.isMemLoc());
            // Load the argument to a virtual register
            unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
            dbgs() << "Mem N" << i << " LowerFormalArguments Unhandled argument type: "
                   << EVT(VA.getLocVT()).getEVTString()
                   << "\n";

            // Create the frame index object for this incoming parameter...
            int FI = MFI->CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);

            // Create the SelectionDAG nodes corresponding to a load
            //from this parameter
            SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
            InVals.push_back(DAG.getLoad(VA.getLocVT(), dl, Chain, FIN,
                                         MachinePointerInfo::getFixedStack(FI),
                                         false, false, false, 0));
        }
    }

    for (SmallVectorImpl<SDValue>::const_iterator i = InVals.begin();
         i != InVals.end(); ++i) {
        dbgs() << ">>> InVals: "; i->getNode()->dump();
    }
    
    dbgs() << ">> done LowerFormalArguments <<\n";
    return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
SampleTargetLowering::LowerReturn(SDValue Chain,
                                  CallingConv::ID CallConv, bool isVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  DebugLoc dl, SelectionDAG &DAG) const {
  DEBUG(dbgs() << ">> SampleTargetLowering::LowerReturn <<\n");
  dbgs() << "Chain:";
  Chain->printrFull(dbgs(), 0);
  dbgs() << "\n";

  // CCValAssign - represent the assignment of the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), RVLocs, *DAG.getContext());

  // Analize return values.
  CCInfo.AnalyzeReturn(Outs, RetCC_Sample);

  // If this is the first return lowered for this function, add the regs to the
  // liveout set for the function.
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                             OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
  }

  for (SmallVectorImpl<SDValue>::const_iterator i = OutVals.begin();
       i != OutVals.end(); ++i) {
      dbgs() << "OutVals: "; i->getNode()->dump();
  }

  // Return on Sample is always a "jump $ra"
  if (Flag.getNode())
    return DAG.getNode(SampleISD::Ret, dl, MVT::Other,
                       Chain, DAG.getRegister(Sample::RA, MVT::i32), Flag);
  else // Return Void
    return DAG.getNode(SampleISD::Ret, dl, MVT::Other,
                       Chain, DAG.getRegister(Sample::RA, MVT::i32));
}
