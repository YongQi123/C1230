
#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')
#define isNumber(a) (a >= '0' && a <= '9')

const string labelMain = "main";
const int frameHeadOffset = 8;
const int maxLabelCount = 10000;
const int globalRegCount = 8;

fstream mipsFile;
int infixIndex = 0;
int stringIndex = 0;
int funcIndex = 0;
string currentFunc;
std::vector<string> stringLabels;
string mipsCodes = "";
int globalAddr = 0;
std::vector<string> sortResult;
struct varCountNode {
    string name;
    int count;
};


inline bool isGlobal(int index) {
    return (staticTable[index].level == 0);
}

inline int getGlobalOffset(string varName) {
    return (staticTable[lookUp(varName.c_str())].addr * 4);
}

inline int getLocalOffset(string varName) {
    return ((-1) * (frameHeadOffset +
                    staticTable[lookUpStatic(currentFunc.c_str(), varName.c_str())].addr * 4));
}

inline void insertCode(string code) {
    mipsCodes = mipsCodes + code + "\n";
}

inline bool isIdentifier(string name) {
    if (isLetter(name[0]) || name[0] == '#') {
        return true;
    }
    return false;
}

bool compareAlgo(varCountNode& left, varCountNode& right) {
    return left.count > right.count;
}

void sortVariables() {
    sortResult.clear();
    sortResult.resize(globalRegCount);
    map<string, int> varCountTable;

    int staticIndex = lookUpStatic(infixTable[infixIndex - 1].operand3.c_str());
    staticIndex++;
    for (; staticIndex < staticTable.size() && staticTable[staticIndex].cls != funcs; ++staticIndex) { 
        if ((staticTable[staticIndex].cls == vars && staticTable[staticIndex].length != 0) ||
                staticTable[staticIndex].cls == params ||
            staticTable[staticIndex].cls == consts) { 
            continue;
        }
        string varName = staticTable[staticIndex].name;
        int count = 0;
        varCountTable.insert(std::pair<string, int>(varName, count));
    }
    for (int i = infixIndex; i < infixTable.size() && infixTable[i].ioperator != "FUNC"; ++i) {
        infixNotation notation = infixTable[i];
        string operand1 = notation.operand1;
        string operand2 = notation.operand2;
        string operand3 = notation.operand3;
        map<string, int>::iterator opIt;

        opIt = varCountTable.find(operand1);
        if (opIt != varCountTable.end()) {
            opIt->second = opIt->second + 1;
        }

        opIt = varCountTable.find(operand2);
        if (opIt != varCountTable.end()) { 
            opIt->second = opIt->second + 1;
        }

        opIt = varCountTable.find(operand3);
        if (opIt != varCountTable.end()) { 
            opIt->second = opIt->second + 1;
        }
    }

    map<string, int>::iterator varIt;
    std::vector<varCountNode> tempTable;
    for (varIt = varCountTable.begin(); varIt != varCountTable.end() ; ++varIt) {
        varCountNode node;
        node.name = varIt->first;
        node.count = varIt->second;
        tempTable.push_back(node);
    }
    std::sort(tempTable.begin(), tempTable.end(), compareAlgo);
    int varCount = tempTable.size();
    for (int k = 0; k < sortResult.size() && k < varCount; ++k) { 
        sortResult[k] = tempTable[k].name;
    }
    for (int j = varCount; j < sortResult.size(); ++j) { 
        sortResult[j] = "";
    }
}


void loadFirstOperand(string operand1) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand1.c_str());

    if (!isIdentifier(operand1)) {
       
        insertCode("li $t1, " + toString(operand1));
    } else if (isIdentifier(operand1) && staticTable[staticIndex].cls == consts) {
        
        insertCode("li $t1, " + toString(staticTable[staticIndex].addr));
    } else {
        int op1Offset;
        if (!isGlobal(staticIndex)) { 

            for (int i = 0; i < sortResult.size(); ++i) {
                if (operand1 == sortResult[i]) {
                    string fromReg = "$s" + toString(i);
                    insertCode("move $t1, " + fromReg);
                    return;
                }
            }
            op1Offset = getLocalOffset(operand1);
            insertCode("lw $t1, " + toString(op1Offset) + "($fp)");
        } else { 
            op1Offset = getGlobalOffset(operand1);
            insertCode("lw $t1, " + toString(op1Offset) + "($gp)");
        }
    }
}

