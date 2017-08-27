#include "vm.h"
#include <string.h>
#include <stdexcept>

void VM::encryptOpcodes(uint8_t *key) {
    uint8_t arr[256];
    uint32_t i, j, tmp, keysize;
    keysize = strlen((char *) key);

    DBG_INFO(("Encrypting instructions using key: %s\n", key));
    /*
    RC4 KSA! :-D
    */
    for (i = 0; i < 256; i++) {
        arr[i] = i;
    }
    j = 0;
    for (i = 0; i < 256; i++) {
        j = (j + arr[i] + key[i % keysize]) % 256;
        tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
    for (i = 0; i < NUM_OPS; i++) {
        INSTR[i].value = arr[i];
    }
#ifdef DBG
    DBG_INFO(("~~~~~~~~~~\nOPCODES:\n"));
    for (i = 0; i < NUM_OPS; i++) {
        DBG_INFO(("%s: 0x%x\n", INSTR[i].name, INSTR[i].value));
    }
    DBG_INFO(("~~~~~~~~~~\n"));
#endif
    return;
}

/*
DBG UTILS
*/

const char *VM::getRegName(uint8_t regvalue) {
#ifdef DBG
    switch (regvalue) {
    case R0:
      return "R0";
    case R1:
     return "R1";
    case R2:
     return "R2";
    case R3:
     return "R3";
    case S0:
     return "S0";
    case S1:
     return "S1";
    case S2:
     return "S2";
    case S3:
     return "S3";
    case IP:
     return "IP";
    case RP:
     return "RP";
    case SP:
     return "SP";
    default:
     return "??";
    }
#endif
    return "";
}

void VM::status(void) {
#ifdef DBG
    uint8_t i;
    DBG_SUCC(("VM Status:\n"));
    DBG_SUCC(("~~~~~~~~~~\n"));
    for (i = R0; i <= SP; i++) {
      DBG_INFO(("%s:\t0x%04x\n", getRegName(i), regs[i]));
    }
    DBG_INFO(("Flags: ZF = %d, CF = %d\n", flags.ZF, flags.CF));
    DBG_SUCC(("~~~~~~~~~~\n"));
#endif
    return;
}

/*
CONSTRUCTORS
*/
VM::VM(uint8_t *key) {
    DBG_SUCC(("Creating VM without code.\n"));
    initVariables();
    encryptOpcodes(key);
}

VM::VM(uint8_t *key, uint8_t *code, uint32_t codesize) {
    DBG_SUCC(("Creating VM with code.\n"));
    as.insCode(code, codesize);
    initVariables();
    encryptOpcodes(key);
}

void VM::initVariables(void) {
    uint8_t i;

    for (i = R0; i < NUM_REGS; i++) {
        this->regs[i] = 0;
    }
    return;
}

bool VM::isRegValid(uint8_t reg) {
    // invalid register
    if (reg < 0 || reg > NUM_REGS) {
        DBG_ERROR(("Unknown register: 0x%x.\n", reg));
        return false;
    }
    if (reg == IP || reg == SP || reg == RP) {
        DBG_ERROR(("Can't modify %s.\n", getRegName(reg)));
        return false;
    }
    return true;
}


/*
INSTRUCTIONS IMPLEMENTATION
*/

bool VM::execMOVI(void) {
    /*
    MOVI R0, 0x2400 | R0 = 0x2400
    */
    uint8_t dst;
    uint16_t src;
    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("MOVI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] = src;
    return true;
}

bool VM::execMOVR(void) {
    /*
    MOVR R1, R0 -> R1 = R0
    ---------------------
    R1, R0 = 0x10 <- DST / SRC are nibbles!
    */
    uint8_t dst, src;
    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("MOVR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] = regs[src];
    return true;
}

bool VM::execLODI(void) {
    /*
    LODI R0, 0x1000 -> R0 = data[0x1000]
    */
    uint8_t dst;
    uint16_t src;
    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("LODI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    if (src < 0 || src + sizeof(uint16_t) >= as.getDatasize()) {
        DBG_ERROR(("Out of bounds: trying to access to invalid data address.\n"));
        return false;
    }
    regs[dst] = *((uint16_t *) &as.getData()[src]);
    return true;
}

bool VM::execLODR(void) {
    /*
    LODR R1, R0 -> R1 = data[R0]
    */
    uint8_t dst;
    uint8_t src;
    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("LODR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    if (regs[src] < 0 || regs[src] + sizeof(uint16_t) >= as.getDatasize()) {
        DBG_ERROR(("Out of bounds: trying to access to invalid data address.\n"));
        return false;
    }
    regs[dst] = *((uint16_t *) &as.getData()[regs[src]]);
    return true;
}

bool VM::execSTRI(void) {
    /*
    STRI 0x1000, R0 -> data[0x1000] = R0
    */
    uint16_t dst;
    uint8_t src;
    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("STRI 0x%x, %s\n", dst, getRegName(src)));
    if (!isRegValid(dst)) {
        return false;
    }
    if (dst < 0 || dst + sizeof(uint16_t) >= as.getDatasize()) {
        DBG_ERROR(("Out of bounds: trying to access to invalid data address.\n"));
        return false;
    }
    *((uint16_t *) &as.getData()[dst]) = regs[src];
    return true;
}

bool VM::execSTRR(void) {
    /*
    STRR R1, R0 -> data[R1] = R0
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("STRR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    if (regs[dst] < 0 || regs[dst] + sizeof(uint16_t) >= as.getDatasize()) {
        DBG_ERROR(("Out of bounds: trying to access to invalid data address.\n"));
        return false;
    }
    *((uint16_t *) &as.getData()[regs[dst]]) = regs[src];
    return true;
}

bool VM::execADDI(void) {
    /*
    ADDI R0, 0x2 -> R0 += 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("ADDI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] += src;
    return true;
}

bool VM::execADDR(void) {
    /*
    ADDR R0, R1 -> R0 += R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("ADDR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] += regs[src];
    return true;
}

bool VM::execSUBI(void) {
    /*
    SUBI R0, 0x2 -> R0 -= 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SUBI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] -= src;
    return true;
}

bool VM::execSUBR(void) {
    /*
    SUBR R0, R1 -> R0 -= R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SUBR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] -= regs[src];
    return true;
}

bool VM::execANDB(void) {
    /*
    ANDB R0, 0x2 -> R0 &= 0x02 or R0 &= [BYTE] 0x02 (low byte)
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst, 1)) {
        return false;
    }
    DBG_INFO(("ANDB %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] &= src;
    return true;
}

bool VM::execANDW(void) {
    /*
    ANDW R0, 0x2 -> R0 &= 0x0002 or R0, ^= [WORD] 0x2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("ANDW %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] &= src;
    return true;
}

bool VM::execANDR(void) {
    /*
    ANDR R0, R1 -> R0 ^= R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("ANDR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] &= regs[src];
    return true;
}

bool VM::execYORB(void) {
    /*
    YORB R0, 0x2 -> R0 |= 0x02 or R0 |= [BYTE] 0x02 (low byte)
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst, 1)) {
        return false;
    }
    DBG_INFO(("YORB %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] |= src;
    return true;
}

bool VM::execYORW(void) {
    /*
    YORW R0, 0x2 -> R0 |= 0x0002 or R0, |= [WORD] 0x2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("YORW %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] |= src;
    return true;
}

bool VM::execYORR(void) {
    /*
    YORR R0, R1 -> R0 |= R1
    */
    uint8_t dst;
    uint8_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("YORR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] |= regs[src];
    return true;
}

bool VM::execXORB(void) {
    /*
    XORB R0, 0x2 -> R0 ^= 0x02 or R0 ^= [BYTE] 0x02 (low byte)
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst, 1)) {
        return false;
    }
    DBG_INFO(("XORB %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] ^= src;
    return true;
}

bool VM::execXORW(void) {
    /*
    XORW R0, 0x2 -> R0 ^= 0x0002 or R0, ^= [WORD] 0x2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("XORW %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] ^= src;
    return true;
}

bool VM::execXORR(void) {
    /*
    XORR R0, R1 -> R0 ^= R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("XORR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] ^= regs[src];
    return true;
}

bool VM::execNOTR(void) {
    /*
    NOTR R0, R1 -> R0 = ~R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("NOTR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] = ~regs[src];
    return true;
}

bool VM::execMULI(void) {
    /*
    MULI R0, 0x2 | R0 *= 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("MULI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] *= src;
    return true;
}

bool VM::execMULR(void) {
    /*
    MULR R0, R1 -> R0 *= R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("MULR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] *= regs[src];
    return true;
}

bool VM::execDIVI(void) {
    /*
    DIVI R0, 0x2 | R0 /= 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("DIVI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst) || !isDivArgValid<uint16_t>(src)) {
        return false;
    }
    regs[dst] /= src;
    return true;
}

bool VM::execDIVR(void) {
    /*
    DIVR R0, R1 -> R0 /= R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("DIVR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst) || !isDivArgValid<uint8_t>(regs[src])) {
        return false;
    }
    regs[dst] /= regs[src];
    return true;
}

bool VM::execSHLI(void) {
    /*
    SHLI R0, 0x2 | R0 << 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SHLI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] = regs[dst] << src;
    return true;
}

bool VM::execSHLR(void) {
    /*
    SHLR R0, R1 -> R0 << R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SHLR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] = regs[dst] << regs[src];
    return true;
}

bool VM::execSHRI(void) {
    /*
    SHRI R0, 0x2 | R0 >> 2
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SHRI %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    regs[dst] = regs[dst] >> src;
    return true;
}

bool VM::execSHRR(void) {
    /*
    SHRR R0, R1 -> R0 >> R1
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("SHRR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(src) || !isRegValid(dst)) {
        return false;
    }
    regs[dst] = regs[dst] >> regs[src];
    return true;
}

bool VM::execPUSH(void) {
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("PUSH %s\n", getRegName(reg)));
    if (!isRegValid(reg)) {
        return false;
    }
    if (regs[SP] + sizeof(uint16_t) >= as.getStacksize()) {
        DBG_ERROR(("Out of bounds: stack is going over the stack size!\n"));
        return false;
    }
    memcpy(&as.getStack()[regs[SP]], &regs[reg], sizeof(uint16_t));
    regs[SP] += sizeof(uint16_t);
    return true;
}

bool VM::execPOOP(void) {
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("POOP %s\n", getRegName(reg)));
    if (!isRegValid(reg)) {
        return false;
    }
    if (regs[SP] - sizeof(uint16_t) < 0) {
        DBG_ERROR(("Out of bounds: stack is going below 0!\n"));
        return false;
    }
    regs[SP] -= sizeof(uint16_t);
    memcpy(&regs[reg], &as.getStack()[regs[SP]], sizeof(uint16_t));
    return true;
}

bool VM::execCMPB(void) {
    /*
    CMPB R0, 0x2 -> Compare immediate with lower half (BYTE) register
    */
    uint8_t dst, src;

    if (!as.getArgs(regs[IP], &src, &dst, 1)) {
        return false;
    }
    DBG_INFO(("CMPB %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    if (*((uint8_t *) &regs[dst]) == src) {
        flags.ZF = 1;
    } else {
        flags.ZF = 0;
    }
    if (*((uint8_t *) &regs[dst]) > src) {
        flags.CF = 0;
    } else {
        flags.CF = 1;
    }
    return true;
}

bool VM::execCMPW(void) {
    /*
    CMPW R0, 0x2 -> Compare immediate with whole (WORD) register
    */
    uint8_t dst;
    uint16_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("CMPW %s, 0x%x\n", getRegName(dst), src));
    if (!isRegValid(dst)) {
        return false;
    }
    if (regs[dst] == src) {
        flags.ZF = 1;
    } else {
        flags.ZF = 0;
    }
    if (regs[dst] > src) {
        flags.CF = 0;
    } else {
        flags.CF = 1;
    }
    return true;
}

bool VM::execCMPR(void) {
    /*
    CMPR R0, R1 -> Compares 2 registers
    */
    uint8_t dst;
    uint8_t src;

    if (!as.getArgs(regs[IP], &src, &dst)) {
        return false;
    }
    DBG_INFO(("CMPR %s, %s\n", getRegName(dst), getRegName(src)));
    if (!isRegValid(dst) || !isRegValid(src)) {
        return false;
    }
    if (regs[dst] == regs[src]) {
        flags.ZF = 1;
    } else {
        flags.ZF = 0;
    }
    if (regs[dst] > regs[src]) {
        flags.CF = 0;
    } else {
        flags.CF = 1;
    }
    return true;
}

bool VM::execJMPI(void) {
    /*
    JMPI 0x2000 -> IP = 0x2000
    */
    uint16_t imm;

    if (!as.getArgs(regs[IP], &imm)) {
        return false;
    }
    DBG_INFO(("JMPI 0x%x\n", imm));
    regs[IP] = imm;
    return true;
}

bool VM::execJMPR(void) {
    /*
    JMPR R0 -> IP = R0
    */
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("JMPR %s = 0x%x\n", getRegName(reg), regs[reg]));
    if (!isRegValid(reg)) {
        return false;
    }
    regs[IP] = regs[reg];
    return true;
}

bool VM::execJPAI(void) {
    /*
    JPAI 0x2000 -> Jump to 0x2000 if above
    */
    uint16_t imm;

    if (!as.getArgs(regs[IP], &imm)) {
        return false;
    }
    DBG_INFO(("JPAI 0x%x\n", imm));
    if (flags.CF == 0 && flags.ZF == 0) {
        regs[IP] = imm;
    } else {
        regs[IP] += JPAI_SIZE;
    }
    return true;
}

bool VM::execJPAR(void) {
    /*
    JPAR R0 -> Jump to [R0] if above
    */
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("JPAR %s = 0x%x\n", getRegName(reg), regs[reg]));
    if (!isRegValid(reg)) {
        return false;
    }
    if (flags.CF == 0 && flags.ZF == 0) {
        regs[IP] = reg;
    } else {
        regs[IP] += JPAR_SIZE;
    }
    return true;
}

bool VM::execJPBI(void) {
    /*
    JPBI 0x2000 -> Jump to 0x2000 if below
    */
    uint16_t imm;

    if (!as.getArgs(regs[IP], &imm)) {
        return false;
    }
    DBG_INFO(("JPBI 0x%x\n", imm));
    if (flags.CF == 1) {
        regs[IP] = imm;
    } else {
        regs[IP] += JPBI_SIZE;
    }
    return true;
}

bool VM::execJPBR(void) {
    /*
    JPBR R0 -> Jump to [R0] if below
    */
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("JPBR %s = 0x%x\n", getRegName(reg), regs[reg]));
    if (!isRegValid(reg)) {
        return false;
    }
    if (flags.CF == 1) {
        regs[IP] = reg;
    } else {
        regs[IP] += JPBR_SIZE;
    }
    return true;
}

