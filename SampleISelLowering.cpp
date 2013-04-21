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
  DEBUG(dbgs() << ">> SampleTargetLowering::constructor <<\n");

  // booleanをどう表すかを定義
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrOneBooleanContent);

  // ターゲットで利用できるレジスタを登録
  addRegisterClass(MVT::i32, &Sample::CPURegsRegClass);

  // (符号)拡張ロード命令が対応していない型の操作方法を登録
  setLoadExtAction(ISD::EXTLOAD,  MVT::i1,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i8,  Promote);
  setLoadExtAction(ISD::EXTLOAD,  MVT::i16, Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::ZEXTLOAD, MVT::i16, Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i1,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i8,  Promote);
  setLoadExtAction(ISD::SEXTLOAD, MVT::i16, Promote);

  // 関数のアラインメント
  setMinFunctionAlignment(2);

  // スタックポインタのレジスタを指定
  setStackPointerRegisterToSaveRestore(Sample::SP);

  // レジスタの操作方法を計算
  computeRegisterProperties();
}

SDValue SampleTargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  llvm_unreachable("not supported operation");
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
  DEBUG(dbgs() << "  Chain: ";  Chain->dumpr(););

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
      // 引数がレジスタで渡された場合
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC = &Sample::CPURegsRegClass;

      DEBUG(dbgs() << "  Reg N" << i 
            << " LowerFormalArguments Unhandled argument type: "
            << RegVT.getSimpleVT().SimpleTy << "\n";);
      if (VA.getLocInfo() != CCValAssign::Full) {
        llvm_unreachable("not supported yet");
      }

      unsigned VReg = RegInfo.createVirtualRegister(RC);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()
      // 引数がメモリで渡された場合
 
      // Sanity check
      assert(VA.isMemLoc());
      // Load the argument to a virtual register
      unsigned ObjSize = VA.getLocVT().getSizeInBits()/8;
      DEBUG(dbgs() << "  Mem N" << i
            << " LowerFormalArguments Unhandled argument type: "
            << EVT(VA.getLocVT()).getEVTString()
            << "\n";);

      // フレームインデックスを作成する
      int FI = MFI->CreateFixedObject(ObjSize, VA.getLocMemOffset(), true);

      // スタックから引数を取得するためにloadノードを作成する
      SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
      InVals.push_back(DAG.getLoad(VA.getLocVT(), dl, Chain, FIN,
                                   MachinePointerInfo::getFixedStack(FI),
                                   false, false, false, 0));
    }
  }

  DEBUG(
      for (SmallVectorImpl<SDValue>::const_iterator i = InVals.begin();
           i != InVals.end(); ++i) {
        dbgs() << "  InVals: "; i->getNode()->dump();
      });
    
  DEBUG(dbgs() << ">> done LowerFormalArguments <<\n";);
  return Chain;
}

//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

