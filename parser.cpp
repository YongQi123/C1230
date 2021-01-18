
#include "main.h"

#define isCompare(a) (a == eql || a == neq || a == gtr || a == geq || a == lss || a == leq)

int level = 0;
int paramCount = 0;
int labelIndex = 0;
const string labelStr = "LABEL";

void integer(string &infixString) {
    symbol tempSy;

    if (sy == plus || sy == minus) {
        tempSy = sy;
        insymbol();
        if (sy == intcon) {
            if (inum != 0) {
                if (tempSy == minus) { 
                    inum = inum * (-1);
                }
                infixString = toString(inum);
                insymbol();
            } else {
                error(SignedZero); 
            }
        } else {
            error(NumErr); 
        }
    } else if (sy == intcon) {
        infixString = toString(inum);
        insymbol();
    } else {
        error(NumErr); 
    }
}


void program() {
    insymbol();
    if (sy == constsy) { 
        constState();
    }

    if (sy == intsy || sy == charsy) { 
        int tempCharPtr = charPtr; 
        char tempCh = ch;
        symbol tempSy = sy;
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            error(NotVarFunc); 
            charPtr = tempCharPtr; 
            ch = tempCh;
            sy = tempSy;
        }
        if (sy == lparent) { 
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;

        } else if (sy == lbrack ||
                   sy == comma || sy == semicolon){ 
            charPtr = tempCharPtr; 
            ch = tempCh;
            sy = tempSy;
            globalVarState();
        } else {
            error(NotVarFunc);
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;
        }
    }


    while (sy == intsy || sy == charsy || sy == voidsy) {
        if (sy == intsy || sy == charsy) { 
            funcWithRetDef();
        } else { 
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;
            insymbol();
            if (sy == identi && sy != mainsy) { 
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithoutRetDef();
            } else if (sy == mainsy) { 
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                break;
            } else {
                error(IdLost);
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                funcWithoutRetDef();
            }
        }
    }

    if (sy == voidsy) {
        mainDef();
    } else {
        error(IllegalSyntax); 

        while (true) {
            insymbol();
        }
    }
}

void constState() {
    while (sy == constsy) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
        constDef();
        if (sy == semicolon) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag) {
            error(SemicolonLost);
        }
    }
    cout << "This is a const declaration statement." << endl;
}

void constDef() {
    char idName[100];
    string infixString = "";

    if (sy == intsy) { 
        do {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
            if (sy == identi) {
                strcpy(idName, token);
                insymbol();
            } else {
                error(IdLost); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }

            if (sy == becomes) {
                insymbol();
            } else if (!skipFlag){
                error(AssignSyLost); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }
            if(!skipFlag) {
                integer(infixString); 
            }
            if (isDefinable(idName)) {
                insertTable(consts, ints, idName, 0, level, strToInt(infixString));
                insertInfix("CONST", infixString, "int", idName);
            } else {
                error(IdRepeat);
            }
        } while (sy == comma);
    } else if (sy == charsy) { 
        do{
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
            if (sy == identi) {
                strcpy(idName, token);
                insymbol();
            } else {
                error(IdLost); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }
            if (sy == becomes) {
                insymbol();
            } else if (!skipFlag){
                error(AssignSyLost); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }
            if (sy == charcon) {
                if (isDefinable(idName)) {
                    insertTable(consts, chars, idName, 0, level, inum);
                    insertInfix("CONST", toString(inum), "char", idName);
                } else {
                    error(IdRepeat); 
                }
                insymbol();
            } else if (!skipFlag) {
                error(CharConErr); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }
        }while (sy == comma);
    } else if (!skipFlag){
        error(ConDefTypeErr);
        symbol nexts[] = {comma, semicolon, rbrace};
        skipUntil(nexts);
    }
}
 
    int tempCharPtr = 0;
    char tempCh = ' ';
    symbol tempSy = sy;

    do {
        if (skipFlag) {
            skipFlag = false;
        }
        tempCharPtr = charPtr;
        tempCh = ch;
        tempSy = sy;
        insymbol();
        if (sy == identi) {
            insymbol();
        } else {
            error(IdLost); 
            symbol nexts[] = {semicolon};
            skipUntil(nexts);
        }
        if (sy == lparent) { 
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;
            cout << "This is a global variable declaration statement." << endl;
            return;
        } else if (!skipFlag) { 
            charPtr = tempCharPtr;
            ch = tempCh;
            sy = tempSy;
        }
        varDef();
        if (sy == semicolon) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag) {
            error(SemicolonLost); 
        }
    } while (sy == intsy || sy == charsy);
    cout << "This is a global variable declaration statement." << endl;
}


