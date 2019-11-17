#ifndef CHARREGEX_HPP
#define CHARREGEX_HPP

#include "regex.hpp"
#include "myChar.hpp"

class charRegex : public regex
{
 public:
    charRegex(myChar c) : isEpsilon(0), isEmptySet(0), isCharacter(1), isUnion(0), isConcat(0), isKleene(0), c(c) {}
    myChar c;
};

#endif