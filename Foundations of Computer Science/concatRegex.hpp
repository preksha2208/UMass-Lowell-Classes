#ifndef CONCATREGEX_HPP
#define CONCATREGEX_HPP

#include "regex.hpp"

class concatRegex : public regex
{
public:
    regex *left;
    regex *right;
    concatRegex(regex *left, regex *right)
    {
        this->left = left;
        this->right = right;
        isEpsilon = 0;
        isEmptySet = 0;
        isCharacter = 0;
        isUnion = 0;
        isConcat = 1;
        isKleene = 0;
    }
    void print()
    {
        left->print();
        right->print();
    }
};

#endif