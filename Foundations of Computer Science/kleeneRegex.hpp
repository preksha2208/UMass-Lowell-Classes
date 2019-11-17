#ifndef KLEENEREGEX_HPP
#define KLEENEREGEX_HPP

#include "regex.hpp"

class kleeneRegex : public regex
{
public:
    regex *next;
    kleeneRegex(regex *next)
    {
        isEpsilon = 0;
        isEmptySet = 0;
        isCharacter = 0;
        isUnion = 0;
        isConcat = 0;
        isKleene = 1;
        this->next = next;
    }
    void print()
    {
        std::cout << "("; 
        next->print();
        std::cout << ")*";
    }
     
};

#endif