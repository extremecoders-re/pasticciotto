#ifndef INSTR_H
#define INSTR_H

/*
MEMORY LOCATIONS AND IMMEDIATES ARE 16 BITS LONG
*/

#ifdef DBG
enum INSTR_ENUM {
    MOVI,
    MOVR,
    LODI,
    LODR,
    STRI,
    STRR,
    ADDI,
    ADDR,
    SUBI,
    SUBR,
    ANDB,
    ANDW,
    ANDR,
    YORB,
    YORW,
    YORR,
    XORB,
    XORW,
    XORR,
    NOTR,
    MULI,
    MULR,
    DIVI,
    DIVR,
    SHLI,
    SHLR,
    SHRI,
    SHRR,
    PUSH,
    POOP,
    CMPB,
    CMPW,
    CMPR,
    JMPI,
    JMPR,
    JPAI,
    JPAR,
    JPBI,
    JPBR,
    JPEI,
    JPER,
    JPNI,
    JPNR,
    CALL,
    RETN,
    SHIT,
    NOPE,
    GRMN,
    DEBG,
    NUM_OPS
};
#else
enum INSTR_ENUM {
    MOVI,
    MOVR,
    LODI,
    LODR,
    STRI,
    STRR,
    ADDI,
    ADDR,
    SUBI,
    SUBR,
    ANDB,
    ANDW,
    ANDR,
    YORB,
    YORW,
    YORR,
    XORB,
    XORW,
    XORR,
    NOTR,
    MULI,
    MULR,
    DIVI,
    DIVR,
    SHLI,
    SHLR,
    SHRI,
    SHRR,
    PUSH,
    POOP,
    CMPB,
    CMPW,
    CMPR,
    JMPI,
    JMPR,
    JPAI,
    JPAR,
    JPBI,
    JPBR,
    JPEI,
    JPER,
    JPNI,
    JPNR,
    CALL,
    RETN,
    SHIT,
    NOPE,
    GRMN,
    NUM_OPS
};
#endif

/*
INSTRUCTION SIZE TYPES
*/
#define REG2REG 2
#define IMM2REG 4
#define BYT2REG 3
#define REGONLY 2
#define IMMONLY 3
#define SINGLE 1

/*
INSTRUCTION SIZES
*/
#define MOVI_SIZE IMM2REG
#define MOVR_SIZE REG2REG
#define LODI_SIZE IMM2REG
#define LODR_SIZE REG2REG
#define STRI_SIZE IMM2REG
#define STRR_SIZE REG2REG
#define ADDI_SIZE IMM2REG
#define ADDR_SIZE REG2REG
#define SUBI_SIZE IMM2REG
#define SUBR_SIZE REG2REG
#define ANDB_SIZE BYT2REG
#define ANDW_SIZE IMM2REG
#define ANDR_SIZE REG2REG
#define YORB_SIZE BYT2REG
#define YORW_SIZE IMM2REG
#define YORR_SIZE REG2REG
#define XORB_SIZE BYT2REG
#define XORW_SIZE IMM2REG
#define XORR_SIZE REG2REG
#define NOTR_SIZE REGONLY
#define MULI_SIZE IMM2REG
#define MULR_SIZE REG2REG
#define DIVI_SIZE IMM2REG
#define DIVR_SIZE REG2REG
#define SHLI_SIZE IMM2REG
#define SHLR_SIZE REG2REG
#define SHRI_SIZE IMM2REG
#define SHRR_SIZE REG2REG
#define PUSH_SIZE REGONLY
#define POOP_SIZE REGONLY
#define CMPB_SIZE BYT2REG
#define CMPW_SIZE IMM2REG
#define CMPR_SIZE REG2REG
#define JMPI_SIZE IMMONLY
#define JMPR_SIZE REGONLY
#define JPAI_SIZE IMMONLY
#define JPAR_SIZE REGONLY
#define JPBI_SIZE IMMONLY
#define JPBR_SIZE REGONLY
#define JPEI_SIZE IMMONLY
#define JPER_SIZE REGONLY
#define JPNI_SIZE IMMONLY
#define JPNR_SIZE REGONLY
#define CALL_SIZE IMMONLY
#define RETN_SIZE SINGLE
#define SHIT_SIZE SINGLE
#define NOPE_SIZE SINGLE
#define GRMN_SIZE SINGLE
#define DEBG_SIZE SINGLE

#endif