bool VM::execJPEI(void) {
    /*
    JPEI 0x2000 -> Jump to 0x2000 if equal
    */
    uint16_t imm;

    if (!as.getArgs(regs[IP], &imm)) {
        return false;
    }
    DBG_INFO(("JPEI 0x%x\n", imm));
    if (flags.ZF == 1) {
        regs[IP] = imm;
    } else {
        regs[IP] += JPEI_SIZE;
    }
    return true;
}

bool VM::execJPER(void) {
    /*
    JPNR R0 -> Jump to [R0] if equal
    */
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("JPER %s = 0x%x\n", getRegName(reg), regs[reg]));
    if (!isRegValid(reg)) {
        return false;
    }
    if (flags.ZF == 1) {
        regs[IP] = reg;
    } else {
        regs[IP] += JPER_SIZE;
    }
    return true;
}

bool VM::execJPNI(void) {
    /*
    JPEI 0x2000 -> Jump to 0x2000 if not equal
    */
    uint16_t imm;

    if (!as.getArgs(regs[IP], &imm)) {
        return false;
    }
    DBG_INFO(("JPNI 0x%x\n", imm));
    if (flags.ZF == 0) {
        regs[IP] = imm;
    } else {
        regs[IP] += JPNI_SIZE;
    }
    return true;
}

