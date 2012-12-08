##===- lib/Target/Sample/Makefile ----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMSampleCodeGen
TARGET = Sample

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = SampleGenRegisterInfo.inc SampleGenInstrInfo.inc \
                SampleGenAsmWriter.inc SampleGenCodeEmitter.inc \
                SampleGenDAGISel.inc SampleGenCallingConv.inc \
                SampleGenSubtargetInfo.inc SampleGenMCCodeEmitter.inc \
                SampleGenEDInfo.inc SampleGenDisassemblerTables.inc

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common

