
#include "tools.h"

int tools::strToInt(std::string str) {
    std::stringstream ss(str);
    int integer;
    ss >> integer;
    return integer;
}

char tools::strToChar(std::string str) {
    std::stringstream ss(str);
    char character;
    ss >> character;
    return character;
}
