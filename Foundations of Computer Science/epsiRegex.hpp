#ifndef EPSIREGEX_HPP
#define EPSIREGEX_HPP

#include "regex.hpp"

class epsiRegex : public regex
{
 public:
    epsiRegex() : isEpsilon(1), isEmptySet(0), isCharacter(0), isUnion(0), isConcat(0), isKleene(0) {}
};


#endif