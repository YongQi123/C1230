
#include "main.h"

struct DAGNode {
    int number;
    string content;
    std::set<DAGNode*> parents;
    DAGNode* leftChild;
    DAGNode* rightChild;
};
int lastNum = 0;
std::vector<DAGNode*> allNodes;
map<string, int> varNodeTable; 
map<string, int> varsWithInitial; 
std::set<string> crossingVars;
string currentFunc2;
std::vector<infixNotation> newInfixTable;
fstream betterInfixFile;
string newOutputBuff;

inline bool isManageable(string ioperater) {
    if (ioperater == "ADD" || ioperater == "SUB" || ioperater == "MUL" ||
            ioperater == "DIV" || ioperater == "NEG" || ioperater == "ASSIGN" ||
            ioperater == "GETARR") {
        return true;
    }
    return false;
}

inline bool isMidNode(DAGNode *dagNode) {
    if (dagNode->leftChild != NULL || dagNode->rightChild != NULL) {
        return true;
    }
    return false;
}

inline bool isVarName(string operand) {
    if (operand[0] != ' ' && (operand[0] < '0' || operand[0] > '9')) {
        return true;
    }
    return false;
}

inline bool isAlpha(string content) {
    if (content[0] == '_' || (content[0] > 'a' && content[0] < 'z') || (content[0] > 'A' && content[0] < 'Z')) {
        return true;
    }
    return false;
}

DAGNode* findNodeWithNumber(int number) {
    for (int i = 0; i < allNodes.size(); ++i) {
        if (allNodes[i]->number == number) {
            return allNodes[i];
        }
    }
}

void insertNewInfix(string ioperator, string operand1, string operand2, string operand3){
    infixNotation notation;
    notation.ioperator = ioperator;
    notation.operand1 = operand1;
    notation.operand2 = operand2;
    notation.operand3 = operand3;
    newInfixTable.push_back(notation);

    if (ioperator == "CONST") {
        newOutputBuff = newOutputBuff + "const " + operand2 + " " + operand3 + " = "
                     + operand1 + "\n";
    } else if (ioperator == "VAR") {
        if (operand1 != " ") { 
            newOutputBuff = newOutputBuff + operand2 + " " + operand3 + "[" + operand1 + "]" +"\n";
        } else { 
            newOutputBuff = newOutputBuff + operand2 + " " + operand3 + "\n";
        }
    } else if (ioperator == "FUNC") {
        newOutputBuff = newOutputBuff + operand2 + " " + operand3 + "()" + "\n";
    } else if (ioperator == "PARAM") {
        newOutputBuff = newOutputBuff + "param " + operand2 + " " + operand3 + "\n";
    } else if (ioperator == "ADD") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " + " + operand2 + "\n";
    } else if (ioperator == "SUB") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " - " + operand2 + "\n";
    } else if (ioperator == "MUL") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " * " + operand2 + "\n";
    } else if (ioperator == "DIV") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " / " + operand2 + "\n";
    } else if (ioperator == "ASSIGN") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand2 + "\n";
    } else if (ioperator == "NEG") {
        newOutputBuff = newOutputBuff + operand3 + " = -" + operand2 + "\n";
    } else if (ioperator == "EQL") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " == " + operand2 + "\n";
    } else if (ioperator == "NEQ") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " != " + operand2 + "\n";
    } else if (ioperator == "LSS") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " < " + operand2 + "\n";
    } else if (ioperator == "LEQ") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " <= " + operand2 + "\n";
    } else if (ioperator == "GTR") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " > " + operand2 + "\n";
    } else if (ioperator == "GEQ") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + " >= " + operand2 + "\n";
    } else if (ioperator == "PUSH") {
        newOutputBuff = newOutputBuff + "push " + operand3 + "\n";
    } else if (ioperator == "CALL") {
        newOutputBuff = newOutputBuff + "call " + operand3 + "\n";
    } else if (ioperator == "RETURN") {
        newOutputBuff = newOutputBuff + "return " + operand3 + "\n";
    } else if (ioperator == "SETARR") {
        newOutputBuff = newOutputBuff + operand3 + "[" + operand2 + "] = " + operand1 + "\n";
    } else if (ioperator == "GETARR") {
        newOutputBuff = newOutputBuff + operand3 + " = " + operand1 + "[" + operand2 + "]" + "\n";
    } else if (ioperator == "JMP") {
        newOutputBuff = newOutputBuff + "jmp " + operand3 + "\n";
    } else if (ioperator == "BEQ") {
        newOutputBuff = newOutputBuff + "goto " + operand3 + ", if " + operand1 + " == " +
                     operand2 + "\n";
    } else if (ioperator == "BNE") {
        newOutputBuff = newOutputBuff + "goto " + operand3 + ", if " + operand1 + " != " +
                     operand2 + "\n";
    } else if (ioperator == "SCANF") {
        newOutputBuff = newOutputBuff + "scan " + operand3 + "\n";
    } else if (ioperator == "PRINTF") {
        if (operand2 != " " && operand3 != " ") {
            newOutputBuff = newOutputBuff + "print " + operand2 + ", " + operand3 + "\n";
        } else if (operand2 != " " && operand3 == " ") {
            newOutputBuff = newOutputBuff + "print " + operand2 + "\n";
        } else if (operand2 == " " && operand3 != " ") {
            newOutputBuff = newOutputBuff + "print " + operand3 + "\n";
        } else {
            cout << "Can't print nothing!" << endl;
        } 
    } else if (ioperator == "LABEL") {
        newOutputBuff = newOutputBuff + operand3 + "\n";
    }
}


