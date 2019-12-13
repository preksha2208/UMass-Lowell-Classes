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
        int random = (rand() % 10); // generate random number 0->9
        if (random == 0)
            return new emptyString;

        myString *ret = next->generator();
        myString *temp = ret;
        while (!temp->next()->isEmpty())  // move to end of currently generate string
            temp = temp->next();
  
        for (int i = 0; i < random; i++)
        {
            temp->setNext(next->generator());
            while (!temp->next()->isEmpty())  // move to end of currently generated string
                temp = temp->next(); 
        }
        return ret;
    }
};

#endif