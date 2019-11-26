#ifndef TRACEPAIRNODE_HPP
#define TRACEPAIRNODE_HPP

#include "myPair.hpp"
#include "myChar.hpp"

/*
   Nodes used to represent an NFA trace as a linked list
*/

template <class State>
class tracePairNode
{
public:
    State current; // the current state in the trace
    State last;  // the state before this state
    myChar input; // represents the input that gets to the given state; "_" will represent epsilon
    bool isFirstState;
    tracePairNode *next; // link to the next element

    tracePairNode<State>(State last, myChar input, State curr, tracePairNode *next)
    {
        this->isFirstState = false;
        this->current = curr;
        this->input = input;
        this->next = next;
        this->last = last;
    }
    tracePairNode<State>(State curr, tracePairNode *next) // used only to construct a node for the first state in a trace
    {
        this->isFirstState = true;
        this->current = curr;
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