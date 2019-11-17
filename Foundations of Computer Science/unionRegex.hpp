#ifndef UNIONREGEX_HPP
#define UNIONREGEX_HPP

#include "regex.hpp"

class unionRegex : public regex
{
public:
    regex *left;
    regex *right;
    unionRegex(regex *left, regex *right)
    {
        isEpsilon = 0;
        isEmptySet = 0;
        isCharacter = 0;
        isUnion = 1;
        isConcat = 0;
        isKleene = 0;
        this->left = left;
        this->right = right;
    }
    void print()
    {
        std::cout << "(";
        left->print();
        std::cout << " U ";
        right->print();
        std::cout << ")";
    }
    myString* generator()
    {
        int random = rand() % 2;  // randomly choose whether to call left or right branch
        
        if (random == 0)
            return left->generator();
        else
            return right->generator();
        
    }
};

#endif