void loadSecondOperand(string operand2) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand2.c_str());

    if (!isIdentifier(operand2)) {
        
        insertCode("li $t2, " + toString(operand2));
    } else if (isIdentifier(operand2) && staticTable[staticIndex].cls == consts) {
        
        insertCode("li $t2, " + toString(staticTable[staticIndex].addr));
    } else {
        int op2Offset;
        if (staticTable[staticIndex].level != 0) {
           
            for (int i = 0; i < sortResult.size(); ++i) {
                if (operand2 == sortResult[i]) {
                    string fromReg = "$s" + toString(i);
                    insertCode("move $t2, " + fromReg);
                    return;
                }
            }
            op2Offset = getLocalOffset(operand2);
            insertCode("lw $t2, " + toString(op2Offset) + "($fp)");
        } else {
            op2Offset = getGlobalOffset(operand2);
            insertCode("lw $t2, " + toString(op2Offset) + "($gp)");
        }
    }
}

void loadFirstTwoOperands(infixNotation notation) {
    loadFirstOperand(notation.operand1);
    loadSecondOperand(notation.operand2);
}

void loadThirdOperand(string operand3) {
    int staticIndex = lookUpStatic(currentFunc.c_str(), operand3.c_str());

    if (!isIdentifier(operand3)) { 
        insertCode("li $t0, " + toString(operand3));
    } else if (isIdentifier(operand3) && staticTable[staticIndex].cls == consts) {

        insertCode("li $t0, " + toString(staticTable[staticIndex].addr));
    } else { 
        int op3Offset;
        if (staticTable[staticIndex].level != 0) {
            
            for (int i = 0; i < sortResult.size(); ++i) {
                if (operand3 == sortResult[i]) {
                    string fromReg = "$s" + toString(i);
                    insertCode("move $t0, " + fromReg);
                    return;
                }
            }
            op3Offset = getLocalOffset(operand3);
            insertCode("lw $t0, " + toString(op3Offset) + "($fp)");
        } else {
            op3Offset = getGlobalOffset(operand3);
            insertCode("lw $t0, " + toString(op3Offset) + "($gp)");
        }
    }
}


void storeThirdOperand(string operand3, string registerName) {
    int staticIndex3 = lookUpStatic(currentFunc.c_str(), operand3.c_str());
    int op3Offset;


    if (!isGlobal(staticIndex3)) {

        for (int i = 0; i < sortResult.size(); ++i) {
            if (sortResult[i] == operand3) { 
                string targetReg = "$s" + toString(i);
                insertCode("move " + targetReg + ", " + registerName);
                return;
            }
        }
        op3Offset = getLocalOffset(operand3);
    } else {
        op3Offset = getGlobalOffset(operand3);
    }
    string rootRegister = isGlobal(staticIndex3) ? "($gp)" : "($fp)";
    insertCode("sw " + registerName + ", " + toString(op3Offset) + rootRegister); 
}

void loadFromOperand(string operand, string toReg) {

    for (int i = 0; i < sortResult.size(); ++i) {
        if (operand == sortResult[i]) {
            string fromReg = "$s" + toString(i);
            insertCode("move " + toReg + ", " + fromReg);
            return;
        }
    }

    int opOffset = getLocalOffset(operand);
    insertCode("lw " + toReg + ", " + toString(opOffset) + "($fp)");
}

void pushGlobalStack() {
    ++globalAddr;
}

void pushGlobalStack(int count) {
    for (int i = 0; i < count; ++i) {
        ++globalAddr;
    }
}

void pushLocalStack() {
    insertCode("sub $sp, $sp, 4");
}

void pushLocalStack(int count) {
    insertCode("sub $sp, $sp, " + toString(4 * count));
}