/// LowerCall - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
/// TODO: isTailCall.
SDValue SampleTargetLowering::
LowerCall(CallLoweringInfo &CLI,
          SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG                     = CLI.DAG;
  DebugLoc &dl                          = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
  SmallVector<SDValue, 32> &OutVals     = CLI.OutVals;
  SmallVector<ISD::InputArg, 32> &Ins   = CLI.Ins;
  SDValue InChain                       = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &isTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool isVarArg                         = CLI.IsVarArg;

  DEBUG(dbgs() << ">> SampleTargetLowering::LowerCall <<\n");
  DEBUG(dbgs() << "  InChain: "; InChain->dumpr(););
  DEBUG(dbgs() << "  Callee: "; Callee->dumpr(););

  // 末尾呼び出しは未対応
  isTailCall = false;
  
  // 関数のオペランドを解析してオペランドをレジスタに割り当てる
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_Sample);

  // スタックを何Byte使っているか取得
  unsigned NumBytes = CCInfo.getNextStackOffset();
  DEBUG(dbgs() << "  stack offset: " << NumBytes << "\n");

  // 関数呼び出し開始のNode
  InChain = DAG.getCALLSEQ_START(InChain ,
                                 DAG.getConstant(NumBytes, getPointerTy(), true));

  SmallVector<std::pair<unsigned, SDValue>, 4> RegsToPass;
  SDValue StackPtr;

  // 引数をRegsToPassに追加していく
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    SDValue Arg = OutVals[i];
    CCValAssign &VA = ArgLocs[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    DEBUG(dbgs() << "  Arg: "; Arg->dumpr());

    // 引数が数値
    if (Flags.isByVal()) {
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      llvm_unreachable("ByVal arguments are not supported");
      continue;
    }

    // 必要に応じてPromoteする
    switch (VA.getLocInfo()) {
      default: llvm_unreachable("Unknown loc info!");
      case CCValAssign::Full: break;
      case CCValAssign::SExt:
        Arg = DAG.getNode(ISD::SIGN_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::ZExt:
        Arg = DAG.getNode(ISD::ZERO_EXTEND, dl, VA.getLocVT(), Arg);
        break;
      case CCValAssign::AExt:
        Arg = DAG.getNode(ISD::ANY_EXTEND, dl, VA.getLocVT(), Arg);
        break;
    }

    // レジスタ経由の引数はRegsToPassに追加
    if (VA.isRegLoc()) {
      DEBUG(dbgs() << "    Reg: " << VA.getLocReg() << "\n");
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
    } else {
      assert(VA.isMemLoc());
      llvm_unreachable("MemLoc arguments are not supported");
    }
  }

  // レジスタをコピーするノードを作成
  SDValue InFlag;
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    InChain = DAG.getCopyToReg(InChain, dl, RegsToPass[i].first,
                             RegsToPass[i].second, InFlag);
    InFlag = InChain.getValue(1);
  }

  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    Callee = DAG.getTargetGlobalAddress(G->getGlobal(), dl, MVT::i32);
    DEBUG(dbgs() << "  Global: " << Callee.getNode() << "\n");
  } else if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(E->getSymbol(), MVT::i32);
    DEBUG(dbgs() << "  External: " << Callee.getNode() << "\n");
  }

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(InChain);
  Ops.push_back(Callee);

  // 引数のレジスタをリストに追加
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Ops.push_back(DAG.getRegister(RegsToPass[i].first,
                                  RegsToPass[i].second.getValueType()));
  }

  if (InFlag.getNode())
    Ops.push_back(InFlag);

  InChain = DAG.getNode(SampleISD::Call, dl, NodeTys, &Ops[0], Ops.size());
  InFlag = InChain.getValue(1);

  // 関数呼び出し終了のNode
  InChain = DAG.getCALLSEQ_END(InChain,
                               DAG.getConstant(NumBytes, getPointerTy(), true),
                               DAG.getConstant(0, getPointerTy(), true),
                               InFlag);
  InFlag = InChain.getValue(1);

  // 戻り値の処理
  return LowerCallResult(InChain, InFlag, CallConv, isVarArg,
                         Ins, dl, DAG, InVals);
}

/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
SDValue SampleTargetLowering::
LowerCallResult(SDValue Chain, SDValue InFlag,
                CallingConv::ID CallConv, bool isVarArg,
                const SmallVectorImpl<ISD::InputArg> &Ins,
                DebugLoc dl, SelectionDAG &DAG,
                SmallVectorImpl<SDValue> &InVals) const {
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), RVLocs, *DAG.getContext());

  CCInfo.AnalyzeCallResult(Ins, RetCC_Sample);

  // 結果レジスタをコピー
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    Chain = DAG.getCopyFromReg(Chain, dl, RVLocs[i].getLocReg(),
                               RVLocs[i].getValVT(), InFlag).getValue(1);
    InFlag = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue SampleTargetLowering::
LowerReturn(SDValue Chain,
            CallingConv::ID CallConv, bool isVarArg,
            const SmallVectorImpl<ISD::OutputArg> &Outs,
            const SmallVectorImpl<SDValue> &OutVals,
            DebugLoc dl, SelectionDAG &DAG) const {
  DEBUG(dbgs() << ">> SampleTargetLowering::LowerReturn <<\n");
  DEBUG(dbgs() << " Chain: "; Chain->dumpr(););

  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(),
		 getTargetMachine(), RVLocs, *DAG.getContext());

  // 戻り値を解析する
  CCInfo.AnalyzeReturn(Outs, RetCC_Sample);

  // この関数で最初の戻り値の場合はレジスタをliveoutに追加
  if (DAG.getMachineFunction().getRegInfo().liveout_empty()) {
    for (unsigned i = 0; i != RVLocs.size(); ++i)
      if (RVLocs[i].isRegLoc())
        DAG.getMachineFunction().getRegInfo().addLiveOut(RVLocs[i].getLocReg());
  }

  SDValue Flag;

  // 戻り値をレジスタにコピーするノードを作成
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, dl, VA.getLocReg(),
                             OutVals[i], Flag);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Flag = Chain.getValue(1);
  }

  DEBUG(
      for (SmallVectorImpl<SDValue>::const_iterator i = OutVals.begin();
           i != OutVals.end(); ++i) {
        dbgs() << "  OutVals: "; i->getNode()->dump();
      });

  // 常に "ret $ra" を生成
  if (Flag.getNode())
    return DAG.getNode(SampleISD::Ret, dl, MVT::Other,
                       Chain, DAG.getRegister(Sample::RA, MVT::i32), Flag);
  else // Return Void
    return DAG.getNode(SampleISD::Ret, dl, MVT::Other,
                       Chain, DAG.getRegister(Sample::RA, MVT::i32));
}