void insertOperands(std::set<string>& varsInBlock, infixNotation notation) {
    if (isVarName(notation.operand1) &&
            varsInBlock.count(notation.operand1) == 0) {
        varsInBlock.insert(notation.operand1);
    }
    if (isVarName(notation.operand2) &&
            varsInBlock.count(notation.operand2) == 0) { 
        varsInBlock.insert(notation.operand2);
    }
    if (isVarName(notation.operand3) &&
            varsInBlock.count(notation.operand3) == 0) { 
        varsInBlock.insert(notation.operand3);
    }
}

void checkVarExistenceCount(map<string, int>& varExistenceCount, std::set<string>& varsInBlock) {
    for (std::set<string>::iterator it = varsInBlock.begin();
         it != varsInBlock.end(); ++it) {
        map<string, int>::iterator varIt;
        varIt = varExistenceCount.find(*it);
        if (varIt != varExistenceCount.end()) { 
            varIt->second++;
        } else {
            varExistenceCount.insert(std::pair<string, int>(*it, 1));
        }
    }
}

void splitBlocks() {
    map<string, int> varExistenceCount;
    std::set<string> varsInBlock;
    bool isInBlock = false;

    for (int i = 0; i < infixTable.size(); ++i) {
        if (isManageable(infixTable[i].ioperator)) { 
            if (!isInBlock) { 
                isInBlock = true;
                checkVarExistenceCount(varExistenceCount, varsInBlock);
                
                varsInBlock.clear();
            }
        } else { 
            if (isInBlock) { 
                isInBlock = false;
                checkVarExistenceCount(varExistenceCount, varsInBlock);
                
                varsInBlock.clear();
            }
        }
        if (infixTable[i].ioperator != "CONST" && infixTable[i].ioperator != "VAR" &&
                infixTable[i].ioperator != "FUNC" && infixTable[i].ioperator != "PARAM") {
            
            insertOperands(varsInBlock, infixTable[i]);
        }
    }

    checkVarExistenceCount(varExistenceCount, varsInBlock);

    for (map<string, int>::iterator it = varExistenceCount.begin(); it != varExistenceCount.end();
            ++it) {
        if (it->second > 1) { 
            crossingVars.insert(it->first);
        }
    }
}
int setUpOperand(string operand, int *lastNum) {
    int operandNum;
    map<string, int>::iterator tempIt;
    tempIt = varNodeTable.find(operand);
    if (tempIt != varNodeTable.end()) { 
        operandNum = tempIt->second;
    } else {
        operandNum = *lastNum;
        if (operand[0] >= '0' && operand[0] <= '9') {

            varNodeTable.insert(std::pair<string, int>(operand, *lastNum));
            DAGNode* node;
            node = new DAGNode;
            node->content = operand;
            node->number = *lastNum;
            node->leftChild = NULL;
            node->rightChild = NULL;
            allNodes.push_back(node); 
            *lastNum = *lastNum + 1;
        } else { 
        
            varsWithInitial.insert(std::pair<string, int>(operand, *lastNum)); 
            varNodeTable.insert(std::pair<string, int>(operand, *lastNum));
            DAGNode* node;
            node = new DAGNode;
            node->content = operand;
            node->number = *lastNum;
            node->leftChild = NULL;
            node->rightChild = NULL;
            allNodes.push_back(node);
            *lastNum = *lastNum + 1;
        }
    }
    return operandNum;
}

