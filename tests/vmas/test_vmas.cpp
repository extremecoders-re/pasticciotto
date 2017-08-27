#include "../include/catch.hpp"
#include "../../vm/vm.h"
#include <cstring>

TEST_CASE("VMAddrSpace initialization", "[VMAS]") {
    uint32_t i;
    uint8_t code_test[] = "testing stuff... code!";
    uint32_t code_len = strlen((const char *) code_test);
    uint8_t stack_test[] = "testing stuff... stack!";
    uint32_t stack_len = strlen((const char *) stack_test);
    uint8_t data_test[] = "testing stuff... data!";
    uint32_t data_len = strlen((const char *) data_test);


/*
 * DEFAULT SIZE
 */
    VMAddrSpace vmas_def;

    REQUIRE(vmas_def.getCodesize() == DEFAULT_CODESIZE);
    REQUIRE(vmas_def.getDatasize() == DEFAULT_DATASIZE);
    REQUIRE(vmas_def.getStacksize() == DEFAULT_STACKSIZE);
    REQUIRE(vmas_def.getCode() != NULL);
    REQUIRE(vmas_def.getStack() != NULL);
    REQUIRE(vmas_def.getData() != NULL);

// Code has to be empty
    for (i = 0; i < DEFAULT_CODESIZE; i++) {
        REQUIRE(vmas_def.getCode()[i] == 0);
    }
// Data has to be empty
    for (i = 0; i < DEFAULT_DATASIZE; i++) {
        REQUIRE(vmas_def.getData()[i] == 0);
    }
// Stack has to be empty
    for (i = 0; i < DEFAULT_STACKSIZE; i++) {
        REQUIRE(vmas_def.getStack()[i] == 0);
    }

    REQUIRE(vmas_def.insCode(code_test, DEFAULT_CODESIZE + 5) == false);
    REQUIRE(vmas_def.insCode(code_test, code_len) == true);
// Code has to match
    for (i = 0; i < DEFAULT_CODESIZE; i++) {
        if (i < code_len) {
            REQUIRE(vmas_def.getCode()[i] == code_test[i]);
        } else {
            REQUIRE(vmas_def.getCode()[i] == 0);
        }
    }
    REQUIRE(vmas_def.insData(data_test, DEFAULT_DATASIZE + 5) == false);
    REQUIRE(vmas_def.insData(data_test, data_len) == true);
    // Data has to match
    for (i = 0; i < DEFAULT_DATASIZE; i++) {
        if (i < data_len) {
            REQUIRE(vmas_def.getData()[i] == data_test[i]);
        } else {
            REQUIRE(vmas_def.getData()[i] == 0);
        }
    }
    REQUIRE(vmas_def.insStack(stack_test, DEFAULT_STACKSIZE + 5) == false);
    REQUIRE(vmas_def.insStack(stack_test, stack_len) == true);
// Stack has to match
    for (i = 0; i < DEFAULT_STACKSIZE; i++) {
        if (i < stack_len) {
            REQUIRE(vmas_def.getStack()[i] == stack_test[i]);
        } else {
            REQUIRE(vmas_def.getStack()[i] == 0);
        }
    }
/*
 * CUSTOM SIZE
 */

    uint32_t cs = 0x1234, ss = 0x4321, ds = 0x4657;
    VMAddrSpace vmas_cus(ss, cs, ds);

    REQUIRE(vmas_cus.getCodesize() == cs);
    REQUIRE(vmas_cus.getDatasize() == ds);
    REQUIRE(vmas_cus.getStacksize() == ss);
    REQUIRE(vmas_cus.getCode() != NULL);
    REQUIRE(vmas_cus.getStack() != NULL);
    REQUIRE(vmas_cus.getData() != NULL);

// Code has to be empty
    for (i = 0; i < cs; i++) {
        REQUIRE(vmas_cus.getCode()[i] == 0);
    }
// Data has to be empty
    for (i = 0; i < ds; i++) {
        REQUIRE(vmas_cus.getData()[i] == 0);
    }
// Stack has to be empty
    for (i = 0; i < ss; i++) {
        REQUIRE(vmas_cus.getStack()[i] == 0);
    }

    // Wrong size
    REQUIRE(vmas_cus.insCode(code_test, cs + 5) == false);
    REQUIRE(vmas_cus.insCode(code_test, code_len) == true);
// Code has to match
    for (i = 0; i < cs; i++) {
        if (i < code_len) {
            REQUIRE(vmas_cus.getCode()[i] == code_test[i]);
        } else {
            REQUIRE(vmas_cus.getCode()[i] == 0);
        }
    }
    REQUIRE(vmas_cus.insData(data_test, ds + 5) == false);
    REQUIRE(vmas_cus.insData(data_test, data_len) == true);
// Data has to match
    for (i = 0; i < ds; i++) {
        if (i < data_len) {
            REQUIRE(vmas_cus.getData()[i] == data_test[i]);
        } else {
            REQUIRE(vmas_cus.getData()[i] == 0);
        }
    }
    REQUIRE(vmas_cus.insStack(stack_test, ss + 5) == false);
    REQUIRE(vmas_cus.insStack(stack_test, stack_len) == true);
// Stack has to match
    for (i = 0; i < ss; i++) {
        if (i < stack_len) {
            REQUIRE(vmas_cus.getStack()[i] == stack_test[i]);
        } else {
            REQUIRE(vmas_cus.getStack()[i] == 0);
        }
    }

}

TEST_CASE("Getting operands from VMAddrSpace", "[VMAS]") {
    uint8_t dst8, src8;
    uint16_t dst16, src16;
    uint8_t code_test[] = "OR OIIR ORII ORB OR OII";
    // offset     "01234567890123456789012"
    uint32_t code_len = strlen((const char *) code_test);
    VMAddrSpace vmas;

    vmas.insCode(code_test, code_len);

    /*
     * REGISTER TO REGISTER
     */

    vmas.getArgs(0, &src8, &dst8);
    REQUIRE(dst8 == 0x5); // R is 0x52
    REQUIRE(src8 == 0x2);


    /*
     * REGISTER TO IMMEDIATE
     */

    vmas.getArgs(3, &src8, &dst16);
    REQUIRE(dst16 == 0x4949); // I is 0x49
    REQUIRE(src8 == 0x52); // R is 0x52

    /*
     * IMMEDIATE TO REGISTER
     */

    vmas.getArgs(8, &src16, &dst8);
    REQUIRE(dst8 == 0x52); // I is 0x49
    REQUIRE(src16 == 0x4949); // R is 0x52

    /*
     * BYTE TO REGISTER
     */
    vmas.getArgs(13, &src8, &dst8, 1);
    REQUIRE(dst8 == 0x52); // R is 0x52
    REQUIRE(src8 == 0x42); // B is 0x42

    /*
     * OP ONLY
     */
    vmas.getArgs(17, &dst8);
    REQUIRE(dst8 == 0x52);

    vmas.getArgs(20, &dst16);
    REQUIRE(dst16 == 0x4949);

}


