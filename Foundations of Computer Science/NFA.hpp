#ifndef NFA_HPP
#define NFA_HPP

#include <iostream>
#include "oneString.hpp"
#include "emptyString.hpp"
#include "myPair.hpp"
#include "myVector.hpp"
#include "DFA.hpp"

template <class State>
class NFA
{
public:
  NFA<State>(std::string name, std::function<bool(State)> Q,
             myVector<myChar> alphabet, State q0,
             std::function<myVector<State>(State, myChar)> transFunc,
             std::function<myVector<State>(State)> epsilonTrans, std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        epsilonTrans(epsilonTrans), F(F) {}

  std::string name;
  std::function<bool(State &)> Q; // list of possible states for this NFA
  myVector<myChar> alphabet;
  State q0;                                                // start state
  std::function<myVector<State>(State, myChar)> transFunc; // transition function
  std::function<myVector<State>(State)> epsilonTrans;      // returns what the state transitions to on epsilon
  std::function<bool(State &)> F;                          // accept states

  bool accepts(myString &inputString) // does NFA accept inputString?
  {
    myVector<State> currentStates{this->q0}; // keeps track of current states
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;

    if (temp->isEmpty()) // inputString is the emptyString
    {
      tempVector = epsilonTrans(this->q0); // check for epsilon transitions from start state
      currentStates.insert(currentStates.begin(), tempVector.begin(), tempVector.end());
    }

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();

      for (State x : currentStates)
      {
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());
      }
      currentStates.insert(currentStates.end(), epsilonStates.begin(), epsilonStates.end());

      for (State x : currentStates)
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }

      currentStates.clear();
      currentStates = newStates;
      temp = temp->next(); // move to next character in the string
    }

    for (State x : currentStates)
    {
      if (F(x))      // check whether any of the set of current states is an accept state
        return true; // if in accept state, then that means this NFA accepts the input string
    }
    return false; // NFA does not accept the input string
  }

  void traceTree(myString &inputString) // creates tree of all possible traces
  {
    myVector<State> currentStates{this->q0}; // keeps track of current states
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;

    if (temp->isEmpty()) // inputString is the emptyString
    {
      tempVector = epsilonTrans(this->q0); // check for epsilon transitions from start state
      currentStates.insert(currentStates.begin(), tempVector.begin(), tempVector.end());
      std::cout << std::endl;
      std::cout << this->q0 << std::endl;
      for (auto x : currentStates)
        std::cout << x << " ";
    }

    std::cout << std::endl;
    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();

      for (State x : currentStates)
      {
        std::cout << x << " ";
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      std::cout << std::endl; // epsilon transitions from current states will be on next level of tree

      for (State x : currentStates)
      {
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions from current states
        epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());
      }
      if (epsilonStates.size() > 0)
      {
        for (State x : epsilonStates) // print all epsilon transitions
          std::cout << x << " ";
        std::cout << std::endl;
      }

      currentStates = newStates; // update current states for next iteration through
      currentStates.insert(currentStates.end(), epsilonStates.begin(), epsilonStates.end());
      temp = temp->next(); // move to next character in the string
    }
    for (State x : currentStates)
      std::cout << x << " ";
    std::cout << std::endl;
  }

  // returns whether or not the given trace is a valid execution of the NFA
  // returns true or false through isValid boolean
  bool oracle(myString &inputString, myString &trace)
  {
    bool isValid = false;
    myString *tempTrace = &trace; // pointer to first state in given trace

    myVector<State> currentStates{this->q0};
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;

    if (temp->isEmpty()) // inputString is empty
    {
      if (tempTrace->isEmpty() == false && tempTrace->next()->isEmpty())
      {
        tempVector = epsilonTrans(this->q0);
        currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());

        for (myChar x : currentStates)
        {
          if (x.getVal() == tempTrace->charValue())
            return true;
        }
      }
      return false;
    }

    if (tempTrace->isEmpty() != temp->isEmpty())
      return false;

    // step through NFA with the input string and at each step compare with trace
    while (temp->isEmpty() != true && tempTrace->isEmpty() != true)
    {
      isValid = false;
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();

      for (State x : currentStates)
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      for (myChar x : newStates)
      {
        if (x.getVal() == tempTrace->charValue())
        {
          isValid = true;
          tempTrace = tempTrace->next();
          break;
        }
      }

      for (State x : currentStates)
      {
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions from current state
        epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());
      }
      for (myChar x : epsilonStates) // check whether one of the states reached by epsilon is part of trace
      {
        if (x.getVal() == tempTrace->charValue())
        {
          isValid = true;
          tempTrace = tempTrace->next(); // if so, then move to next state in the trace
          break;
        }
      }

      currentStates = newStates;
      currentStates.insert(currentStates.end(), epsilonStates.begin(), epsilonStates.end());
      temp = temp->next(); // move to next character in the string
    }
    return isValid;
  }

private:
};

#endif