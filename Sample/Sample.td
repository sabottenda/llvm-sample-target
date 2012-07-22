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
  // Return Values and Arguments
  V0, A0, A1, A2, A3,
  // Not preserved across procedure calls
  T0, T1, T2, T3,
  // Callee save
  S0, S1, S2, S3,
  // Reserved
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
def IICMul    : InstrItinClass;
def IICDiv    : InstrItinClass;
def IICPseudo : InstrItinClass;

//===----------------------------------------------------------------------===//
// Sample Generic instruction itineraries.
//===----------------------------------------------------------------------===//

def SampleGenericItineraries : ProcessorItineraries<[ALU], [], [
    InstrItinData<IICAlu    , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICLoad   , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICStore  , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICBranch , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICMul    , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICDiv    , [InstrStage<1,  [ALU]>]>,
    InstrItinData<IICPseudo , [InstrStage<1,  [ALU]>]>
]>;

//===----------------------------------------------------------------------===//
// Sample Operand Definitions.
//===----------------------------------------------------------------------===//

// Address operands
def mem : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops CPURegs, i16imm);
}

//===----------------------------------------------------------------------===//
// Sample Complex Pattern Definitions.
//===----------------------------------------------------------------------===//

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Sample Format Definitions.
//===----------------------------------------------------------------------===//

class Format<bits<2> val> {
  bits<2> Value = val;
}

def Pseudo    : Format<0>;
def FormReg1  : Format<1>;
def FormReg2  : Format<2>;
def FormReg3  : Format<3>;

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

  bits<2> FormBits = Form.Value;

  let DecoderNamespace = "Sample";

  field bits<32> SoftFail = 0;
}

// FormReg1
class SampleInstFormReg1<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInst<outs, ins, asmstr, pattern, itin, FormReg3>
{
  bits<4> rc;
  bits<20> other = 0;

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
  bits<12> other = 0;

  let Opcode = op;

  let Inst{23-20} = rc;
  let Inst{19-16} = ra;
  let Inst{15-12} = rb;
  let Inst{11-0}  = other;
}

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

// Arithmetic and logical instructions with 3 register operands.
class ArithLogicInst<bits<8> op, string instr_asm, SDNode OpNode, InstrItinClass itin, RegisterClass RC>
  : SampleInstFormReg3<op, (outs RC:$rc), (ins RC:$ra, RC:$rb),
                      !strconcat(instr_asm, "\t$rc, $ra, $rb"),
                      [(set RC:$rc, (OpNode RC:$ra, RC:$rb))], itin> {
}

class FMem<bits<8> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
      : SampleInstFormReg1<op, outs, ins, asmstr, pattern, itin> {
  bits<20> addr;
  let Inst{19-0}  = addr;
}


let canFoldAsLoad = 1 in
class LoadM<bits<8> op, string asmstr, RegisterClass RC>:
  FMem<op, (outs RC:$rc), (ins mem:$addr),
     !strconcat(asmstr, "\t$rc, $addr"),
     [(set RC:$rc, (load addr:$addr))], IICAlu>;

class StoreM<bits<8> op, string asmstr, RegisterClass RC>:
  FMem<op, (outs), (ins i8imm:$addr, RC:$rt),
     !strconcat(asmstr, "\t$rt, $addr"),
     [(store RC:$rt, addr:$addr)], IICAlu>;

//===----------------------------------------------------------------------===//
// Sample profiles and nodes
//===----------------------------------------------------------------------===//

def SDT_SampleRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def SampleRet : SDNode<"SampleISD::Ret", SDT_SampleRet, [SDNPHasChain,
                     SDNPOptInGlue]>;

//===----------------------------------------------------------------------===//
// Sample Instruction definition
//===----------------------------------------------------------------------===//

def ADD   : ArithLogicInst<0x03, "add",   add, IICAlu, CPURegs>;
def SUB   : ArithLogicInst<0x04, "sub",   sub, IICAlu, CPURegs>;

let isReturn=1, isTerminator=1, isCodeGenOnly=1, isBarrier=1,
    ra=0, rb=0, other=0 in
  def RET : SampleInstFormReg3<0x02, (outs), (ins CPURegs:$target),
                  "jump\t$target", [(SampleRet CPURegs:$target)], IICAlu>;

def LOAD  : LoadM<0x00, "load", CPURegs>;
def STORE : StoreM<0x01, "store", CPURegs>;

//===----------------------------------------------------------------------===//
// Sample Calling Convention
//===----------------------------------------------------------------------===//

def CC_Sample : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // Integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[A0, A1, A2, A3]>>,

  // All stack parameter slots become 64-bit doublewords and are 8-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;


def RetCC_Sample : CallingConv<[
  // i32 are returned in registers V0
  CCIfType<[i32], CCAssignToReg<[V0]>>
]>;

//===----------------------------------------------------------------------===//
// Callee-saved register lists.
//===----------------------------------------------------------------------===//

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
