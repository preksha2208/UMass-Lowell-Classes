#ifndef EMPTYSETREGEX_HPP
#define EMPTYSETREGEX_HPP

#include "regex.hpp"

class emptySetRegex : public regex
{
 public:
    emptySetRegex() 
    {
        isEpsilon = 0;
        isEmptySet = 1;
        isCharacter = 0;
        isUnion = 0;
        isConcat = 0;
        isKleene = 0;
    }
    void print() {}
};

#endif