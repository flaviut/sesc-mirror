/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2003 University of Illinois.

   Contributed by Jose Renau

This file is part of SESC.

SESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

SESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
SESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* This code is inspired in smt:scodes.c It translates MIPS codes to a
 * easier to manipulate format. MIPSInstruction.cpp, Instruction.h,
 * and ExecutionFlow.h should be the only files to modify if VESC is
 * ported to another fronted (embra,i386...)
 *
 */

#include "icode.h"
#include "opcodes.h"
#include "globals.h"

#include "Instruction.h"
#include "PPCDecoder.h"

void mint_init(int argc, char **argv, char **envp);
OP(mint_getpid);
int isFirstInFuncCall(unsigned long addr);
char *print_instr_regs(icode_ptr picode, thread_ptr pthread, int maxlen);

// iBJUncond is also true for all the conditional instruction
// marked as likely. If the compiler tells me that most of the
// time is taken. Do you believe the compiler?
// For crafty, the results are worse.

// Following the ISA advice can backfire through two places:
//
// 1-You have a very good predictor, and it's better than the ISA
// suggested.
//
// After playing with it for a while, I recommend not to follow the
// ISA advice
//
//#define FOLLOW_MIPSPRO_ADVICE 1

static const char *opcode2NameTable[] = {
  "iOpInvalid",
  "iALU",
  "iMult",
  "iDiv",
  "iBJ",
  "iLoad",
  "iStore",
  "fpALU",
  "fpMult",
  "fpDiv",
  "iFence",
  "iEvent"
};


icode_ptr Instruction::LowerLimit;
icode_ptr Instruction::UpperLimit;

int Instruction::maxFuncID=0;

Instruction *Instruction::InstTable = 0;
Instruction::InstHash Instruction::instHash;


size_t Instruction::InstTableSize = 0;

void Instruction::initialize(int argc,
			     char **argv,
			     char **envp)
{
#ifdef TRACE_DRIVEN
  initializeTrace(argc, argv, envp);
#else
  initializeMINT(argc, argv, envp);
#endif
}  

void Instruction::initializeTrace(int argc,
				 char **argv,
				 char **envp)
{
  PPCDecoder::Initialize();
}

