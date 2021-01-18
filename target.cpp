
#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')

const string labelMain_old = "main";
const int frameHeadOffset_old = 8;
const int maxLabelCount_old = 10000;

fstream mipsFile_old;
int infixIndex_old = 0;
int stringIndex_old = 0;
int funcIndex_old = 0;
string currentFunc_old;
std::vector<string> stringLabels_old;
string mipsCodes_old = "";
int globalAddr_old = 0;


inline bool isGlobal_old(int index) {
    return (staticTable[index].level == 0);
}

inline int getGlobalOffset_old(string varName) {
    return (staticTable[lookUp(varName.c_str())].addr * 4);
}

inline int getLocalOffset_old(string varName) {
    return ((-1) * (frameHeadOffset_old +
                    staticTable[lookUpStatic(currentFunc_old.c_str(), varName.c_str())].addr * 4));
}

inline void insertCode_old(string code) {
    mipsCodes_old = mipsCodes_old + code + "\n";
}

inline bool isIdentifier_old(string name) {
    if (isLetter(name[0]) || name[0] == '#') {
        return true;
    }
    return false;
}


void firstOperandToRegister_old(string operand1) {
    int staticIndex = lookUpStatic(currentFunc_old.c_str(), operand1.c_str());

    if (!isIdentifier_old(operand1)) {

        insertCode_old("li $t1, " + toString(operand1));
    } else if (isIdentifier_old(operand1) && staticTable[staticIndex].cls == consts) {

        insertCode_old("li $t1, " + toString(staticTable[staticIndex].addr));
    } else {
        int op1Offset;
        if (!isGlobal_old(staticIndex)) { 
            op1Offset = getLocalOffset_old(operand1);
            insertCode_old("lw $t1, " + toString(op1Offset) + "($fp)");
        } else { 
            op1Offset = getGlobalOffset_old(operand1);
            insertCode_old("lw $t1, " + toString(op1Offset) + "($gp)");
        }
    }
}

void secondOperandToRegister_old(string operand2) {
    int staticIndex = lookUpStatic(currentFunc_old.c_str(), operand2.c_str());

    if (!isIdentifier_old(operand2)) {

        insertCode_old("li $t2, " + toString(operand2));
    } else if (isIdentifier_old(operand2) && staticTable[staticIndex].cls == consts) {
    
        insertCode_old("li $t2, " + toString(staticTable[staticIndex].addr));
    } else {
        int op2Offset;
        if (staticTable[staticIndex].level != 0) {
            op2Offset = getLocalOffset_old(operand2);
            insertCode_old("lw $t2, " + toString(op2Offset) + "($fp)");
        } else {
            op2Offset = getGlobalOffset_old(operand2);
            insertCode_old("lw $t2, " + toString(op2Offset) + "($gp)");
        }
    }
}

void operandsToRegister_old(infixNotation notation) {
    firstOperandToRegister_old(notation.operand1);
    secondOperandToRegister_old(notation.operand2);
}

void thirdOperandToRegister_old(string operand3) {
    int staticIndex = lookUpStatic(currentFunc_old.c_str(), operand3.c_str());

    if (!isIdentifier_old(operand3)) {
        insertCode_old("li $t0, " + toString(operand3));
    } else if (isIdentifier_old(operand3) && staticTable[staticIndex].cls == consts) {

        insertCode_old("li $t0, " + toString(staticTable[staticIndex].addr));
    } else { 
        int op3Offset;
        if (staticTable[staticIndex].level != 0) {
            op3Offset = getLocalOffset_old(operand3);
            insertCode_old("lw $t0, " + toString(op3Offset) + "($fp)");
        } else {
            op3Offset = getGlobalOffset_old(operand3);
            insertCode_old("lw $t0, " + toString(op3Offset) + "($gp)");
        }
    }
}


void storeThirdOperand_old(string operand3, string registerName) {
    int staticIndex3 = lookUpStatic(currentFunc_old.c_str(), operand3.c_str());
    int op3Offset;

    if (!isGlobal_old(staticIndex3)) {
        op3Offset = getLocalOffset_old(operand3);
    } else {
        op3Offset = getGlobalOffset_old(operand3);
    }
    string rootRegister = isGlobal_old(staticIndex3) ? "($gp)" : "($fp)";
    insertCode_old("sw " + registerName + ", " + toString(op3Offset) + rootRegister); 
}