void mipsVarDef(bool isGlobal) {
    if (isGlobal) {
        for (int i = 0; i < staticTable.size() && staticTable[i].cls != funcs; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) { 
                    pushGlobalStack();
                } else { 
                    pushGlobalStack(staticTable[i].length);
                }
            }
        }
    } else { 
        int startIndex = lookUpStatic(currentFunc.c_str());
        for (int i = startIndex + 1; i < staticTable.size() && staticTable[i].cls != funcs ; ++i) {
            if (staticTable[i].cls == vars) {
                if (staticTable[i].length == 0) {
                    pushLocalStack();
                } else { 
                    pushLocalStack(staticTable[i].length);
                }
            }
        }
    }
}


void funcHead() {
    int paramCount = staticTable[lookUpStatic(currentFunc.c_str())].length;
    int fpOffset = frameHeadOffset + paramCount * 4;
    int raOffset = fpOffset - 4;

    insertCode("sw $fp, " + toString(fpOffset) + "($sp)"); 
    insertCode("add $fp, $sp, " + toString(fpOffset)); 
    insertCode("sw $ra, " + toString(raOffset) + "($sp)"); 

    for (; infixTable[infixIndex].ioperator == "PARAM"; ++infixIndex);
}
void funcTail() {
    insertCode("lw $ra, -4($fp)");
    insertCode("move $sp, $fp");
    insertCode("lw $fp, ($sp)");
    insertCode("jr $ra\n");
}