void Instruction::initializeMINT(int argc,
				 char **argv,
				 char **envp)
{
  I(InstTable == 0);          // Not recursive calls

  mint_init(argc, argv, envp);
  
  Mint_output = 0;            // == /dev/null mint reports

  I(MaxEvent < SESC_MAXEVENT);
  
  InstTableSize = Text_size + EXTRA_ICODES;    // Itext size

  InstTable = (Instruction *) malloc(InstTableSize * sizeof(Instruction));

  I(InstTable);

  LowerLimit = Itext[0];
  UpperLimit = Itext[Text_size];

  size_t codeSize = InstTableSize - MaxEvent;

  maxFuncID=0;
  
  char *cFuncName = strdup("func0");

  for(size_t i = 0; i < codeSize; i++) {
    
    // This way to initialize is VERY VERY CRAPPY, but I don't
    // want to replicate code or start to pass parameters as crazy
    icode_ptr picode;
    InstType opcode;
    InstSubType subCode;
    RegType src1;
    RegType dest;
    RegType src2;
    EventType uEvent;
    bool BJCondLikely;
    bool gTaken;
    bool jumpLabel;

    MIPSDecodeInstruction(i, picode, opcode, subCode, src1, dest, src2, uEvent
			  ,BJCondLikely, gTaken, jumpLabel);

    I(picode->instID==i);

    if( opcode == iBJ && !BJCondLikely && uEvent == NoEvent ) {
      // BJCondLikely MINT already reorganizes them for me. In MIPS
      // the delay slot is executed only if the branch is taken. This
      // means that the delay slot is skipped if the branch is not
      // taken.
      picode->opflags |= E_UFUNC;
    }
    
    // In case that a iLoad has two dependences, the
    // MAX_PENDING_SOURCES from DInst should be increased.
    GI(opcode == iLoad,  src2 == NoDependence);
    
    if( subCode == BJCond && !gTaken ) {
      if( picode->target < picode )
	gTaken = true;
    }
    if( subCode == BJCond ) {
      if( i > 10 ) {
	switch( Itext[i-1]->opnum ) {
	case andi_opn:
	case and_opn:
	  gTaken = false;
	}
      }
    }

    if( dest == NoDependence )
      dest = InvalidOutput;
    
    if( subCode == iNop ) {
      src1 = NoDependence;
      src2 = NoDependence;
      dest = InvalidOutput;
    }

    GI(opcode == iStore, dest == InvalidOutput);

    if(src2 != NoDependence && src1 == NoDependence) {
      // This happens for instructions like: sw $zero,4($a1)
      src1 = src2;
      src2 = NoDependence;
    }

    if( isFirstInFuncCall(Itext[i]->addr) ) {
      maxFuncID++;
#ifdef AIX
      char name[100];
      sprintf(name,"func%d",maxFuncID);
      cFuncName = strdup(name);
#endif
    }

#ifdef AIX
    InstTable[i].funcName = cFuncName;
#endif

    I(src1 < NumArchRegs);
    I(src2 < NumArchRegs);
    I(dest < NumArchRegs);

    InstTable[i].opcode = opcode;
    InstTable[i].subCode= subCode;
    InstTable[i].src1   = src1;
    InstTable[i].dest   = dest;
    InstTable[i].src2   = src2;
    InstTable[i].uEvent = uEvent;
    InstTable[i].guessTaken = gTaken;
    InstTable[i].condLikely = BJCondLikely;
    InstTable[i].jumpLabel  = jumpLabel;

    InstTable[i].src1Pool = whichPool(src1);
    InstTable[i].src2Pool = whichPool(src2);
    InstTable[i].dstPool  = whichPool(dest);
    
    if(opcode == iBJ) {
      if( !BJCondLikely ) 
	InstTable[i].skipDelay = 2; // Delay slot
      else
	InstTable[i].skipDelay = 1; // Branch Link no delay slot (executed only if taken)
      // InstTable[i].skipDelay = 2;
    }else{
      InstTable[i].skipDelay   = 1; // Normal inst
    }
  }

  // IMPORTANT: All the events must have NoDependence in the dest
  // field.

  // Fake entry for PreEvent
  // Useless picode field. The only requirement is not to point to Itext[0]
  InstTable[codeSize + PreEvent].opcode = iEvent;
  InstTable[codeSize + PreEvent].subCode= iSubInvalid;  // Not used
  InstTable[codeSize + PreEvent].src1   = NoDependence;
  InstTable[codeSize + PreEvent].src2   = NoDependence;
  InstTable[codeSize + PreEvent].dest   = InvalidOutput;
  InstTable[codeSize + PreEvent].uEvent = PreEvent;

  // Fake entry for PostEvent
  InstTable[codeSize + PostEvent].opcode = iEvent;
  InstTable[codeSize + PostEvent].subCode= iSubInvalid; // Not used
  InstTable[codeSize + PostEvent].src1   = (RegType)4; // R4 is the first parameter
  InstTable[codeSize + PostEvent].src2   = (RegType)6; // R6 is the third parameter
  InstTable[codeSize + PostEvent].dest   = InvalidOutput;
  InstTable[codeSize + PostEvent].uEvent = PostEvent;

  // Fake entry for FetchOp
  InstTable[codeSize + FetchOpEvent].opcode = iFence;
  InstTable[codeSize + FetchOpEvent].subCode= iFetchOp;
  InstTable[codeSize + FetchOpEvent].src1   = (RegType)5; // *addr
  InstTable[codeSize + FetchOpEvent].src2   = (RegType)6; // val
  InstTable[codeSize + FetchOpEvent].dest   = (RegType)2; // return
  InstTable[codeSize + FetchOpEvent].uEvent = FetchOpEvent;

  // Fake entry for MemFence
  InstTable[codeSize + MemFenceEvent].opcode = iFence;
  InstTable[codeSize + MemFenceEvent].subCode= iMemFence;
  InstTable[codeSize + MemFenceEvent].src1   = (RegType)4;
  InstTable[codeSize + MemFenceEvent].src2   = NoDependence;
  InstTable[codeSize + MemFenceEvent].dest   = InvalidOutput;
  InstTable[codeSize + MemFenceEvent].uEvent = MemFenceEvent;

  // Fake entry for Acquire
  InstTable[codeSize + AcquireEvent].opcode = iFence;
  InstTable[codeSize + AcquireEvent].subCode= iAcquire;
  InstTable[codeSize + AcquireEvent].src1   = (RegType)4;
  InstTable[codeSize + AcquireEvent].src2   = NoDependence;
  InstTable[codeSize + AcquireEvent].dest   = InvalidOutput;
  InstTable[codeSize + AcquireEvent].uEvent = AcquireEvent;

  // Fake entry for Release
  InstTable[codeSize + ReleaseEvent].opcode = iFence;
  InstTable[codeSize + ReleaseEvent].subCode= iRelease;
  InstTable[codeSize + ReleaseEvent].src1   = (RegType)4;
  InstTable[codeSize + ReleaseEvent].src2   = NoDependence;
  InstTable[codeSize + ReleaseEvent].dest   = InvalidOutput;
  InstTable[codeSize + ReleaseEvent].uEvent = ReleaseEvent;
}