void pushGlobalStack_old() {
    ++globalAddr_old;
}

void pushGlobalStack_old(int count) {
    for (int i = 0; i < count; ++i) {
        ++globalAddr_old;
    }
}

void pushLocalStack_old() {
    insertCode_old("sub $sp, $sp, 4");
}

void pushLocalStack_old(int count) {
    insertCode_old("sub $sp, $sp, " + toString(4 * count));
}

void mipsVarDef_old(bool isGlobal) {
    if (isGlobal) { 
        for (int i = 0; i < staticTable.size() && staticTable[i].cls != funcs; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) { 
                    pushGlobalStack_old();
                } else { 
                    pushGlobalStack_old(staticTable[i].length);
                }
            }
        }
    } else {
        int startIndex = lookUpStatic(currentFunc_old.c_str());
        for (int i = startIndex + 1; i < staticTable.size() && staticTable[i].cls != funcs ; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) { 
                    pushLocalStack_old();
                } else { 
                    pushLocalStack_old(staticTable[i].length);
                }
            }
        }
    }
}

void funcHead_old() {
    int paramCount = staticTable[lookUpStatic(currentFunc_old.c_str())].length;
    int fpOffset = frameHeadOffset_old + paramCount * 4;
    int raOffset = fpOffset - 4;

    insertCode_old("sw $fp," + toString(fpOffset) + "($sp)"); 
    insertCode_old("add $fp, $sp," + toString(fpOffset)); 
    insertCode_old("sw $ra," + toString(raOffset) + "($sp)"); 

    for (; infixTable[infixIndex_old].ioperator == "PARAM"; ++infixIndex_old);
}

void funcTail_old() {
    insertCode_old("lw $ra, -4($fp)");
    insertCode_old("move $sp, $fp");
    insertCode_old("lw $fp, ($sp)");
    insertCode_old("jr $ra\n");
}