void funcContent() {
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "CONST"; ++infixIndex); // Jump all constant definitions
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "VAR" ; ++infixIndex); // Skip all variable definitions
    mipsVarDef(false); 
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator != "FUNC" ; ++infixIndex) {

        infixNotation notation = infixTable[infixIndex];
        int staticIndex1 = lookUpStatic(currentFunc.c_str(), notation.operand1.c_str());
        int staticIndex2 = lookUpStatic(currentFunc.c_str(), notation.operand2.c_str());
        int staticIndex3 = lookUpStatic(currentFunc.c_str(), notation.operand3.c_str());

        if (notation.ioperator == "ADD") {
            loadFirstTwoOperands(notation);
            insertCode("add $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "SUB") {
            loadFirstTwoOperands(notation);
            insertCode("sub $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "MUL") {
            loadFirstTwoOperands(notation);
            insertCode("mul $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "DIV") {
            loadFirstTwoOperands(notation);
            insertCode("div $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "ASSIGN") {
            if (notation.operand2 == "#RET") {
                insertCode("move $t0, $v1");
                storeThirdOperand(notation.operand3, "$t0");
            } else { 
                if (!isIdentifier(notation.operand2)) { 
                    insertCode("li $t0, " + toString(notation.operand2));
                } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls != consts) {
               
                    int op2Offset;
                    if (staticTable[staticIndex2].level != 0) {
                        loadFromOperand(notation.operand2, "$t0");
                    } else { 
                        op2Offset = getGlobalOffset(notation.operand2);
                        insertCode("lw $t0, " + toString(op2Offset) + "($gp)");
                    }
                } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls == consts) {
                   
                    insertCode("li $t0, " + toString(staticTable[staticIndex2].addr));
                }
                storeThirdOperand(notation.operand3, "$t0");
            }
        } else if (notation.ioperator == "NEG") {
            if (!isIdentifier(notation.operand2)) { 
                insertCode("li $t0, " + toString(notation.operand2));
                insertCode("sub $t0, $0, $t0");
            } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls != consts) {
                
                int op2Offset;
                if (staticTable[staticIndex2].level != 0) {
                    loadFromOperand(notation.operand2, "$t0");
                } else { 
                    op2Offset = getGlobalOffset(notation.operand2);
                    insertCode("lw $t0, " + toString(op2Offset) + "($gp)");
                }
                insertCode("sub $t0, $0, $t0");
            } else if (isIdentifier(notation.operand2) && staticTable[staticIndex2].cls == consts) {
               
                insertCode("li $t0, " + toString(staticTable[staticIndex2].addr));
            }
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "EQL") {
            loadFirstTwoOperands(notation);
            insertCode("seq $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "NEQ") {
            loadFirstTwoOperands(notation);
            insertCode("sne $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "LSS") {
            loadFirstTwoOperands(notation);
            insertCode("slt $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "LEQ") {
            loadFirstTwoOperands(notation);
            insertCode("sle $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "GTR") {
            loadFirstTwoOperands(notation);
            insertCode("sgt $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "GEQ") {
            loadFirstTwoOperands(notation);
            insertCode("sge $t0, $t1, $t2");
            storeThirdOperand(notation.operand3, "$t0");
        } else if (notation.ioperator == "RETURN") {
            if (notation.operand3 != " ") { 
                if (staticIndex3 == -1) { 
                    insertCode("li $t0, " + notation.operand3);
                } else if (staticTable[staticIndex3].cls == consts) { 
                    insertCode("li $t0, " + toString(staticTable[staticIndex3].addr));
                } else { 
                    if (!isGlobal(staticIndex3)) {
                        loadFromOperand(notation.operand3, "$t0");
                    } else {
                        int op3Offset = getGlobalOffset(notation.operand3);
                        insertCode("lw $t0, " + toString(op3Offset) + "($gp)");
                    }
                }
            }
            insertCode("move $v1, $t0");
            insertCode("j " + currentFunc + "Tail"); 
        } else if (notation.ioperator == "PUSH") { 
            for (int i = 0; i < sortResult.size(); ++i) {
                if (sortResult[i] == "") { 
                    break;
                }
                string regName = "$s" + toString(i);
                int varOffset = getLocalOffset(sortResult[i]);
                insertCode("sw " + regName + ", " + toString(varOffset) + "($fp)");
            }
        
            insertCode("sub $sp, $sp, " + toString(frameHeadOffset)); 
            for (; infixTable[infixIndex].ioperator == "PUSH"; ++infixIndex) {
                loadThirdOperand(infixTable[infixIndex].operand3); 
                insertCode("sw $t0, ($sp)");
                insertCode("sub $sp, $sp, 4");
            }
           
            insertCode("jal " + infixTable[infixIndex].operand3); 
            
            for (int j = 0; j < sortResult.size(); ++j) {
                if (sortResult[j] == "") { 
                    break;
                }
                string regName = "$s" + toString(j);
                int varOffset = getLocalOffset(sortResult[j]);
                insertCode("lw " + regName + ", " + toString(varOffset) + "($fp)");
            }
        } else if (notation.ioperator == "CALL") { 
            for (int i = 0; i < sortResult.size(); ++i) {
                if (sortResult[i] == "") { 
                    break;
                }
                string regName = "$s" + toString(i);
                int varOffset = getLocalOffset(sortResult[i]);
                insertCode("sw " + regName + ", " + toString(varOffset) + "($fp)"); 
            }
            insertCode("sub $sp, $sp, " + toString(frameHeadOffset));
            insertCode("jal " + infixTable[infixIndex].operand3);
            for (int j = 0; j < sortResult.size(); ++j) {
                if (sortResult[j] == "") { 
                    break;
                }
                string regName = "$s" + toString(j);
                int varOffset = getLocalOffset(sortResult[j]);
                insertCode("lw " + regName + ", " + toString(varOffset) + "($fp)"); 
            }
        } else if (notation.ioperator == "SETARR") {
            int op3Offset;

            loadFirstTwoOperands(notation); 
            if (!isGlobal(staticIndex3)) { 
                insertCode("mul $t2, $t2, -4");
                op3Offset = getLocalOffset(notation.operand3); 
                insertCode("add $t3, $fp, " + toString(op3Offset)); 
            } else { 
                insertCode("mul $t2, $t2, 4"); 
                op3Offset = getGlobalOffset(notation.operand3);
                insertCode("add $t3, $gp, " + toString(op3Offset));
            }
            insertCode("add $t2, $t2, $t3");
            insertCode("sw $t1, ($t2)"); 
        } else if (notation.ioperator == "GETARR") {
            int op1Offset;

            loadSecondOperand(notation.operand2); 
            if (!isGlobal(staticIndex1)) { 
                insertCode("mul $t2, $t2, -4"); 
                op1Offset = getLocalOffset(notation.operand1); 
                insertCode("add $t3, $fp, " + toString(op1Offset));
            } else { 
                insertCode("mul $t2, $t2, 4"); 
                op1Offset = getGlobalOffset(notation.operand1);
                insertCode("add $t3, $gp, " + toString(op1Offset));
            }
            insertCode("add $t2, $t2, $t3"); 
            insertCode("lw $t1, ($t2)"); 
            storeThirdOperand(notation.operand3, "$t1");
        } else if (notation.ioperator == "JMP") {
            insertCode("j " + notation.operand3);
        } else if (notation.ioperator == "BEQ") {
            loadFirstTwoOperands(notation);
            insertCode("beq $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "BNE") {
            loadFirstTwoOperands(notation);
            insertCode("bne $t1, $t2, " + notation.operand3);
        } else if (notation.ioperator == "SCANF") {
            int operand3Offset = getLocalOffset(notation.operand3);
            if (staticTable[staticIndex3].typ == chars) { 
                insertCode("li $v0, 12");
                insertCode("syscall");
                storeThirdOperand(notation.operand3, "$v0");
            } else { 
                insertCode("li $v0, 5");
                insertCode("syscall");
                storeThirdOperand(notation.operand3, "$v0");
            }
        } else if (notation.ioperator == "PRINTF") {
            if (notation.operand2 != " ") { 
                insertCode("la $a0, " + stringLabels[stringIndex]);
                insertCode("li $v0, 4");
                insertCode("syscall");
                ++stringIndex;
            }
            if (notation.operand3 != " ") {
                int operand3Offset;
                if (staticTable[staticIndex3].cls == consts) { 
                    insertCode("li $a0, " + toString(staticTable[staticIndex3].addr)); 
                } else {
                    if (!isGlobal(staticIndex3)) { 
                        loadFromOperand(notation.operand3, "$a0");
                    } else { 
                        operand3Offset = getGlobalOffset(notation.operand3);
                        insertCode("lw $a0, " + toString(operand3Offset) + "($gp)");
                    }
                }
                if (staticTable[staticIndex3].typ == chars) { 
                    insertCode("li $v0, 11");
                } else { 
                    insertCode("li $v0, 1");
                }
                insertCode("syscall");
            }
        } else if (notation.ioperator == "LABEL") {
            insertCode(notation.operand3 + ":");
        }
    }
}

void mipsFuncDef() {
    currentFunc = infixTable[infixIndex].operand3;
    insertCode(currentFunc + ":"); 
    ++infixIndex;

    sortVariables();
  
    funcHead();
    funcContent();
    insertCode(currentFunc + "Tail:"); 
    funcTail();
}

void mainHead() {
    insertCode("sw $fp, ($sp)");
    insertCode("move $fp, $sp");
    insertCode("sub $sp, $sp, 8");
}

void mipsMainDef() {
    currentFunc = "main";
    insertCode("main:"); 
    ++infixIndex;
    sortVariables();
    mainHead();
    funcContent();
    insertCode("mainTail:");
}

void mipsJ(string label) {
    insertCode("j " + label);
}

inline void outputMipsCodes() {
    mipsFile << mipsCodes;
}

void optMipsProgram(string mipsFileName) {
    mipsFile.open(mipsFileName.c_str(), ios::out);
    stringLabels.reserve(maxLabelCount);

    insertCode(".data");
    for (int i = 0, count = 0; i < infixTable.size(); ++i) {
        if (infixTable[i].ioperator == "PRINTF" && infixTable[i].operand2 != " ") {
            insertCode("strLabel" + toString(count) + ":");
            insertCode(".asciiz");
            insertCode(infixTable[i].operand2);
            stringLabels.push_back("strLabel" + toString(count));
            count++;
        }
    }

    insertCode(""); 
    insertCode(".text");
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "CONST"; ++infixIndex);
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "VAR" ; ++infixIndex) {
    }
    mipsVarDef(true);
    mipsJ(labelMain); 
    insertCode("");
    for (; infixIndex < infixTable.size() && infixTable[infixIndex].ioperator == "FUNC"
           && infixTable[infixIndex].operand3 != "main";) {
        mipsFuncDef();
    }
    mipsMainDef();
    outputMipsCodes();
}
