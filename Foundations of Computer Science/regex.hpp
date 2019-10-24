#ifndef REGEX_HPP
#define REGEX_HPP

#include <iostream>
#include "NFA.hpp"

class regex<State>
{
 public:
    regex(){
        isEmptyString = false;
        isEmptySet = true;
    } // empty set
    regex(emptyString a) {
        isEmptyString = true;
        isEmptySet = false;
    }
    regex(myChar a) {
        isEmptyString = false;
        isEmptySet = false;
    }
    regex(NFA<State> a) {
        isEmptyString = false;
        isEmptySet = false;
    }
    
    bool isEmptyString;
    bool isEmptySet;

    
    


};








#endif