void infixToDAGNode(infixNotation notation) {
    int leftOpNum = 0;
    int rightOpNum = 0;
    int resultNum = 0;
    string ioperator = notation.ioperator;
    string leftOperand = notation.operand1;
    string rightOperand = notation.operand2;
    string result = notation.operand3;


    if (ioperator != "ASSIGN") { 
        leftOpNum = setUpOperand(leftOperand, &lastNum);
        rightOpNum = setUpOperand(rightOperand, &lastNum);

        int i;

        for (i = 0; i < allNodes.size(); ++i) {
            if (allNodes[i]->content == ioperator &&
                allNodes[i]->leftChild->number == leftOpNum &&
                allNodes[i]->rightChild->number == rightOpNum) { 
                resultNum = allNodes[i]->number;
                break;
            }
        }
        if (i == allNodes.size()) { 
            resultNum = lastNum;
            ++lastNum;
            DAGNode *leftNode = allNodes[leftOpNum];
            DAGNode *rightNode = allNodes[rightOpNum];
            
            DAGNode* node;
            node = new DAGNode;
            node->number = resultNum;
            node->content = ioperator;
            node->parents.clear();
            node->leftChild = leftNode;
            node->rightChild = rightNode;
            allNodes.push_back(node); 

            leftNode->parents.insert(node);
            rightNode->parents.insert(node);
        } else { 
        }
    } else { 

        rightOpNum = setUpOperand(rightOperand, &lastNum);

        if (isMidNode(allNodes[rightOpNum])) { 
            resultNum = rightOpNum;
        } else { 
            resultNum = lastNum;
            ++lastNum;
            DAGNode *rightNode = allNodes[rightOpNum];

            DAGNode* node;
            node = new DAGNode;
            node->number = resultNum;
            node->content = ioperator;
            node->parents.clear();
            node->leftChild = NULL;
            node->rightChild = rightNode;
            allNodes.push_back(node); 

            rightNode->parents.insert(node);
        }
    }

    map<string, int>::iterator tempIt;
    tempIt = varNodeTable.find(result);
    if (tempIt != varNodeTable.end()) {
        tempIt->second = resultNum;
    } else { 
        varNodeTable.insert(std::pair<string, int>(result, resultNum));
    }
}

void setMidNode(DAGNode* node, std::vector<DAGNode*>& calculationQueue) {
    DAGNode* leftChild = node->leftChild;
    DAGNode* rightChild = node->rightChild;

    calculationQueue.push_back(node);

    std::set<DAGNode*>::iterator tempIt;
    if (leftChild != NULL) {
        tempIt = leftChild->parents.find(node);
        if (tempIt != leftChild->parents.end()) {
            leftChild->parents.erase(tempIt);
        }
    }
    if (rightChild != NULL) {
        tempIt = rightChild->parents.find(node);
        if (tempIt != rightChild->parents.end()) {
            rightChild->parents.erase(tempIt);
        }
    }

    if (leftChild != NULL) {
        if (isMidNode(leftChild) && leftChild->parents.size() == 0) {
            setMidNode(leftChild, calculationQueue);
        }
    }
    if (rightChild != NULL && leftChild != rightChild) {
        if (isMidNode(rightChild) && rightChild->parents.size() == 0) {
            setMidNode(rightChild, calculationQueue);
        }
    }
}