void Instruction::finalize()
{
#ifdef TRACE_DRIVEN
  //TODO: go through the instHash deleting the insts
#else
    free(InstTable);
    InstTable = 0;
#endif
}

void Instruction::MIPSDecodeInstruction(size_t        index
                                        ,icode_ptr   &epicode
                                        ,InstType    &opcode
                                        ,InstSubType &subCode
                                        ,RegType     &src1
                                        ,RegType     &dest
                                        ,RegType     &src2
                                        ,EventType   &uEvent
					,bool        &BJCondLikely
					,bool        &guessTaken
					,bool        &jumpLabel
  )
{
  I(index < InstTableSize);
  epicode = Itext[index];

  opcode = iOpInvalid;
  subCode = iSubInvalid;
  src1 = NoDependence;
  dest = InvalidOutput;
  src2 = NoDependence;
  uEvent = NoEvent;
  BJCondLikely = false;
  guessTaken   = false;
  jumpLabel    = false;

  icode_t icode;
  icode_ptr picode = &icode;

  picode->opnum = decode_instr(picode, epicode->instr);

  switch (picode->opnum) {
  case move_opn:
  case addu_opn:
  case add_opn:
  case and_opn:
  case nor_opn:
  case or_opn:
  case sllv_opn:
  case slt_opn:
  case sltu_opn:
  case srlv_opn:
  case srav_opn:
  case sub_opn:
  case subu_opn:
  case xor_opn:
    opcode = iALU;
    dest = static_cast < RegType > (picode->getRN(RD));
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case li_opn:
  case addiu_opn:
  case addi_opn:
  case slti_opn:
  case sltiu_opn:
  case andi_opn:
  case ori_opn:
  case xori_opn:
    opcode = iALU;
    dest = static_cast < RegType > (picode->getRN(RT));
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case beq_opn:
  case b_opn:
  case bne_opn:
    opcode = iBJ;
    subCode = BJCond;
    guessTaken = true;
    jumpLabel  = true;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case beql_opn:
  case bnel_opn:
    opcode  = iBJ;
    guessTaken = true;
    jumpLabel  = true;
#ifdef FOLLOW_MIPSPRO_ADVICE
    subCode = BJUncond;
#else
    subCode = BJCond;
#endif
    BJCondLikely = true;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case bgez_opn:
  case bgtz_opn:
  case blez_opn:
  case bltz_opn:
    opcode = iBJ;
    subCode = BJCond;
    guessTaken = false;
    jumpLabel  = true;
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case bgezal_opn:
  case bltzal_opn:
    opcode  = iBJ;
    subCode = BJCall;
    guessTaken = true;
    jumpLabel  = true;
    src1    = static_cast < RegType > (picode->getRN(RS));
    dest    = ReturnReg;
    break;
  case bgezall_opn:
  case bltzall_opn:
    opcode       = iBJ;
    subCode      = BJCall;
    BJCondLikely = true;
    guessTaken   = true;
    jumpLabel    = true;
    src1         = static_cast < RegType > (picode->getRN(RS));
    dest         = ReturnReg;
    break;
  case bgezl_opn:
  case bgtzl_opn:
  case blezl_opn:
  case bltzl_opn:
    opcode       = iBJ;
#ifdef FOLLOW_MIPSPRO_ADVICE
    subCode = BJUncond;
#else
    subCode = BJCond;
#endif
    guessTaken   = true;
    jumpLabel  = true;
    BJCondLikely = true;
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case break_opn:
  case cache_opn:
  case cop0_opn:
  case cop1_opn:
  case cop2_opn:
  case cop3_opn:
    opcode = fpALU; // This is not really ALU, but who cares
    break;
  case mult_opn:
  case multu_opn:
    opcode = iMult;
    dest = HiReg;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case div_opn:
  case divu_opn:
    opcode = iDiv;
    dest = HiReg;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case j_opn:
    opcode = iBJ;
    guessTaken   = true;
    jumpLabel  = true;
    subCode = BJUncond;
    break;
  case jal_opn:
    opcode  = iBJ;
    subCode = BJCall;
    jumpLabel  = true;
    guessTaken   = true;
    dest    = ReturnReg;
    break;
  case jalr_opn:
    opcode = iBJ;
    subCode = BJCall;
    guessTaken   = true;
    dest = static_cast < RegType > (picode->getRN(RD));
    src2 = static_cast < RegType > (picode->getRN(RS));
    break;
  case jr_opn:
    opcode  = iBJ;
    subCode = BJRet;
    guessTaken   = true;
    src2    = static_cast < RegType > (picode->getRN(RS));
    break;
  case lb_opn:
  case lbu_opn:
  case lh_opn:
  case lhu_opn:
  case ll_opn:
  case lw_opn:
  case lwl_opn:
  case lwr_opn:
    opcode  = iLoad;
    subCode = iMemory;
    dest    = static_cast < RegType > (picode->getRN(RT));
    src1    = static_cast < RegType > (picode->getRN(RS));
    break;
  case sb_opn:
  case sc_opn:
  case sh_opn:
  case sw_opn:
  case swl_opn:
  case swr_opn:
    opcode = iStore;
    subCode = iMemory;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case ldc1_opn:
  case ldc2_opn:
  case ldc3_opn:
    opcode = iLoad;
    subCode = iMemory;
    dest = static_cast < RegType > (picode->getDPN(RT) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case lwc1_opn:
  case lwc2_opn:
  case lwc3_opn:
    opcode = iLoad;
    subCode = iMemory;
    dest = static_cast < RegType > (picode->getFPN(RT) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case sdc1_opn:
  case sdc2_opn:
  case sdc3_opn:
    opcode = iStore;
    subCode = iMemory;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getDPN(RT) + static_cast < int >(IntFPBoundary));
    break;
  case swc1_opn:
  case swc2_opn:
  case swc3_opn:
    opcode = iStore;
    subCode = iMemory;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getFPN(RT) + static_cast < int >(IntFPBoundary));
    break;
  case lui_opn:
    opcode = iALU;
    dest = static_cast < RegType > (picode->getRN(RT));
    break;
  case mfhi_opn:
  case mflo_opn:
    opcode = iALU;
    dest = static_cast < RegType > (picode->getRN(RD));
    src1 = HiReg;
    break;
  case mthi_opn:
  case mtlo_opn:
    opcode = iALU;
    dest = HiReg;
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;
  case nop_opn:
    opcode  = iALU;
    subCode = iNop;
    break;
  case sll_opn:
  case sra_opn:
  case srl_opn:
    opcode = iALU;
    dest = static_cast < RegType > (picode->getRN(RD));
    src1 = static_cast < RegType > (picode->getRN(RT));
    break;
  case sync_opn:
  case syscall_opn:
    opcode = fpALU;
    break;
  case teq_opn:
  case tge_opn:
  case tgeu_opn:
  case tlt_opn:
  case tltu_opn:
  case tne_opn:
    // Traps not implemented
    opcode = iALU;
    subCode = iNop;
    src1 = static_cast < RegType > (picode->getRN(RS));
    src2 = static_cast < RegType > (picode->getRN(RT));
    break;
  case teqi_opn:
  case tgei_opn:
  case tgeiu_opn:
  case tlti_opn:
  case tltiu_opn:
  case tnei_opn:
    // Traps not implemented
    opcode = iALU;
    subCode = iNop;
    src1 = static_cast < RegType > (picode->getRN(RS));
    break;

    /*
     * coprocessor instructions 
     */
  case add_s_opn:
  case sub_s_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getFPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case mul_s_opn:
    opcode = fpMult;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getFPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case div_s_opn:
    opcode = fpDiv;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getFPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case sqrt_s_opn:
    opcode = fpDiv;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case abs_s_opn:
  case mov_s_opn:
  case neg_s_opn:
  case round_w_s_opn:
  case trunc_w_s_opn:
  case ceil_w_s_opn:
  case floor_w_s_opn:
  case cvt_w_s_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case cvt_d_s_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case c_f_s_opn:
  case c_un_s_opn:
  case c_eq_s_opn:
  case c_ueq_s_opn:
  case c_olt_s_opn:
  case c_ult_s_opn:
  case c_ole_s_opn:
  case c_ule_s_opn:
  case c_sf_s_opn:
  case c_ngle_s_opn:
  case c_seq_s_opn:
  case c_ngl_s_opn:
  case c_lt_s_opn:
  case c_nge_s_opn:
  case c_le_s_opn:
  case c_ngt_s_opn:
    opcode = fpALU;
    dest = CoprocStatReg;
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getFPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case add_d_opn:
  case sub_d_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getDPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case mul_d_opn:
    opcode = fpMult;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getDPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case div_d_opn:
    opcode = fpDiv;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getDPN(FT) + static_cast < int >(IntFPBoundary));
    break;
  case sqrt_d_opn:
    opcode = fpDiv;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case abs_d_opn:
  case mov_d_opn:
  case neg_d_opn:
  case round_w_d_opn:
  case trunc_w_d_opn:
  case ceil_w_d_opn:
  case floor_w_d_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case cvt_s_d_opn:
  case cvt_w_d_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case c_f_d_opn:
  case c_un_d_opn:
  case c_eq_d_opn:
  case c_ueq_d_opn:
  case c_olt_d_opn:
  case c_ult_d_opn:
  case c_ole_d_opn:
  case c_ule_d_opn:
  case c_sf_d_opn:
  case c_ngle_d_opn:
  case c_seq_d_opn:
  case c_ngl_d_opn:
  case c_lt_d_opn:
  case c_nge_d_opn:
  case c_le_d_opn:
  case c_ngt_d_opn:
    opcode = fpALU;
    dest = CoprocStatReg;
    src1 = static_cast < RegType > (picode->getDPN(FS) + static_cast < int >(IntFPBoundary));
    src2 = static_cast < RegType > (picode->getDPN(FT) + static_cast < int >(IntFPBoundary));
    break;

    /*
     * fixed-point precision 
     */
  case cvt_s_w_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getFPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;
  case cvt_d_w_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getDPN(FD) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;

    /*
     * other coprocessor instructions 
     */
  case mfc1_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getRN(RT));
    src1 = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    break;

  case mtc1_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getFPN(FS) + static_cast < int >(IntFPBoundary));
    src1 = static_cast < RegType > (picode->getRN(RT));
    break;
  case cfc1_opn:
    opcode = fpALU;
    dest = static_cast < RegType > (picode->getRN(RT));
    src1 = CoprocStatReg;
    break;
  case ctc1_opn:
    opcode = fpALU;
    dest = CoprocStatReg;
    src1 = static_cast < RegType > (picode->getRN(RT));
    break;

    /*
     * coprocessor branch codes 
     */
  case bc0f_opn:
  case bc0t_opn:
  case bc1f_opn:
  case bc1t_opn:
  case bc2f_opn:
  case bc2t_opn:
  case bc3f_opn:
  case bc3t_opn:
    opcode = iBJ;
    subCode = BJCond;
    guessTaken = false;
    jumpLabel  = true;
    src1 = CoprocStatReg;
    break;

  case bc3fl_opn:
  case bc3tl_opn:
  case bc2fl_opn:
  case bc2tl_opn:
  case bc0fl_opn:
  case bc0tl_opn:
  case bc1fl_opn:
  case bc1tl_opn:
    opcode = iBJ;
