#ifndef TRACEPAIRNODE_HPP
#define TRACEPAIRNODE_HPP

#include "myPair.hpp"
#include "myChar.hpp"

/*
   Nodes used to represent an NFA trace as a linked list
*/

template<class State>
class tracePairNode
{
public:
    State state; // represents the state in the trace
    myChar input; // represents the input that gets to the given state; "_" will represent epsilon
    tracePairNode *next;

    tracePairNode<State>(State state, myChar input, tracePairNode* next)
    {
        this->state = state;
        this->input = input;
        this->next = next;
    }
    void setNext(tracePairNode *next)
    {
        this->next = next;
    }
    friend std::ostream &operator<<(std::ostream &os, const tracePairNode &t)
    {
        os << "(" << t.state << ", " << t.input << ")";
        return os;
    }
};

#endif