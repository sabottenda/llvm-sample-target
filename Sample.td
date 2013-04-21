//===-- Sample.td - Describe the Sample Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This is the top level entry point for the Sample target.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//

class SampleReg<bits<4> num, string n> : Register<n> {
  field bits<4> num;
  let Namespace = "Sample";
}

// General Purpose Registers
def ZERO : SampleReg< 0, "ZERO">, DwarfRegNum<[0]>;
def V0   : SampleReg< 1, "V0">,   DwarfRegNum<[1]>;
def A0   : SampleReg< 2, "A0">,   DwarfRegNum<[2]>;
def A1   : SampleReg< 3, "A1">,   DwarfRegNum<[3]>;
def A2   : SampleReg< 4, "A2">,   DwarfRegNum<[4]>;
def A3   : SampleReg< 5, "A3">,   DwarfRegNum<[5]>;
def T0   : SampleReg< 6, "T0">,   DwarfRegNum<[6]>;
def T1   : SampleReg< 7, "T1">,   DwarfRegNum<[7]>;
def T2   : SampleReg< 8, "T2">,   DwarfRegNum<[8]>;
def T3   : SampleReg< 9, "T3">,   DwarfRegNum<[9]>;
def S0   : SampleReg< 10, "S0">,  DwarfRegNum<[10]>;
def S1   : SampleReg< 11, "S1">,  DwarfRegNum<[11]>;
def S2   : SampleReg< 12, "S2">,  DwarfRegNum<[12]>;
def S3   : SampleReg< 13, "S3">,  DwarfRegNum<[13]>;
def SP   : SampleReg< 14, "SP">,  DwarfRegNum<[14]>;
def RA   : SampleReg< 15, "RA">,  DwarfRegNum<[15]>;


//===----------------------------------------------------------------------===//
// Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"Sample", [i32], 32, (add
  // 戻り値と引数用レジスタ (Return values and Arguments registers)
  V0, A0, A1, A2, A3,
  // 呼び出し元待避レジスタ (Caller saved registers)
  T0, T1, T2, T3,
  // 呼び出し先待避レジスタ (Callee saved registers)
  S0, S1, S2, S3,
  // 予約レジスタ (Reserved registers)
  ZERO, SP, RA)>;

//===----------------------------------------------------------------------===//
// Functional units
//===----------------------------------------------------------------------===//

def ALU     : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes
//===----------------------------------------------------------------------===//
def IICAlu    : InstrItinClass;
def IICLoad   : InstrItinClass;
def IICStore  : InstrItinClass;
def IICBranch : InstrItinClass;
def IICPseudo : InstrItinClass;

//===----------------------------------------------------------------------===//
// Sample Generic instruction itineraries.
//===----------------------------------------------------------------------===//

def SampleGenericItineraries : ProcessorItineraries<[ALU], [], [
    InstrItinData<IICAlu    , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICLoad   , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICStore  , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICBranch , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICPseudo , [InstrStage<1,  [ALU]>]>
]>;

//===----------------------------------------------------------------------===//
// Sample Operand Definitions.
//===----------------------------------------------------------------------===//

// load/storeで利用するオペランド.20bit
// 19-16: CPURegs Operand(base)
// 15-0 : 符号付き16bit整数(offset)
// printMethod: base(offset) 形式で出力
// EncoderMethod: bit列から2つの値を取得
def mem : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops CPURegs, i16imm);
  let EncoderMethod = "getMemEncoding";
}

// 即値ロード用のオペランド. 20bit
// 19-0: 符号付き20bit整数
// EncoderMethod: bit列から符号付き20bit整数を取得
def movetarget : Operand<i32> {
  let EncoderMethod = "getMoveTargetOpValue";
}

def calltarget  : Operand<iPTR> {
  let EncoderMethod = "getCallTargetOpValue";
}

// 符号付き20bit整数
// 定数(ISD::Constant)で20bitで表現可能なもの
def immSExt20  : PatLeaf<(imm), [{ return isInt<20>(N->getSExtValue()); }]>;

//===----------------------------------------------------------------------===//
// Sample Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Sample Format Definitions.
//===----------------------------------------------------------------------===//

class Format<bits<3> val> {
  bits<3> Value = val;
}

def Pseudo    : Format<0>;
def FormReg0  : Format<1>;
def FormReg1  : Format<2>;
def FormReg2  : Format<3>;
def FormReg3  : Format<4>;