void exportCodesFromDAG() {
    std::vector<DAGNode*> rootNodes;
    std::vector<DAGNode*> calculationQueue;
    rootNodes.reserve(10000);
    calculationQueue.reserve(10000);

    for (int i = 0; i < allNodes.size(); ++i) {
        if (allNodes[i]->parents.size() == 0) { 
            rootNodes.push_back(allNodes[i]);
        }
    }

    while (rootNodes.size() != 0) {
        setMidNode(rootNodes[0], calculationQueue);
        rootNodes.erase(rootNodes.begin());
    }
    map<string, int>::iterator initIt;
    for (initIt = varsWithInitial.begin(); initIt != varsWithInitial.end();
            ++initIt) {
        map<string, int>::iterator currentIt = varNodeTable.find(initIt->first);
        if (currentIt->second != initIt->second) { 
            string varName = initIt->first;
            string newVarName = initIt->first + "0";
            
            int originalIdIndex;
            int insertIndex;
            for (insertIndex = lookUpStatic(currentFunc2.c_str());
                 staticTable[insertIndex].cls == params; ++insertIndex);
            originalIdIndex = lookUpStatic(currentFunc2.c_str(), varName.c_str());
            if (originalIdIndex == -1) { 
                originalIdIndex = lookUp(varName.c_str());
            }
            insertStatic(currentFunc2, vars, staticTable[originalIdIndex].typ, newVarName.c_str(), 0, 1); // Insert table

            insertNewInfix("ASSIGN", " ", varName, newVarName);

            for (int i = 0; i < allNodes.size(); ++i) {
                if (allNodes[i]->content == varName) {
                    allNodes[i]->content = newVarName;
                    break;
                }
            }
        }
    }

    for (int j = calculationQueue.size() - 1; j >= 0; --j) {
        int nodeNumber = calculationQueue[j]->number;

        map<string, int>::iterator tempIt;
        std::set<string> varNodes;
        for (tempIt = varNodeTable.begin(); tempIt != varNodeTable.end(); ++tempIt) {
            if (nodeNumber == tempIt->second) {
                varNodes.insert(tempIt->first);
            }
        }

        std::set<string> varsToStay;
        std::set<string> varsToLeave;
        std::set<string>::iterator tempIt2;
        DAGNode* node = findNodeWithNumber(nodeNumber);
        DAGNode* leftChild = node->leftChild;
        DAGNode* rightChild = node->rightChild;

        for (tempIt2 = varNodes.begin(); tempIt2 != varNodes.end(); ++tempIt2) {
            std::set<string>::iterator crossingVarIt;
            crossingVarIt = crossingVars.find(*tempIt2);
            if (crossingVarIt != crossingVars.end()) {
                varsToStay.insert(*tempIt2);
            } else { 
                varsToLeave.insert(*tempIt2);
            }
        }
        if (varsToStay.size() == 0) { 
            string varName;
            if (varsToLeave.size() != 0) { 
                varName = *varNodes.begin();
                varsToStay.insert(varName);
                varsToLeave.erase(varsToLeave.find(varName));
            } else { 
                varName = createTempVar();
                varsToStay.insert(varName);
                insertStatic(currentFunc2, vars, ints, varName.c_str(), 0, 1); 
            }

            string operand1;
            string operand2;
            if (leftChild == NULL) {
                operand1 = " ";
            } else {
                operand1 = leftChild->content;
            }
            if (rightChild == NULL) {
                operand2 = " ";
            } else {
                operand2 = rightChild->content;
            }
            insertNewInfix(node->content, operand1, operand2, varName);
        } else {
            infixNotation notation;
            string varName = *varsToStay.begin();

            string operand1;
            string operand2;
            if (leftChild == NULL) {
                operand1 = " ";
            } else {
                operand1 = leftChild->content;
            }
            if (rightChild == NULL) {
                operand2 = " ";
            } else {
                operand2 = rightChild->content;
            }
            insertNewInfix(node->content, operand1, operand2, varName);

            std::set<string>::iterator tempIt;
            tempIt = varsToStay.begin();
            ++tempIt;
            for (; tempIt != varsToStay.end(); ++tempIt) {
                insertNewInfix("ASSIGN", " ", varName, *tempIt);
            }
        }

        node->content = *varsToStay.begin();

        std::set<string>::iterator tempIt3;
        for (tempIt3 = varsToLeave.begin(); tempIt3 != varsToLeave.end(); ++tempIt3) {
            string varName = *tempIt3;
            int staticIndex = lookUpStatic(currentFunc2.c_str(), varName.c_str());
            if (staticIndex != -1) { 
                staticTable.erase(staticTable.begin() + staticIndex);
            }
        }
    }
}

void resettleTableAddr() {
    int staticIndex;
    for (staticIndex = 0; staticTable[staticIndex].cls != funcs ; ++staticIndex); 

    int currentAddr = 0;
    for (; staticIndex < staticTable.size(); ++staticIndex) {
        tabElement* element = &staticTable[staticIndex];

        if (element->cls == funcs) { 
            continue;
        }
        if (staticTable[staticIndex - 1].cls == funcs) { 
            if (element->cls == consts) {
                currentAddr = 0;
            } else { 
                element->addr = 0;
                if (element->cls == vars && element->length != 0) { 
                    currentAddr = element->length;
                } else { 
                    currentAddr = 1;
                }
            }
        } else { 
            if (element->cls == consts) { 
            } else if (element->cls == vars && element->length != 0) { 
                element->addr = currentAddr;
                currentAddr += element->length;
            } else {  
                element->addr = currentAddr;
                ++currentAddr;
            }
        }
    }
}

void optimizeInfixes() {

    splitBlocks();

    newInfixTable.reserve(infixTable.size());
    for (int i = 0; i < infixTable.size(); ++i) {
        if (isManageable(infixTable[i].ioperator)) { 
            infixToDAGNode(infixTable[i]);
        } else {
            if (infixTable[i].ioperator == "FUNC") { 
                currentFunc2 = infixTable[i].operand3;
            }
            if (allNodes.size() != 0 ) { 
                exportCodesFromDAG();
                varsWithInitial.clear();
                allNodes.clear();
                varNodeTable.clear();
                lastNum = 0;
            }
            insertNewInfix(infixTable[i].ioperator, infixTable[i].operand1,
            infixTable[i].operand2, infixTable[i].operand3);
        }
    }

    if (allNodes.size() != 0 ) { 
        exportCodesFromDAG();

        varsWithInitial.clear();
        allNodes.clear();
        varNodeTable.clear();
    }

    resettleTableAddr();

    infixTable.clear();
    infixTable.resize(newInfixTable.size());
    for (int j = 0; j < newInfixTable.size(); ++j) {
        infixTable[j] = newInfixTable[j];
    }

    betterInfixFile.open("betterInfixes.txt", ios::out);
    betterInfixFile << newOutputBuff;
    betterInfixFile.close();
}