bool VM::execJPNR(void) {
    /*
    JPER R0 -> Jump to [R0] if not equal
    */
    uint8_t reg;

    if (!as.getArgs(regs[IP], &reg)) {
        return false;
    }
    DBG_INFO(("JPNR %s = 0x%x\n", getRegName(reg), regs[reg]));
    if (!isRegValid(reg)) {
        return false;
    }
    if (flags.ZF == 0) {
        regs[IP] = reg;
    } else {
        regs[IP] += JPNR_SIZE;
    }
    return true;
}

bool VM::execCALL(void) {
    /*
    CALL 0x1000 -> Jump to data[0x1000] and saves the RP onto the stack
    */
    uint16_t dst;

    if (!as.getArgs(regs[IP], &dst)) {
        return false;
    }
    DBG_INFO(("CALL 0x%x\n", dst));
    if (regs[SP] + sizeof(uint16_t) >= as.getStacksize()) {
        DBG_ERROR(("Out of bounds: stack is going over the stack size!\n"));
        return false;
    }
    if (regs[IP] + 1 + sizeof(dst) >= as.getCodesize()) {
        DBG_ERROR(("Out of bounds: trying to read over codesize.\n"));
        return false;
    }
    regs[RP] = regs[IP] + 1 + sizeof(dst);
    *((uint16_t *) &as.getStack()[regs[SP]]) = regs[RP];
    regs[SP] += sizeof(uint16_t);
    regs[IP] = dst;
    return true;
}