void funcContent_old() {
    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator == "CONST"; ++infixIndex_old); // Jump all constant definitions
    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator == "VAR" ; ++infixIndex_old); // Skip all variable definitions
    mipsVarDef_old(false); 
    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator != "FUNC" ; ++infixIndex_old) {

        infixNotation notation = infixTable[infixIndex_old];
        int staticIndex1 = lookUpStatic(currentFunc_old.c_str(), notation.operand1.c_str());
        int staticIndex2 = lookUpStatic(currentFunc_old.c_str(), notation.operand2.c_str());
        int staticIndex3 = lookUpStatic(currentFunc_old.c_str(), notation.operand3.c_str());

        if (notation.ioperator == "ADD") {
            operandsToRegister_old(notation);
            insertCode_old("add $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "SUB") {
            operandsToRegister_old(notation);
            insertCode_old("sub $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "MUL") {
            operandsToRegister_old(notation);
            insertCode_old("mul $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "DIV") {
            operandsToRegister_old(notation);
            insertCode_old("div $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "ASSIGN") {
            if (notation.operand2 == "#RET") { 
                insertCode_old("move $t0, $v1");
                storeThirdOperand_old(notation.operand3, "$t0");
            } else { 
                if (!isIdentifier_old(notation.operand2)) { 
                    insertCode_old("li $t0, " + toString(notation.operand2));
                } else if (isIdentifier_old(notation.operand2) && staticTable[staticIndex2].cls != consts) {

                    int op2Offset;
                    if (staticTable[staticIndex2].level != 0) { 
                        op2Offset = getLocalOffset_old(notation.operand2);
                        insertCode_old("lw $t0, " + toString(op2Offset) + "($fp)");
                    } else { 
                        op2Offset = getGlobalOffset_old(notation.operand2);
                        insertCode_old("lw $t0, " + toString(op2Offset) + "($gp)");
                    }
                } else if (isIdentifier_old(notation.operand2) && staticTable[staticIndex2].cls == consts) {
                   
                    insertCode_old("li $t0, " + toString(staticTable[staticIndex2].addr));
                }
                storeThirdOperand_old(notation.operand3, "$t0");
            }
        } else if (notation.ioperator == "NEG") {
            if (!isIdentifier_old(notation.operand2)) {
                insertCode_old("li $t0, " + toString(notation.operand2));
                insertCode_old("sub $t0, $0, $t0");
            } else if (isIdentifier_old(notation.operand2) && staticTable[staticIndex2].cls != consts) {
              
                int op2Offset;
                if (staticTable[staticIndex2].level != 0) {
                    op2Offset = getLocalOffset_old(notation.operand2);
                    insertCode_old("lw $t0, " + toString(op2Offset) + "($fp)");
                } else { 
                    op2Offset = getGlobalOffset_old(notation.operand2);
                    insertCode_old("lw $t0, " + toString(op2Offset) + "($gp)");
                }
                insertCode_old("sub $t0, $0, $t0");
            } else if (isIdentifier_old(notation.operand2) && staticTable[staticIndex2].cls == consts) {
                
                insertCode_old("li $t0, " + toString(staticTable[staticIndex2].addr));
            }
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "EQL") {
            operandsToRegister_old(notation);
            insertCode_old("seq $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "NEQ") {
            operandsToRegister_old(notation);
            insertCode_old("sne $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "LSS") {
            operandsToRegister_old(notation);
            insertCode_old("slt $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "LEQ") {
            operandsToRegister_old(notation);
            insertCode_old("sle $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "GTR") {
            operandsToRegister_old(notation);
            insertCode_old("sgt $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "GEQ") {
            operandsToRegister_old(notation);
            insertCode_old("sge $t0, $t1, $t2");
            storeThirdOperand_old(notation.operand3, "$t0");
        } else if (notation.ioperator == "RETURN") {
            if (notation.operand3 != " ") { 
                if (staticIndex3 == -1) { 
                    insertCode_old("li $t0, " + notation.operand3);
                } else if (staticTable[staticIndex3].cls == consts) {
                    insertCode_old("li $t0, " + toString(staticTable[staticIndex3].addr));
                } else { 
                    if (!isGlobal_old(staticIndex3)) {
                        int op3Offset = getLocalOffset_old(notation.operand3);
                        insertCode_old("lw $t0, " + toString(op3Offset) + "($fp)");
                    } else {
                        int op3Offset = getGlobalOffset_old(notation.operand3);
                        insertCode_old("lw $t0, " + toString(op3Offset) + "($gp)");
                    }
                }
            }
            insertCode_old("move $v1, $t0");
            insertCode_old("j " + currentFunc_old + "Tail"); 
        } else if (notation.ioperator == "PUSH") { 
            insertCode_old("sub $sp, $sp, " + toString(frameHeadOffset_old)); 
            for (; infixTable[infixIndex_old].ioperator == "PUSH"; ++infixIndex_old) {
                thirdOperandToRegister_old(infixTable[infixIndex_old].operand3); 
                insertCode_old("sw $t0, ($sp)");
                insertCode_old("sub $sp, $sp, 4");
            }
            insertCode_old("jal " + infixTable[infixIndex_old].operand3); 
        } else if (notation.ioperator == "CALL") { 
            insertCode_old("sub $sp, $sp, " + toString(frameHeadOffset_old)); 
            insertCode_old("jal " + infixTable[infixIndex_old].operand3); 
        } else if (notation.ioperator == "SETARR") {
            int op3Offset;

            operandsToRegister_old(notation); 
            if (!isGlobal_old(staticIndex3)) { 
                insertCode_old("mul $t2, $t2, -4"); 
                op3Offset = getLocalOffset_old(notation.operand3); 
                insertCode_old("add $t3, $fp, " + toString(op3Offset)); 
            } else { 
                insertCode_old("mul $t2, $t2, 4"); 
                op3Offset = getGlobalOffset_old(notation.operand3);
                insertCode_old("add $t3, $gp, " + toString(op3Offset));
            }
            insertCode_old("add $t2, $t2, $t3");
            insertCode_old("sw $t1, ($t2)"); 
        } else if (notation.ioperator == "GETARR") {
            int op1Offset;

            secondOperandToRegister_old(notation.operand2); 
            if (!isGlobal_old(staticIndex1)) { 
                insertCode_old("mul $t2, $t2, -4"); 
                op1Offset = getLocalOffset_old(notation.operand1);
                insertCode_old("add $t3, $fp, " + toString(op1Offset)); 
            } else { 
                insertCode_old("mul $t2, $t2, 4"); 
                op1Offset = getGlobalOffset_old(notation.operand1);
                insertCode_old("add $t3, $gp, " + toString(op1Offset));
            }
            insertCode_old("add $t2, $t2, $// Merge offset
            insertCode_old("lw $t1, ($// Get value from array
            storeThirdOperand_old(notation.operand3, "$t1");
        } else if (notation.ioperator == "JMP") {
            insertCode_old("j " + notation.operand3);
        } else if (notation.ioperator == "BEQ") {
            operandsToRegister_old(notation);
            insertCode_old("beq $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "BNE") {
            operandsToRegister_old(notation);
            insertCode_old("bne $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "SCANF") {
            int operand3Offset = getLocalOffset_old(notation.operand3);
            if (staticTable[staticIndex3].typ == chars) { 
                insertCode_old("li $v0, 12");
                insertCode_old("syscall");
                storeThirdOperand_old(notation.operand3, "$v0");
            } else { 
                insertCode_old("li $v0, 5");
                insertCode_old("syscall");
                storeThirdOperand_old(notation.operand3, "$v0");
            }
        } else if (notation.ioperator == "PRINTF") {
            if (notation.operand2 != " ") { 
                insertCode_old("la $a0, " + stringLabels_old[stringIndex_old]);
                insertCode_old("li $v0, 4");
                insertCode_old("syscall");
                ++stringIndex_old;
            }
            if (notation.operand3 != " ") { 
                int operand3Offset;
                if (staticTable[staticIndex3].cls == consts) { 
                    insertCode_old("li $a0, " + toString(staticTable[staticIndex3].addr)); 
                } else { 
                    if (!isGlobal_old(staticIndex3)) {
                        operand3Offset = getLocalOffset_old(notation.operand3);
                        insertCode_old("lw $a0, " + toString(operand3Offset) + "($fp)"); 
                    } else { 
                        operand3Offset = getGlobalOffset_old(notation.operand3);
                        insertCode_old("lw $a0, " + toString(operand3Offset) + "($gp)");
                    }
                }
                if (staticTable[staticIndex3].typ == chars) { 
                    insertCode_old("li $v0, 11");
                } else { 
                    insertCode_old("li $v0, 1");
                }
                insertCode_old("syscall");
            }
        } else if (notation.ioperator == "LABEL") {
            insertCode_old(notation.operand3 + ":");
        }
    }
}

void mipsFuncDef_old() {
    currentFunc_old = infixTable[infixIndex_old].operand3;
    insertCode_old(currentFunc_old + ":"); 
    ++infixIndex_old;
    funcHead_old();
    funcContent_old();
    insertCode_old(currentFunc_old + "Tail:"); 
    funcTail_old();
}

void mainHead_old() {
    insertCode_old("sw $fp, ($sp)");
    insertCode_old("move $fp, $sp");
    insertCode_old("sub $sp, $sp, 8");
}

void mipsMainDef_old() {
    currentFunc_old = "main";
    insertCode_old("main:"); 
    ++infixIndex_old;
    mainHead_old();
    funcContent_old();
    insertCode_old("mainTail:");
}

void mipsJ_old(string label) {
    insertCode_old("j " + label);
}

inline void outputMipsCodes_old() {
    mipsFile_old << mipsCodes_old;
}

void mipsProgram(string mipsFileName) {
    mipsFile_old.open(mipsFileName.c_str(), ios::out);

    stringLabels_old.reserve(maxLabelCount_old);

    insertCode_old(".data");
    for (int i = 0, count = 0; i < infixTable.size(); ++i) {
        if (infixTable[i].ioperator == "PRINTF" && infixTable[i].operand2 != " ") {
            insertCode_old("strLabel" + toString(count) + ":");
            insertCode_old(".asciiz");
            insertCode_old(infixTable[i].operand2);
            stringLabels_old.push_back("strLabel" + toString(count));
            count++;
        }
    }
    insertCode_old(""); 

    insertCode_old(".text");

    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator == "CONST"; ++infixIndex_old);
    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator == "VAR" ; ++infixIndex_old) {
    }
    mipsVarDef_old(true); 
    mipsJ_old(labelMain_old); 
    insertCode_old("");
    for (; infixIndex_old < infixTable.size() && infixTable[infixIndex_old].ioperator == "FUNC"
           && infixTable[infixIndex_old].operand3 != "main";) {
        mipsFuncDef_old();
    }
    mipsMainDef_old();
    outputMipsCodes_old();
}
