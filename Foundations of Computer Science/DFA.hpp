#ifndef DFA_HPP
#define DFA_HPP

#include <iostream>
#include <vector>
#include "oneString.hpp"
#include "emptyString.hpp"
#include "myVector.hpp"
#include "NFA.hpp"

template <class State>
class DFA
{
public:
  DFA<State>(std::string name, std::function<bool(State &)> Q,
             myVector<myChar> alphabet, State q0,
             std::function<State(State, myChar)> transFunc,
             std::function<bool(State &)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        F(F) {}
  std::string name;
  std::function<bool(State &)> Q; // list of possible states for this dfa
  myVector<myChar> alphabet;
  State q0;                                      // start state
  std::function<State(State, myChar)> transFunc; // transition function
  std::function<bool(State &)> F;                // accept states


  bool accepts(myString &inputString) // does DFA accept inputString?
  {
    State qi = this->q0;
    myString *temp = &inputString;

    // step through DFA with the input string
    while (temp->isEmpty() != true)
    {
      qi = transFunc(qi, temp->charObject());
      temp = temp->next();
    }

    return F(qi); // checks whether arrived-at state is an accept state
  }

  void trace(myString &inputString) // returns the trace for the inputString
  {                                 // does DFA accept inputString?
    State qi = this->q0;
    myString *temp = &inputString;
    std::cout << qi;
    // step through DFA with the input string
    while (temp->isEmpty() == false)
    {
      std::cout << ", ";
      qi = transFunc(qi, temp->charObject());
      std::cout << qi;
      temp = temp->next();
    }
  }

  // returns a string that is accepted by the DFA, or false if it accepts no strings
  std::pair<bool, myString *> acceptedString()
  {
    State qi = q0;
    if (F(qi) == true) // empty string is accepted by this DFA
    {
      return std::make_pair<bool, myString *>(true, new emptyString);
    }
    std::list<State> visitedStates;
    std::stack<myChar> stringStack;
    acceptedStringRecursive(qi, visitedStates, stringStack); // recursive graph search of DFA

    if (stringStack.size() == 0) // graph search found no possible strings
    {
      return std::make_pair<bool, myString *>(false, NULL);
    }
    else // graph search found possible string
    {
      // accepted string is inside of stringStack, so need to convert values to myString*
      oneString beginning = oneString(stringStack.top().getVal(), NULL);
      myString *temp = &beginning;
      while (stringStack.size() != 0)
      {
        temp->setNext(new oneString(stringStack.top().getVal(), NULL));
        stringStack.pop();
        temp = temp->next();
      }
      temp->setNext(new emptyString);

      return std::make_pair<bool, myString *>(true, &beginning);
    }
  }

private:
  bool acceptedStringRecursive(State qi, std::list<State> &visitedStates, std::stack<myChar> &stringStack)
  {
    if (F(qi) == true)
    {
      return true; // current state is accept state
    }
    for (auto x : visitedStates)
    {
      if (qi == x) // current state has already been visited
      {
        return false;
      }
    }
    visitedStates.push_back(qi);

    // recursively call function on all of current state's children
    for (auto x : alphabet)
    {
      if ((acceptedStringRecursive(transFunc(qi, x), visitedStates, stringStack)))
      {
        stringStack.push(x);
        return true;
      }
    }
    return false; // current state and its children are not accept states
  }
};
#endif