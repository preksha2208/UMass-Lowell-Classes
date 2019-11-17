#ifndef CONCATREGEX_HPP
#define CONCATREGEX_HPP

#include "regex.hpp"

class concatRegex : public regex
{
 public:
    concatRegex() : isEpsilon(0), isEmptySet(0), isCharacter(0), isUnion(0), isConcat(1), isKleene(0) {}
    regex* next;
};


#endif