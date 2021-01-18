
#ifndef C0_COMPILER_MAIN_H
#define C0_COMPILER_MAIN_H

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <set>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include "tools.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ios;
using std::fstream;
using std::map;
using namespace tools;

const int maxIdentLength = 100;
const int maxLineLength = 100;
const int keywordCount = 14;
const int maxDigit = 15;
const int maxInt = 100000000;

typedef char alpha[maxIdentLength + 1];
typedef enum {
    identi,
    constsy,
    intsy, charsy,
    voidsy,
    intcon, charcon, stringcon,
    plus, minus, times, idiv,
    eql, neq, gtr, geq, lss, leq,
    becomes,
    lparent, rparent, lbrack, rbrack, lbrace, rbrace,
    comma, semicolon, colon,
    ifsy, elsesy,
    switchsy, casesy, defaultsy,
    forsy,
    scanfsy, printfsy,
    mainsy,
    returnsy
} symbol;
typedef enum {
    consts,
    vars,
    params,
    funcs
} kind;
typedef enum {
    ints, chars, voids
} type;
typedef struct {
    char name[maxIdentLength];
    kind cls;
    type typ;
    int addr;
    int length;
    int level;
} tabElement;
typedef struct {
    string ioperator;
    string operand1;
    string operand2;
    string operand3;
} infixNotation;
typedef enum {
    LeadingZero, NumRangeExceed, ExclamationMark,
    EmptyChar, IllegalChar, QuotMarkLost,
    IllegalStr, StrRangeExceed, UnknownWord,
    SignedZero, NumErr, NotVarFunc,
    IdRepeat, CharConErr, ConDefTypeErr,
    ArrIndexErr, DefHeadErr, IdNotDefine,
    IllegalFact, IdKindNotMatch, StepLenZero,
    CaseNotCon, ParamCountExceed, IllegalType,
    PrintfSyLost, ScanfSyLost, ReturnSyLost,
    DefaultSyLost, ColonLost, CaseSyLost,
    SwitchSyLost, PlusLost, ForSyLost,
    IfSyLost, BecomesSyLost, VoidSyLost,
    LeftBraceLost, RightBraceLost, LeftParentLost,
    RightParentLost, RightBrackLost, IdLost,
    SemicolonLost, AssignSyLost, TypeDefLost,
    StepLenLost, StateErr, IllegalSyntax
} errorType;

// Static elements
extern alpha keywords[];
extern symbol keySymbols[];
extern map<char, symbol> specialSymbols;

extern std::vector<tabElement> idTable;
extern std::vector<tabElement> staticTable;
extern std::vector<infixNotation> infixTable;


extern char line[];
extern char ch;
extern alpha token;
extern symbol sy;
extern int inum;
extern char str[];
extern int level;
extern fstream inputFile, infixFile;

extern int lineLength;
extern int stringLength;

extern int charPtr;
extern int linePtr;
extern std::vector<int> funcPtrs;
extern int idTablePtr;

extern bool errorFlag;
extern bool skipFlag;


void error(int errorNum);
void warn(int warnNum);
void skipUntil(symbol nexts[]);
void setup();


void nextch();
void insymbol();

void insertTable(kind cls, type typ, const char name[], int length, int level, int addr);
void insertStatic(string function, kind cls, type typ, const char name[], int length, int level);
void popElement();
void popLocals();
int lookUp(const char *name);
int lookUpStatic(const char *functionName);
int lookUpStatic(const char *functionName, const char *identifier);
bool isDefinable(char name[]);
int findCurrentFunc();
void printTable();

void program();
void constState();
void constDef();
void globalVarState();
void varState();
void varDef();
void funcWithRetDef();
void funcWithoutRetDef();
void defHead(type* returnType, char* identifier);
void parameter();
void parameterTable();
void complexState();
void mainDef();
type factor(string &infixString);
type term(string &infixString);
type expression(string &infixString);
void assignState();
void ifState();
void judgement(string &infixString);
void loopState();
void conditionState();
void conditionTable(type firstType, string switchElement, string endSwitchLabel);
void conditionBranch(type firstType, string switchElement, string endSwitchLabel);
void conditionDefault();
void funcWithRetCall(string &infixString);
void funcWithoutRetCall();
void valueParamTable(int idIndex);
void returnState();
void scanfState();
void printfState();
void stateList();
void statement();
void integer(string &infixString);

void insertInfix(string ioperator, string operand1, string operand2, string operand3);
void outputInfixes();
string createTempVar();

void optimizeInfixes();

void mipsProgram(string mipsFileName);

void optMipsProgram(string mipsFileName);

#endif
