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
    myString *generator()
    {
        int random = (rand() % 5) + 1;
        myString* ret = next->generator();
        myString *temp = ret;

        for (int i = 1; i < random; i++)
            temp->setNext(next->generator());
        return ret;
    }
};

#endif