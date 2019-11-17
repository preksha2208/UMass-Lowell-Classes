#ifndef CONCATREGEX_HPP
#define CONCATREGEX_HPP

#include "regex.hpp"

template<class State>
class concatRegex : public regex
{
 public:
    emptySetRegex() : isEpsilon(0), isEmptySet(0), isCharacter(0), isUnion(0), isConcat(1), isKleene(0) {}
    regex* next;
};


#endif