void varState() {
    do {
        if (skipFlag) {
            skipFlag = false;
        }
        varDef();
        if (sy == semicolon) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag) {
            error(SemicolonLost); 
        }
    } while (sy == intsy || sy == charsy);
    cout << "This is a variable declaration statement." << endl;
}


void varDef() {
    char idName[100] = "";
    type varType = voids;

    if (sy == intsy || sy == charsy) {
        if (sy == intsy) {
            varType = ints;
        } else {
            varType = chars;
        }
        insymbol();
    } else if (!skipFlag) {
        error(IllegalType); 
        symbol nexts[] = {comma, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        strcpy(idName, token);
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {comma, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lbrack) { 
        insymbol();
        if (sy == intcon && inum != 0) {
            insymbol();
            if (isDefinable(idName)) {
                insertTable(vars, varType, idName, inum, level, 0);
                insertInfix("VAR", toString(inum), varType == ints ? "int" : "char", idName);
            } else {
                error(IdRepeat);
            }
        } else {
            error(ArrIndexErr); 
        }
        if (sy == rbrack) {
            insymbol();
        } else {
            error(RightBrackLost);
            symbol nexts[] = {comma, semicolon, rbrace};
            skipUntil(nexts);
        }
    } else { 
        if (isDefinable(idName)) {
            insertTable(vars, varType, idName, 0, level, 0);
            insertInfix("VAR", " ", varType == ints ? "int" : "char", idName);
        } else {
            error(IdRepeat); 
        }
    }
    while (sy == comma) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
        if (sy == identi) {
            strcpy(idName, token);
            insymbol();
        } else {
            error(IdLost); 
            symbol nexts[] = {comma, semicolon, rbrace};
            skipUntil(nexts);
        }
        if (sy == lbrack) { 
            insymbol();
            if (sy == intcon) {
                insymbol();
                if (isDefinable(idName)) {
                    insertTable(vars, varType, idName, inum, level, 0);
                    insertInfix("VAR", toString(inum), varType == ints ? "int" : "char", idName);
                } else {
                    error(IdRepeat); 
                }
            } else {
                error(ArrIndexErr); 
                symbol nexts[] = {comma, semicolon, rbrace};
                skipUntil(nexts);
            }
            if (sy == rbrack) {
                insymbol();
            } else {
                error(RightBrackLost); 
            }
        } else { 
            if (isDefinable(idName)) {
                insertTable(vars, varType, idName, 0, level, 0);
                insertInfix("VAR", " ", varType == ints ? "int" : "char", idName);
            } else {
                error(IdRepeat); 
            }
        }
    }
}


void defHead(type* returnType, char* identifier) {
    if (sy == intsy) {
        *returnType = ints;
        insymbol();
    } else if (sy == charsy) {
        *returnType = chars;
        insymbol();
    } else if (!skipFlag){
        error(DefHeadErr); 
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        strcpy(identifier, token);
        insymbol();
    } else if (!skipFlag) {
        error(IdLost);
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
}

void funcWithRetDef() {
    char idName[100] = "";
    string idNameString = "";
    paramCount = 0;
    type returnType = voids;

    defHead(&returnType, idName); 
    if (isDefinable(idName)) { 
        insertTable(funcs, returnType, idName, paramCount, level, 0); 
        idNameString = idName;
        insertInfix("FUNC", " ", returnType == ints ? "int" : "char", idNameString);
    } else {
        error(IdRepeat); 
    }

    level++; 
    if (sy == lparent) {
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == rparent) { 
        insymbol();
    } else {
        if (!skipFlag) {
            parameter(); 
        }
        int idIndex = lookUp(idName);
        if (idIndex != -1) {
            idTable[idIndex].length = paramCount; 
            staticTable[lookUpStatic(idName)].length = paramCount;
        }
        if (sy == rparent) {
            insymbol();
        } else if (!skipFlag) {
            error(RightParentLost); 
            symbol nexts[] = {lbrace, rbrace};
            skipUntil(nexts);
        }
    }
    if (sy == lbrace) {
        insymbol();
    } else if (!skipFlag) {
        error(LeftBraceLost); 
        symbol nexts[] = {rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        complexState();
    }
    if (sy == rbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        cout << "This is a definition of function with return." << endl;
        insymbol();
    } else {
        error(RightBraceLost); 
    }

    popLocals();
    level--; /
}

void funcWithoutRetDef() {
    char idName[100] = "";
    paramCount = 0;

    level++; 
    if (sy == voidsy) { 
        insymbol();
    } else {
        error(VoidSyLost); 
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        if (isDefinable(token)) { 
            string idNameString = token;
            insertTable(funcs, voids, token, paramCount, level - 1, 0); 
            insertInfix("FUNC", " ", "void", idNameString);
        } else {
            error(IdRepeat); 
        }
        strcpy(idName, token);
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else { 
        if (!skipFlag) {
            parameter(); 
        }
        int idIndex = lookUp(idName);
        if (idIndex != - 1) { 
            idTable[idIndex].length = paramCount;
            staticTable[lookUpStatic(idName)].length = paramCount;
        }
        if (sy == rparent) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag){
            error(RightParentLost); 
            symbol nexts[] = {lbrace, rbrace};
            skipUntil(nexts);
        }
    }
    if (sy == lbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftBraceLost); 
        symbol nexts[] = {rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        complexState(); 
    }
    if (sy == rbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        cout << "This is a definition of function without return." << endl;
        insymbol();
    } else {
        error(RightBraceLost); 
    }


    popLocals();
    level--; 
}

void parameter() {
    parameterTable();
}

void parameterTable() {
    type paramType = voids;

    if (sy == intsy || sy == charsy) {
        if (sy == intsy) {
            paramType = ints;
        } else {
            paramType = chars;
        }
        insymbol();
    } else if (!skipFlag){
        error(TypeDefLost); 
        symbol nexts[] = {comma, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        paramCount++;
        if (isDefinable(token)) {
            insertTable(params, paramType, token, 0, level, 0);
            insertInfix("PARAM", " ", paramType == ints ? "int" : "char", token);
        } else {
            error(IdRepeat); 
        }
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {comma, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    while (sy == comma) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
        if (sy == intsy || sy == charsy) {
            if (sy == intsy) {
                paramType = ints;
            } else {
                paramType = chars;
            }
            insymbol();
        } else {
            error(TypeDefLost); 
            symbol nexts[] = {comma, rparent, lbrace, rbrace};
            skipUntil(nexts);
        }
        if (sy == identi) {
            paramCount++;
            if (isDefinable(token)) {
                insertTable(params, paramType, token, 0, level, 0);
            } else {
                error(IdRepeat); 
            }
            insymbol();
        } else if (!skipFlag) {
            error(IdLost); 
            symbol nexts[] = {comma, rparent, lbrace, rbrace};
            skipUntil(nexts);
        }
    }
}

void complexState() {
    while (sy == constsy) {
        constState();
    }
    while (sy == intsy || sy == charsy) {
        varState();
    }
    stateList();
}

void mainDef() {
    level++;
    if (sy == voidsy) { 
        insymbol();
    } else if (!skipFlag) {
        error(VoidSyLost);
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == mainsy) {
        if (isDefinable(token)) {
            insertTable(funcs, voids, token, 0, level - 1, 0);
        } else {
            error(IdRepeat); 
        }
        insymbol();
    } else if (!skipFlag) {
        error(IdLost);
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost);
        symbol nexts[] = {rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost); 
        symbol nexts[] = {lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == lbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftBraceLost); 
        symbol nexts[] = {rbrace};
        skipUntil(nexts);
    }
    insertInfix("FUNC", " ", "void", "main");
    if (!skipFlag) {
        complexState();
    }
    if (sy == rbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        popLocals();
        cout << "This is the main function." << endl;
        insymbol();
    } else {
        error(RightBraceLost); 
    }
    level--;
}

// ＜因子＞ ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’
type factor(string &infixString) {
    type firstType = voids;

    switch (sy) {
        case identi: {
            int idIndex;
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;

            idIndex = lookUp(token);
            if (idIndex == -1) {
                error(IdNotDefine);
            } else {
                firstType = idTable[idIndex].typ;
            }
            infixString = token; 
            insymbol();
            if (sy == lbrack) { 
                string arrayIndex;
                if (idIndex != -1) {
                    if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                        || idTable[idIndex].length == 0) {
                        error(IdKindNotMatch); 
                    }
                }
                insymbol();
                if (!skipFlag) {
                    expression(arrayIndex);
                }
                if (sy == rbrack) { 
                    string tempVar = createTempVar();
                    insertTable(vars, firstType, tempVar.c_str(), 0, level, 0);
                    insertInfix("GETARR", infixString, arrayIndex, tempVar);
                    infixString = tempVar; 
                    insymbol();
                } else if (!skipFlag) {
                    error(RightBrackLost); 
                    symbol nexts[] = {semicolon, rbrace};
                    skipUntil(nexts);
                }
            } else if (sy == lparent) {
                if (idTable[idIndex].cls != funcs) {
                    error(IdKindNotMatch);
                }
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                if (!skipFlag) {
                    funcWithRetCall(infixString); 
                }
            } else { 
                if ((idTable[idIndex].cls != consts && idTable[idIndex].cls != vars && idTable[idIndex].cls != params) ||
                        idTable[idIndex].length != 0) {
                    error(IdKindNotMatch); 
                }
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                insymbol();
            }
            break;
        }
        case plus:
        case minus:
        case intcon: {
            if (!skipFlag) {
                integer(infixString); 
            }
            firstType = ints;
            break;
        }
        case charcon: {
            infixString = toString(inum);
            insymbol();
            firstType = chars;
            break;
        }
        case lparent: { 
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
            firstType = expression(infixString);
            if (sy == rparent) {
                insymbol();
            } else {
                error(RightParentLost); 
                symbol nexts[] = {semicolon, rbrace};
                skipUntil(nexts);
            }
            break;
        }
        default:
            error(IllegalFact); 
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
    }
    return firstType;
}

// ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
type term(string &infixString) {
    type firstType = factor(infixString);
    type tempType = voids;
    string tempOperand = "";
    string tempVariable = "";
    string ioperator = "";

    while (sy == times || sy == idiv) {
        ioperator = sy == times ? "MUL" : "DIV";
        insymbol();
        tempType = factor(tempOperand);
        if (firstType != tempType) { 
            firstType = ints;
        }
        tempVariable = createTempVar();
        insertTable(vars, tempType, tempVariable.c_str(), 0, level, 0);
        insertInfix(ioperator, infixString, tempOperand, tempVariable);
        infixString = tempVariable;
    }
    return firstType;
}

// ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
type expression(string &infixString) { // infixString here must be a variable or value
    type firstType = voids;
    type tempType = voids;
    symbol tempSy = voidsy;
    string tempOperand = "";
    string tempVariable = "";

    if (sy == plus || sy == minus) {
        tempSy = sy;
        insymbol();
    }
    if (!skipFlag) {
        firstType = term(infixString);
    }
    if (tempSy == minus) { 
        tempVariable = createTempVar();
        insertTable(vars, ints, tempVariable.c_str(), 0, level, 0);
        insertInfix("NEG", " ", infixString, tempVariable);
        infixString = tempVariable;
    }
    while (sy == plus || sy == minus) {
        tempSy = sy;
        insymbol();
        tempType = term(tempOperand);
        if (firstType != tempType) { 
            warn(0);
            firstType = ints; 
        }
        tempVariable = createTempVar();
        insertTable(vars, ints, tempVariable.c_str(), 0, level, 0);
        insertInfix(tempSy == plus ? "ADD" : "SUB", infixString, tempOperand, tempVariable);
        infixString = tempVariable;
    }
    return firstType;
}

// ＜赋值语句＞ ::= ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
void assignState() {
    string leftToEqual = "";
    string rightToEqual = "";
    int idIndex = -1;
    type firstType = voids;
    type secondType = voids;

    if (sy == identi) {
        if ((idIndex = lookUp(token)) == -1) {
            error(IdNotDefine); 
        } else {
            firstType = idTable[idIndex].typ;
        }
        leftToEqual = token; 
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == becomes) { 
        if (idIndex != -1) {
            if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                || idTable[idIndex].length != 0) {
                error(IdKindNotMatch); 
            }
        }
        insymbol();
        secondType = expression(rightToEqual);
        if (firstType != secondType) {
            warn(0); 
        }
        insertInfix("ASSIGN", " ", rightToEqual, leftToEqual);
        cout << "This is an assignment statement." << endl;
    } else if (sy == lbrack) { 
        string arrayIndex;

        if (idIndex != -1) {
            if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                || idTable[idIndex].length == 0) {
                error(IdKindNotMatch); 
            }
        }
        insymbol();
        expression(arrayIndex);
        if (sy == rbrack) {
            insymbol();
        } else if (!skipFlag) {
            error(RightBrackLost); 
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
        }
        if (sy == becomes) {
            insymbol();
        } else if (!skipFlag) {
            error(BecomesSyLost);
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
        }
        if (!skipFlag) {
            secondType = expression(rightToEqual);
        }
        if (firstType != secondType) {
            warn(0); 
        }
        insertInfix("SETARR", rightToEqual, arrayIndex, leftToEqual);
        cout << "This is an assignment statement." << endl;
    } else if (!skipFlag) {
        error(BecomesSyLost);
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }

}

// ＜条件＞ ::= ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void judgement(string &infixString) {
    string compareLeft = "";
    string compareRight = "";
    symbol compareSy = eql;

    if (!skipFlag) {
        expression(compareLeft);
    }
    if (isCompare(sy)) { 
        compareSy = sy;
        insymbol();
        expression(compareRight);
        infixString = createTempVar();
        insertTable(vars, ints, infixString.c_str(), 0, level, 0); // 类型为数字，1表示判断成立，0表示判断不成立
        switch (compareSy) {
            case eql:
                insertInfix("EQL", compareLeft, compareRight, infixString);
                break;
            case neq:
                insertInfix("NEQ", compareLeft, compareRight, infixString);
                break;
            case lss:
                insertInfix("LSS", compareLeft, compareRight, infixString);
                break;
            case leq:
                insertInfix("LEQ", compareLeft, compareRight, infixString);
                break;
            case gtr:
                insertInfix("GTR", compareLeft, compareRight, infixString);
                break;
            case geq:
                insertInfix("GEQ", compareLeft, compareRight, infixString);
                break;
        }
    } else { 
        infixString = compareLeft;
    }
}

