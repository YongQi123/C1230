
#include "main.h"

#define isLetter(a) (a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a =='_')
#define isMyAscii(a) (a == 32 || a == 33 || a >= 35 && a <= 126)

const int maxStrLength = 1000;

char ch = ' ';
alpha token;
symbol sy;
int inum;
char str[maxStrLength + 1]; 

char line[maxLineLength + 1]; 
alpha keywords[keywordCount];
symbol keySymbols[keywordCount];
map<char, symbol> specialSymbols;
fstream inputFile, infixFile;

int lineLength = 0;

int charPtr = 0;
int linePtr = 0;

void fatal(){}

void nextch() {
    if (charPtr == lineLength) {
        if (inputFile.eof()) {
            cout << endl;
            cout << "Compiling finished." << endl;
            exit(0);
        }
        lineLength = 0;
        charPtr = 0;
        linePtr++; 
        char tempChar;
        while (inputFile.peek() != '\n' && inputFile.peek() != EOF) { 
            tempChar = inputFile.get();
            line[lineLength] = tempChar;
            lineLength++;
        }
        line[lineLength] = inputFile.get(); 
        lineLength++;
        line[lineLength] = '\0';

    }
    ch = line[charPtr];
    charPtr++;
}

void clearStr() {
    for (int i = 0; i <= maxStrLength; ++i) {
        str[i] = '\0';
    }
}

void clearToken() {
    for (int i = 0; i <= maxIdentLength; ++i) {
        token[i] = '\0';
    }
}

void insymbol() {
    int k = 0;

    while (isspace(ch)) nextch();
    if (isLetter(ch)) { 
        clearToken();

        do {
            if (k < maxIdentLength) {
                token[k] = ch;
                k++;
            }
            nextch();
        } while (isLetter(ch) || isdigit(ch));

        for (k = 0; k < keywordCount; k++) {
            if (strcmp(token, keywords[k]) == 0) {
                break;
            }
        }
        if (k < keywordCount) sy = keySymbols[k];
        else sy = identi;
    } else if (isdigit(ch)) { 
        if (ch == '0') {
            inum = 0;
            sy = intcon;
            nextch();
            if (isdigit(ch)) { 
                error(LeadingZero);
                nextch();
                while (isdigit(ch)){
                    nextch();
                }
            }
        } else { 
            inum = 0;
            sy = intcon;
            for (; isdigit(ch); k++) {
                inum = inum * 10 + ch - '0';
                nextch();
            }
        }
    if( k > maxDigit  ||  inum > maxInt ) {
        error(NumRangeExceed);
        inum = 0;
        k = 0;
    }
    } else if (ch == '<') {
        nextch();
        if (ch == '=') {
            k++;
            sy = leq;
            nextch();
        } else {
            sy = lss;
        }
    } else if (ch == '>') { 
        nextch();
        if (ch == '=') {
            k++;
            sy = geq;
            nextch();
        } else {
            sy = gtr;
        }
    } else if (ch == '=') { 
        nextch();
        if (ch == '=') {
            k++;
            sy = eql;
            nextch();
        } else {
            sy = becomes;
        }
    } else if (ch == '!') {
        nextch();
        if (ch == '=') {
            sy = neq;
            nextch();
        } else {
            error(ExclamationMark);
            sy = neq;
        }
    } else if (ch == '\'') { 
        nextch();
        if (ch == '\'') {
            error(EmptyChar);
            sy = charcon;
            inum = 0;
        } else {
            if (ch != '+' && ch != '-' && ch != '*' && ch != '/'
                    && !isdigit(ch) && !isLetter(ch)) {
                error(IllegalChar);
                sy = charcon;
                inum = 0;
            } else {
                inum = ch; 
                nextch();
                if (ch != '\''){ 
                    errorFlag = true;
                    error(QuotMarkLost);
                    sy = charcon;
                    inum = 0;
                    while (ch != '\'' && ch != '\n') nextch(); 
                    return;
                } else { 
                    sy = charcon; 
                }
            }
        }
        nextch();
    } else if (ch == '"') {
        clearStr();
        nextch();
        if (ch == '"') { 
            sy = stringcon;
            inum = 0;
        } else { 
            k = 0;
            do {
                if (!isMyAscii(ch)) { 
                    error(IllegalStr);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); 
                    return;
                }
                if (k == maxStrLength) { 
                    error(StrRangeExceed);
                    sy = stringcon;
                    inum = 0;
                    while (ch != '"' && isspace(ch)) nextch(); 
                    return;
                }
                str[k] = ch;
                k++;
                if (ch == '\\') { 
                    str[k] = '\\';
                    k++;
                }
                nextch();
            } while (ch != '"');
            sy = stringcon;
            inum = 0;
        }
        nextch();
    } else if (ch == '+' || ch == '-' || ch ==  '*' || ch ==  '/'  ||
        ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' ||
        ch == '}' || ch == ',' || ch == ';' || ch == ':') { 
        sy = specialSymbols[ch];
        nextch();
    } else if (ch == EOF) {
        printTable();
        outputInfixes();
        if (errorFlag) {
            cout << "Compiling failed" << endl;
            exit(0);
        } else {
            cout << "Compiling succeeded." << endl;
        }
        return;
    } else {
        error(UnknownWord); 
        nextch();
        insymbol();
    }
}