// Generic Sample Format
class SampleInst<dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin, Format f>
      : Instruction
{
  field bits<32> Inst;
  Format Form = f;

  bits<8> Opcode = 0;

  let Namespace = "Sample";
  let Size = 4;
  let Inst{31-24} = Opcode;
  let OutOperandList = outs;
  let InOperandList  = ins;
  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = itin;

  bits<3> FormBits = Form.Value;

  let DecoderNamespace = "Sample";

  field bits<32> SoftFail = 0;
}

class SamplePseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
  SampleInst<outs, ins, asmstr, pattern, IICPseudo, Pseudo> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
}

// FormReg0
class SampleInstFormReg0<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInst<outs, ins, asmstr, pattern, itin, FormReg0>
{
  bits<24> other;

  let Opcode = op;

  let Inst{23-0} = other;
}

// FormReg1
class SampleInstFormReg1<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInst<outs, ins, asmstr, pattern, itin, FormReg1>
{
  bits<4> rc;
  bits<20> other;

  let Opcode = op;

  let Inst{23-20} = rc;
  let Inst{19-0}  = other;
}

// FormReg3
class SampleInstFormReg3<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInst<outs, ins, asmstr, pattern, itin, FormReg3>
{
  bits<4> rc;
  bits<4> ra;
  bits<4> rb;
  bits<12> other;

  let Opcode = op;

  let Inst{23-20} = rc;
  let Inst{19-16} = ra;
  let Inst{15-12} = rb;
  let Inst{11-0}  = other;
}

//===----------------------------------------------------------------------===//
// Sample profiles and nodes
//===----------------------------------------------------------------------===//

// SDTypeProfileはSDNodeの必要条件を定義する。
// 第一引数: 結果Node数
// 第二引数: オペランドNode数
// 第三引数: 制約条件(SDTypeConstraint)

// SDTCisInt<N>: N番目のオペランドはInt型
// SDTCisVT<N, VT>: N番目のオペランドはVT型

// SDNodeは新しいSDNodeを定義する。
// 第一引数: opcode
// 第二引数: 制約条件(SDTypeProfile)
// 第三引数: SDNodeProperty

// SDNodeProperty
// SDNPCommutative : // 可換
// SDNPAssociative : // 結合法則
// SDNPHasChain    : // R/W chain operand and result
// SDNPOutGlue     : // Write a flag result
// SDNPInGlue      : // Read a flag operand
// SDNPOptInGlue   : // Optionally read a flag operand
// SDNPMayStore    : // May write to memory, sets 'mayStore'.
// SDNPMayLoad     : // May read memory, sets 'mayLoad'.
// SDNPSideEffect  : // Sets 'HasUnmodelledSideEffects'.
// SDNPMemOperand  : // Touches memory, has assoc MemOperand
// SDNPVariadic    : // 可変引数を持つ
// SDNPWantRoot    : // ComplexPattern gets the root of match
// SDNPWantParent  : // ComplexPattern gets the parent

def SDT_SampleRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SampleRet : SDNode<"SampleISD::Ret", SDT_SampleRet, 
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_SampleCall      : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

def SampleCall : SDNode<"SampleISD::Call",SDT_SampleCall,
                         [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
                          SDNPVariadic]>;

