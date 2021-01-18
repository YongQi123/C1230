

#include "main.h"
#include <iomanip>

const int startIndex = 0;

std::vector<tabElement> idTable;
std::vector<tabElement> staticTable;
int addressIndex = startIndex;

void insertTable(kind cls, type typ, const char name[], int length, int level, int addr) {
    tabElement element;
    if (level == 0){ 
        if (cls == consts) { 
            element.addr = addr;
        } else if (cls == vars){
            element.addr = addressIndex;
            if (length != 0) { 
                addressIndex = addressIndex + length;
            } else {
                addressIndex++;
            }
        } else { 
            element.addr = 0;
            addressIndex = startIndex;
        }
        strcpy(element.name, name);
        element.cls = cls;
        element.typ = typ;
        element.length = length;
        element.level = level;
        idTable.push_back(element);
        staticTable.push_back(element);
    } else { 
        if (cls == consts) { 
            element.addr = addr;
        } else if (cls == vars || cls == params) {
            element.addr = addressIndex;
            if (length != 0) { 
                addressIndex = addressIndex + length;
            } else {
                addressIndex++;
            }
        }
        strcpy(element.name, name);
        element.cls = cls;
        element.typ = typ;
        element.length = length;
        element.level = level;
        idTable.push_back(element);
        staticTable.push_back(element);
    }
}


void insertStatic(string function, kind cls, type typ, const char name[], int length, int level) {
    tabElement element;
    int insertIndex;

    insertIndex = lookUpStatic(function.c_str());
    if (insertIndex != staticTable.size() - 1) {
        insertIndex++;
    }
    for (; staticTable[insertIndex].cls != funcs && insertIndex < staticTable.size();
         ++insertIndex);
    insertIndex--;

    tabElement lastElement = staticTable[insertIndex];
    if (lastElement.cls == funcs) { 
        element.addr = 0;
    }
    else if (lastElement.cls != vars || lastElement.length == 0) {
        element.addr = lastElement.addr + 1;
    } else { 
        element.addr = lastElement.addr + lastElement.length;
    }
    strcpy(element.name, name);
    element.cls = cls;
    element.typ = typ;
    element.length = length;
    element.level = level;
    staticTable.insert(staticTable.begin() + insertIndex, element);
}

void popElement() {
    idTable.pop_back();
}

void popLocals() {
    for (int i = idTable.size() - 1; i >= 0 ; --i) { 
        if (idTable[i].level != 0) {
            popElement();
        } else { 
            break;
        }
    }
}

int lookUp(const char *name) {
    if (idTable.size() == 0) {
        return -1;
    }
    for (int i = idTable.size() - 1; i >= 0; i--) {
        if (strcmp(idTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int lookUpStatic(const char *functionName) {
    if (staticTable.size() == 0) {
        return -1;
    }
    for (int i = 0; i < staticTable.size(); i++) {
        if (strcmp(staticTable[i].name, functionName) == 0 && staticTable[i].cls == funcs) {
            return i;
        }
    }
    return -1;
}

int lookUpStatic(const char *functionName, const char *identifier) {
    int funcIndex = lookUpStatic(functionName);
    if (staticTable.size() == 0 || funcIndex == -1) {
        return -1;
    }
    for (int i = funcIndex + 1; i < staticTable.size() &&
            (staticTable[i].cls != funcs); ++i) { 
        if (strcmp(staticTable[i].name, identifier) == 0) {
            return i;
        }
    }
    for (int j = 0; j < staticTable.size() &&
                    (staticTable[j].cls != funcs); ++j) { 
        if (strcmp(staticTable[j].name, identifier) == 0) {
            return j;
        }
    }
    return -1;
}

bool isDefinable(char name[]) {
    if (lookUp(name) == -1) { 
        return true;
    } else if (idTable[lookUp(name)].level < level) { 
        return true;
    }
    return false;
}

int findCurrentFunc() {
    if (idTable.size() == 0) {
        return -1;
    }
    for (int i = idTable.size() - 1; i >= 0; --i) {
        if (idTable[i].cls == funcs) {
            return i;
        }
    }
    return -1; 
}

void printTable(){
    cout << endl;
    cout << "----------------------------------Identifier Table---------------------------------" << endl;
    cout << "Name           "
         << "Kind           "
         << "Type           "
         << "Address        "
         << "Length         "
         << "Level          "
         << endl;
    for (int i = 0; i < idTable.size(); ++i) {
        printf("%-*s", 15, idTable[i].name);
        printf("%-*d", 15, idTable[i].cls);
        printf("%-*d", 15, idTable[i].typ);
        printf("%-*d", 15, idTable[i].addr);
        printf("%-*d", 15, idTable[i].length);
        printf("%-*d", 15, idTable[i].level);
        printf("\n");
    }
    cout << "------------------------------------Static Table-----------------------------------" << endl;
    cout << "Name           "
         << "Kind           "
         << "Type           "
         << "Address        "
         << "Length         "
         << "Level          "
         << endl;
    for (int i = 0; i < staticTable.size(); ++i) {
        printf("%-*s", 15, staticTable[i].name);
        printf("%-*d", 15, staticTable[i].cls);
        printf("%-*d", 15, staticTable[i].typ);
        printf("%-*d", 15, staticTable[i].addr);
        printf("%-*d", 15, staticTable[i].length);
        printf("%-*d", 15, staticTable[i].level);
        printf("\n");
    }
}
