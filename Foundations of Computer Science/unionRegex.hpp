#ifndef UNIONREGEX_HPP
#define UNIONREGEX_HPP

#include "regex.hpp"

class unionRegex : public regex
{
public:
    unionRegex(regex &left, regex &right) : left(left), right(right), isEpsilon(0), isEmptySet(0), isCharacter(0), isUnion(1), isConcat(0), isKleene(0) {}
    regex *left;
    regex *right;
};

#endif