bool VM::execRETN(void) {
    /*
    RETN -> IP = RP , restores saved return IP
    */
    if (regs[SP] - sizeof(uint16_t) < 0) {
        DBG_ERROR(("Out of bounds: stack is going below 0!\n"));
        return false;
    }
    regs[SP] -= sizeof(uint16_t);
    DBG_INFO(("RETN 0x%x\n", regs[RP]));
    regs[IP] = regs[RP];
    return true;
}

bool VM::execGRMN(void) {
    uint8_t i;
    for (i = 0; i < NUM_REGS; i++) {
        if (i != IP && i != RP && i != SP) {
            regs[i] = 0x4747;
        }
    }
    return true;
}

bool VM::execSHIT(void) {
    DBG_INFO(("SHIT\n"));
    return false;
}

bool VM::execNOPE(void) {
    return true;
}

bool VM::execDEBG(void) {
    status();
    return true;
}

void VM::run(void) {
    uint8_t next_instr, i;
    instruction_t *instr_p = NULL;
    bool success;
    bool finished = false;
    while (!finished) {
        next_instr = (uint8_t) as.getCode()[regs[IP]];

        // getting pointer to correct instruction_t
        for (i = 0; i < NUM_OPS; i++) {
            if (next_instr == INSTR[i].value) {
                instr_p = &INSTR[i];
            }
        }

        if (instr_p == NULL) {
            DBG_ERROR(("WAT: 0x%x", next_instr));
            finished = true;
        } else {
            /*
             * Eye bleeding ahead
             */
            success = (this->*(instr_p->exec))();

            if (!success) {
                DBG_ERROR(("%s failed.\n", instr_p->name));
                finished = true;
            } else {
                if (!instr_p->isJump) {
                    regs[IP] += instr_p->length;
                }
            }
        }
    }
    DBG_INFO(("Finished.\n"));
    return;
}

VMAddrSpace *VM::addressSpace() {
    return &as;
}

uint16_t VM::reg(uint8_t reg) {
    if (reg < 0 || reg >= NUM_REGS) {
        throw std::invalid_argument("Invalid register");
    }
    return regs[reg];
}
