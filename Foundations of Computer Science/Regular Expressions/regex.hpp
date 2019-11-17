#ifndef REGEX_HPP
#define REGEX_HPP

#include <iostream>
#include "NFA.hpp"

template <class State>
class regex
{
 public:
    bool isEpsilon;
    bool isEmptySet;
    bool isCharacter;
    bool isUnion;
    bool isConcat;
    bool isKleene;
};

#endif