def SDT_SampleCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_SampleCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_SampleCallSeqStart,
                           [SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_SampleCallSeqEnd,
                           [SDNPHasChain, SDNPSideEffect,
                            SDNPOptInGlue, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

// Arithmetic and logical instructions with 3 register operands.
class ArithLogicInst<bits<8> op, string asmstr, SDNode OpNode, InstrItinClass itin, RegisterClass RC>
  : SampleInstFormReg3<op, (outs RC:$rc), (ins RC:$ra, RC:$rb),
                      !strconcat(asmstr, "\t$rc, $ra, $rb"),
                      [(set RC:$rc, (OpNode RC:$ra, RC:$rb))], itin> {
}

// Load/Store common form
// DecoderMethod: objdumpなどでバイナリから値をデコードする
class FMem<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInstFormReg1<op, outs, ins, asmstr, pattern, itin> {
  bits<20> addr;
  let Inst{19-0}  = addr;
  let DecoderMethod = "DecodeMem";
}

// Load
let canFoldAsLoad = 1 in
class LoadM<bits<8> op, string asmstr, RegisterClass RC>:
  FMem<op, (outs RC:$rc), (ins mem:$addr),
     !strconcat(asmstr, "\t$rc, $addr"),
     [(set RC:$rc, (load addr:$addr))], IICLoad>;

// Store
class StoreM<bits<8> op, string asmstr, RegisterClass RC>:
  FMem<op, (outs), (ins RC:$rc, mem:$addr),
     !strconcat(asmstr, "\t$rc, $addr"),
     [(store RC:$rc, addr:$addr)], IICStore>;

// Return
class RetInst<bits<8> op, string asmstr>:
  SampleInstFormReg1<op, (outs), (ins CPURegs:$rc),
     !strconcat(asmstr, "\t$rc"), [(SampleRet CPURegs:$rc)], IICBranch> {
  let isBranch=1;
  let isTerminator=1;
  let isBarrier=1;
  let isReturn=1;
}

// Load Immediate
// DecoderMethod: 即値ロード用のデコード
class LoadI<bits<8> op, string asmstr>:
  SampleInstFormReg1<op, (outs CPURegs:$rc), (ins movetarget:$other),
     !strconcat(asmstr, "\t$rc, $other"), [(set CPURegs:$rc, immSExt20:$other)], IICLoad> {
  let DecoderMethod = "DecodeMoveTarget";
}

class Call<bits<8> op, string asmstr>:
  SampleInstFormReg0<op, (outs), (ins calltarget:$other, variable_ops),
     !strconcat(asmstr, "\t$other"), [(SampleCall imm:$other)],
     IICBranch> {
  let isCall=1;
  let DecoderMethod = "DecodeCallTarget";
}

//===----------------------------------------------------------------------===//
// Sample Instruction definition
//===----------------------------------------------------------------------===//

def LOAD  : LoadM<0x00, "load", CPURegs>;
def STORE : StoreM<0x01, "store", CPURegs>;
def MOVE  : LoadI<0x02, "move">;
def CALL  : Call<0x03, "call">;
def RET   : RetInst<0x04, "ret">;
def ADD   : ArithLogicInst<0x05, "add", add, IICAlu, CPURegs>;
def SUB   : ArithLogicInst<0x06, "sub", sub, IICAlu, CPURegs>;

//===----------------------------------------------------------------------===//
// Pseudo instructions
//===----------------------------------------------------------------------===//

let Defs = [SP], Uses = [SP] in {
def ADJCALLSTACKDOWN : SamplePseudo<(outs), (ins i32imm:$amt),
                                  "!ADJCALLSTACKDOWN $amt",
                                  [(callseq_start timm:$amt)]>;
def ADJCALLSTACKUP   : SamplePseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                  "!ADJCALLSTACKUP $amt1",
                                  [(callseq_end timm:$amt1, timm:$amt2)]>;
}

//===----------------------------------------------------------------------===//
//  Arbitrary patterns that map to one or more instructions
//===----------------------------------------------------------------------===//

def : Pat<(SampleCall (i32 tglobaladdr:$dst)),
          (CALL tglobaladdr:$dst)>;
def : Pat<(SampleCall (i32 texternalsym:$dst)),
          (CALL texternalsym:$dst)>;

//===----------------------------------------------------------------------===//
// Sample Calling Convention
//===----------------------------------------------------------------------===//

def CC_Sample : CallingConv<[
  // i8/i16型の引数はi32型に昇格する
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // 整数型はレジスタに渡す
  CCIfType<[i32], CCAssignToReg<[A0, A1, A2, A3]>>
]>;

def RetCC_Sample : CallingConv<[
  // i32型はV0レジスタに渡す
  CCIfType<[i32], CCAssignToReg<[V0]>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved register lists.
//===----------------------------------------------------------------------===//

// 呼び出し先待避レジスタ(Callee-saved register)
def CSR_SingleFloatOnly : CalleeSavedRegs<(add (sequence "S%u", 3, 0), RA)>;

//===----------------------------------------------------------------------===//
// Sample processors supported.
//===----------------------------------------------------------------------===//

def SampleInstrInfo : InstrInfo;

def : Processor<"sample32", SampleGenericItineraries, []>;

def SampleAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  bit isMCAsmWriter = 1;
}

def Sample : Target {
  let InstructionSet = SampleInstrInfo;
  let AssemblyWriters = [SampleAsmWriter];
}