// ＜条件语句＞ ::= if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
void ifState() {
    string judgeResult = "";
    string firstLabel = "";
    string secondLabel = "";

    if (sy == ifsy) {
        insymbol();
    } else if (!skipFlag) {
        error(IfSyLost); 
        symbol nexts[] = {lparent, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        judgement(judgeResult);
    }
    firstLabel = "LABEL" + toString(labelIndex);
    labelIndex++;
    insertInfix("BEQ", judgeResult, "0", firstLabel); 
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost); 
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        statement();
    }
    secondLabel = "LABEL" + toString(labelIndex);
    labelIndex++;
    insertInfix("JMP", " ", " ", secondLabel); 
    insertInfix("LABEL", " ", " ", firstLabel); 
    labelIndex++;
    if (sy == elsesy) {
        insymbol();
        statement();
        labelIndex++;
    }
    insertInfix("LABEL", " ", " ", secondLabel); 
    cout << "This is an if statement." << endl;
}

// ＜循环语句＞ ::= for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
void loopState() {
    string idNameString = "";
    string idNameString2 = "";
    string expressionString = "";
    string judgeResult = "";
    string judgeLabel = "";
    string increaseLabel = "";
    string bodyLabel = "";
    string endLabel = "";

    int idIndex = -1;

    if (sy == forsy) {
        insymbol();
    } else if (!skipFlag) {
        error(ForSyLost); 
        symbol nexts[] = {lparent, semicolon, rparent, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else {
        error(LeftParentLost); 
        symbol nexts[] = {semicolon, rparent, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            error(IdNotDefine); 
        } else if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                   || idTable[idIndex].length != 0) {
            error(IdKindNotMatch); 
        }
        idNameString = token;
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {semicolon, rparent, rbrace};
        skipUntil(nexts);
    }
    if (sy == becomes) {
        insymbol();
    } else if (!skipFlag) {
        error(BecomesSyLost);
        symbol nexts[] = {semicolon, rparent, rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        expression(expressionString);
    }
    insertInfix("ASSIGN", " ", expressionString, idNameString);
    bodyLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("JMP", " ", " ", bodyLabel);
    if (sy == semicolon) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(SemicolonLost);
        symbol nexts[] = {semicolon, rparent, rbrace};
        skipUntil(nexts);
    }
    judgeLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("LABEL", " ", " ", judgeLabel);
    if (!skipFlag) {
        judgement(judgeResult);
    }
    insertInfix("BNE", judgeResult, "0", bodyLabel);
    endLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("BEQ", judgeResult, "0", endLabel);
    if (sy == semicolon) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else {
        error(SemicolonLost);
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    increaseLabel = labelStr + toString(labelIndex);
    ++labelIndex;
    insertInfix("LABEL", " ", " ", increaseLabel);
    if (sy == identi) {
        if (idIndex == -1) {
            error(IdNotDefine); 
        } else if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                   || idTable[idIndex].length != 0) {
            error(IdKindNotMatch); 
        }
        idNameString = token;
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == becomes) {
        insymbol();
    } else if (!skipFlag) {
        error(BecomesSyLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        if (idIndex == -1) {
            error(IdNotDefine);
        } else if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                   || idTable[idIndex].length != 0) {
            error(IdKindNotMatch); 
        }
        idNameString2 = token;
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    string ioperator;
    if (sy == plus || sy == minus) {
        ioperator = sy == plus ? "ADD" : "SUB";
        insymbol();
    } else if (!skipFlag) {
        error(PlusLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == intcon) {
        if (inum != 0) {
            insertInfix(ioperator, idNameString2, toString(inum), idNameString); 
            insymbol();
        } else {
            error(StepLenZero); 
        }
    } else if (!skipFlag) {
        error(StepLenLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    insertInfix("JMP", " ", " ", judgeLabel);
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost); 
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
    insertInfix("LABEL", " ", " ", bodyLabel);
    if (!skipFlag) {
        statement();
    }
    insertInfix("JMP", " ", " ", increaseLabel);
    insertInfix("LABEL", " ", " ", endLabel);
    cout << "This is a loop statement." << endl;
}

// ＜情况语句＞ ::= switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
void conditionState() {
    string switchElement = "";
    string endLabel = "";

    if (sy == switchsy) {
        insymbol();
    } else if (!skipFlag) {
        error(SwitchSyLost); 
        symbol nexts[] = {lparent, rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {rparent, lbrace, rbrace};
        skipUntil(nexts);
    }
    type firstType = expression(switchElement);
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost);
        symbol nexts[] = {lbrace, rbrace};
        skipUntil(nexts);
    }
    if (sy == lbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftBraceLost); 
        symbol nexts[] = {rbrace};
        skipUntil(nexts);
    }
    endLabel = labelStr + toString(labelIndex);
    labelIndex++;
    if (!skipFlag) {
        conditionTable(firstType, switchElement, endLabel);
    }
    if (!skipFlag) {
        conditionDefault();
    }
    if (sy == rbrace) {
        if (skipFlag) {
            skipFlag = false;
        }
        cout << "This is a condition statement." << endl;
        insymbol();
    } else {
        error(RightBraceLost); 
    }
    insertInfix("LABEL", " ", " ", endLabel);
}

// ＜情况表＞ ::= ＜情况子语句＞{＜情况子语句＞}
void conditionTable(type firstType, string switchElement, string endSwitchLabel) {
    do {
        conditionBranch(firstType, switchElement, endSwitchLabel);
    } while (sy == casesy);
}

// ＜情况子语句＞ ::= case＜常量＞:＜语句＞
void conditionBranch(type firstType, string switchElement, string endSwitchLabel) {
    string caseElement = "";
    string nextCaseLabel = "";

    if (sy == casesy) {
        insymbol();
    } else if (!skipFlag) {
        error(CaseSyLost)
        symbol nexts[] = {colon, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == charcon) { 
        if (firstType != chars) {
            warn(0); 
        }
        caseElement = toString(inum);
        insymbol();
    } else if (sy == intcon) { 
        if (firstType != ints) {
            warn(0); 
        }
        caseElement = toString(inum);
        insymbol();
    } else if (sy == plus || sy == minus) { 
        symbol tempSy = sy;
        if (firstType != ints) {
            warn(0); 
        }
        insymbol();
        if (sy == intcon && inum != 0) {
            int temp = tempSy == plus ? inum : (inum * (-1));
            caseElement = toString(temp);
            insymbol();
        } else {
            error(CaseNotCon);
            symbol nexts[] = {colon, semicolon, rbrace};
            skipUntil(nexts);
        }
    } else if (!skipFlag) {
        error(CaseNotCon);
        symbol nexts[] = {colon, semicolon, rbrace};
        skipUntil(nexts);
    }
    nextCaseLabel = labelStr + toString(labelIndex);
    labelIndex++;
    insertInfix("BNE", switchElement, caseElement, nextCaseLabel); 
    if (sy == colon) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag){
        error(ColonLost); 
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        statement();
    }
    insertInfix("JMP", " ", " ", endSwitchLabel);
    insertInfix("LABEL", " ", " ", nextCaseLabel);
}

// ＜缺省＞ ::= default:＜语句＞
void conditionDefault() {
    if (sy == defaultsy) {
        insymbol();
    } else if (!skipFlag) {
        error(DefaultSyLost); 
        symbol nexts[] = {colon, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == colon) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(ColonLost); 
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
    if (!skipFlag) {
        statement();
    }
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞|<空>‘)’
void funcWithRetCall(string &infixString) {
    string funcName = "";
    int idIndex = -1;

    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            error(IdNotDefine); 
        } else if (idTable[idIndex].cls != funcs || idTable[idIndex].typ == voids) {
            error(IdKindNotMatch); 
        }
        funcName = token;
        insymbol();
    } else if (!skipFlag) {
        error(IdLost);
        symbol nexts[] = {lparent, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == rparent) { 
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else { 
        if (!skipFlag) {
            valueParamTable(idIndex);
        }
        if (sy == rparent) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag) {
            error(RightParentLost); 
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
        }
    }
    insertInfix("CALL", " ", " ", funcName);
    infixString = createTempVar();
    if (idIndex != -1) {
        insertTable(vars, idTable[idIndex].typ, infixString.c_str(), 0, level, 0);
    }
    insertInfix("ASSIGN", " ", "#RET", infixString);
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞|<空>‘)’
void funcWithoutRetCall() {
    string funcName = "";
    int idIndex = -1;

    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            error(IdNotDefine); 
        } else if (idTable[idIndex].cls != funcs) {
            error(IdKindNotMatch); 
        }
        funcName = token;
        insymbol();
    } else if (!skipFlag) {
        error(IdLost); 
        symbol nexts[] = {lparent, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost);
        symbol nexts[] = {rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else {
        if (!skipFlag) {
            valueParamTable(idIndex);
        }
        if (sy == rparent) {
            if (skipFlag) {
                skipFlag = false;
            }
            insymbol();
        } else if (!skipFlag) {
            error(RightParentLost); 
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
        }
    }
    insertInfix("CALL", " ", " ", funcName);
}

// ＜值参数表＞ ::= ＜表达式＞{,＜表达式＞}
void valueParamTable(int idIndex) {
    int valueParamIndex = 1;
    tabElement function = idTable[idIndex];
    int funcIndex = lookUpStatic(function.name); 
    type secondType = voids;
    string infixString = "";
    std::vector<string> parameterStrings;

    parameterStrings.reserve(10);

    if (!skipFlag) {
        secondType = expression(infixString);
    }
    if (valueParamIndex <= function.length) {
        if (staticTable[funcIndex + valueParamIndex].typ != secondType) {
            warn(0); 
        }
        parameterStrings.push_back(infixString); 
        error(ParamCountExceed); 
    }
    valueParamIndex++;
    while (sy == comma) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
        if (!skipFlag) {
            secondType = expression(infixString);
        }
        if (valueParamIndex <= function.length) {
            if (staticTable[funcIndex + valueParamIndex].typ != secondType) {
                warn(0); 
            }
            parameterStrings.push_back(infixString); 
        } else {
            error(ParamCountExceed); 
        }
        valueParamIndex++;
    }
    for (int i = 0; i < parameterStrings.size(); ++i) {
        insertInfix("PUSH", " ", " ", parameterStrings[i]);
    }
}

// ＜返回语句＞ ::= return[‘(’＜表达式＞‘)’]
void returnState() {
    type firstType = idTable[findCurrentFunc()].typ;
    type secondType = voids;
    string returnString = "";

    if (sy == returnsy) {
        insymbol();
    } else if (!skipFlag) {
        error(ReturnSyLost); 
        symbol nexts[] = {lparent, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) { 
        insymbol();
        secondType = expression(returnString);
        if (sy == rparent) {
            if (firstType != secondType) {
                warn(0); 
            }
            insertInfix("RETURN", " ", " ", returnString);
            cout << "This is a return statement." << endl;
            insymbol();
        } else if (!skipFlag) {
            error(RightParentLost); 
            symbol nexts[] = {semicolon, rbrace};
            skipUntil(nexts);
        }
    } else {
        insertInfix("RETURN", " ", " ", " "); 
        cout << "This is a return statement." << endl;
    }
}

// ＜读语句＞ ::= scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
void scanfState() {
    int idIndex = -1;
    string scanString = "";

    if (sy == scanfsy) {
        insymbol();
    } else if (!skipFlag) {
        error(ScanfSyLost); 
        symbol nexts[] = {lparent, comma, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost);
        symbol nexts[] = {comma, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == identi) {
        idIndex = lookUp(token);
        if (idIndex == -1) {
            error(IdNotDefine); 
        } else if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                   || idTable[idIndex].length != 0) {
            error(IdKindNotMatch); 
        }
        scanString = token;
        insertInfix("SCANF", " ", " ", scanString);
        insymbol();
    } else if (!skipFlag){
        error(IdLost); 
        symbol nexts[] = {comma, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }

    while (sy == comma){
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
        if (sy == identi) {
            if (idIndex == -1) {
                error(IdNotDefine); 
            } else if ((idTable[idIndex].cls != vars && idTable[idIndex].cls != params)
                       || idTable[idIndex].length != 0) {
                error(IdKindNotMatch); 
            }
            scanString = token;
            insertInfix("SCANF", " ", " ", scanString);
            insymbol();
        } else {
            error(IdLost); 
            symbol nexts[] = {comma, rparent, semicolon, rbrace};
            skipUntil(nexts);
        }
    }
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        cout << "This is a scanf statement." << endl;
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost);
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
}

// ＜写语句＞ ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞‘)’| printf ‘(’＜表达式＞‘)’
void printfState() {
    type expressionType = voids;
    string generalString = " "; 
    string expressionString = " ";

    if (sy == printfsy) {
        insymbol();
    } else if (!skipFlag) {
        error(PrintfSyLost); 
        symbol nexts[] = {lparent, comma, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == lparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insymbol();
    } else if (!skipFlag) {
        error(LeftParentLost); 
        symbol nexts[] = {comma, rparent, semicolon, rbrace};
        skipUntil(nexts);
    }
    if (sy == stringcon) { 
        generalString = str;
        generalString = "\"" + generalString + "\"";
        insymbol();
        if (sy == comma) { 
            insymbol();
            expressionType = expression(expressionString);
        }
    } else { 
        if (!skipFlag) {
            expressionType = expression(expressionString);
        }
    }
    if (sy == rparent) {
        if (skipFlag) {
            skipFlag = false;
        }
        insertInfix("PRINTF", " ", generalString, expressionString);
        cout << "This is a printf statement." << endl;
        insymbol();
    } else if (!skipFlag) {
        error(RightParentLost);
        symbol nexts[] = {semicolon, rbrace};
        skipUntil(nexts);
    }
}

/* <语句> ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞;
| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;|＜情况语句＞｜
 ＜返回语句＞;｜＜空＞;*/
void statement() {
    string emptyString = "";

    switch (sy) {
        case ifsy:
            ifState();
            break;
        case forsy:
            loopState();
            break;
        case lbrace: {
            insymbol();
            stateList();
            if (sy == rbrace){
                insymbol();
            } else {
                error(RightBraceLost);
            }
            break;
        }
        case identi: {
            int tempCharPtr = charPtr;
            char tempCh = ch;
            symbol tempSy = sy;
            char tempToken[100];
            strcpy(tempToken, token);
            insymbol();
            if (sy == becomes || sy == lbrack) {
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                assignState();
            } else { 
                charPtr = tempCharPtr;
                ch = tempCh;
                sy = tempSy;
                if (idTable[lookUp(tempToken)].typ == voids) {
                    funcWithoutRetCall();
                } else {
                    funcWithRetCall(emptyString);
                }
            }
            if (sy == semicolon) {
                insymbol();
            } else {
                error(SemicolonLost); 
            }
            break;
        }
        case scanfsy: {
            scanfState();
            if (sy == semicolon) {
                insymbol();
            } else {
                error(SemicolonLost);
            }
            break;
        }
        case printfsy: {
            printfState();
            if (sy == semicolon) {
                insymbol();
            } else {
                error(SemicolonLost); 
            }
            break;
        }
        case switchsy:
            conditionState();
            break;
        case returnsy: {
            returnState();
            if (sy == semicolon) {
                insymbol();
            } else {
                error(SemicolonLost); 
            }
            break;
        }
        case semicolon: 
            insymbol();
            break;
        default:
            error(StateErr); 
    }
}

// ＜语句列＞ ::=｛＜语句＞｝
void stateList() {
    while (sy == ifsy || sy == forsy || sy == lbrace || sy == identi ||
            sy == scanfsy || sy == printfsy || sy == switchsy ||
            sy == returnsy || sy == semicolon) {
        statement();
    }
    cout << "This is a statement list." << endl;
}