#ifdef FOLLOW_MIPSPRO_ADVICE
    subCode = BJUncond;
#else
    subCode = BJCond;
#endif
    guessTaken   = true;
    BJCondLikely = true;
    jumpLabel    = true;
    src1 = CoprocStatReg;
    break;
  case terminate_opn:
    opcode  = iALU;
    subCode = iNop;
    MSG("epicode opnum[%d] addr[0x%x]", epicode->opnum, (unsigned int)epicode->addr);
    break;
  case reserved_opn:
  case invalid_opn:
    opcode  = iALU;
    subCode = iNop;
    MSG("epicode opnum[%d] addr[0x%x]", epicode->opnum, (unsigned int)epicode->addr);
    break;

  //User Defined Instructions in MINT:
  case call_opn:
    opcode  = iBJ;
    subCode = BJCall;
    jumpLabel  = true;
    guessTaken   = true;
    dest    = ReturnReg;
    break;
  case swallow_opn:
  case spawn_opn:
    opcode = iALU;
    subCode = iNop;
    break;
  //End User Defined Instructions in MINT

  default:
    I(0);
    MSG("epicode opnum[%d] addr[0x%x]", epicode->opnum, (unsigned int)epicode->addr);
    break;
  }

  if(opcode == iBJ && subCode == BJRet) {
    if(!(( static_cast < RegType > (picode->getRN(RS)) == ReturnReg)
	 || (static_cast < RegType > (picode->getRN(RS)) == 25 && InstTable[index-1].opcode == iALU) ) )
      subCode = BJUncond;
  }

  if(index == 0) {
    opcode  = iALU;
    subCode = iNop;
  }

  if(epicode->next == 0)
    return;

  picode = epicode->next;

  if(!inLimits(picode))
    return;

  if(!picode->is_target)
    return;

  if(picode->next == 0)
    return;

  if(picode->next->addr != picode->addr)
    return;

  // Here for sure that it is a intercepted library call.

  // MSG("library[0x%x]",picode->addr);

  // MINT notifies the first instruction of the intercepted library
  // calls. To balance the RAT, I simulate a RET.
  opcode  = iBJ;
  subCode = BJRet;
  dest    = NoDependence;
  src1    = ReturnReg;
  src2    = NoDependence;
  I(!BJCondLikely);
  uEvent  = LibCallEvent;
    
  // special library patchs This is only required to skip the jal
  // instruction. In that way the overhead is absolutly nothing.

  if(picode->func == mint_sesc_preevent) {
    uEvent = PreEvent;
  } else if(picode->func == mint_sesc_postevent) {
    uEvent = PostEvent;
  } else if(picode->func == mint_sesc_fetch_op) {
    uEvent = FetchOpEvent;
  } else if(picode->func == mint_sesc_memfence) {
    uEvent = MemFenceEvent;
  } else if(picode->func == mint_sesc_acquire) {
    uEvent = ReleaseEvent;
  } else if(picode->func == mint_sesc_release ) {
    uEvent = AcquireEvent;
  }
#ifdef TASKSCALAR
  if(picode->func == mint_sesc_fork_successor)
    uEvent = SpawnEvent;
#endif
}

const char *Instruction::opcode2Name(InstType op)
{
  return opcode2NameTable[op];
}

void Instruction::dump(const char *str) const
{
  MSG("%s:0x%8x: reg[%2d] = reg[%2d] [%8s:%2d] reg[%2d] (uEvent=%d)", str,
      (int)getAddr(), dest, src1, opcode2Name(opcode), subCode, src2, uEvent);

  Itext[currentID()]->dump();
}

void Instruction::PPCDecodeInstruction(Instruction *inst, ulong rawInst)
{
  PPCInstDef *ppcInst = PPCDecoder::getInstDef(rawInst);

  inst->src1 = NoDependence;
  inst->dest = InvalidOutput;
  inst->src2 = NoDependence;
  inst->uEvent = NoEvent;
  inst->condLikely = false;
  inst->guessTaken = false;
  inst->jumpLabel = false;

  inst->opcode = ppcInst->type;
  inst->subCode = ppcInst->subType;

  // TODO: add the instruction specific sutff here, like register numbers, pools, etc..
  // most of the instructions can be decoded based on the form.
  // except some corner cases, so the code can be *very* clean.

}
