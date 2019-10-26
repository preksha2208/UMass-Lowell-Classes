#ifndef NFA_HPP
#define NFA_HPP

#include <iostream>
#include "DFA.hpp"
#include "oneString.hpp"
#include "emptyString.hpp"
#include "myPair.hpp"

template <class State>
class NFA
{
public:
  NFA<State>(std::string name, std::function<bool(State)> Q,
             std::vector<myChar> alphabet, State q0,
             std::function<std::vector<State>(State, myChar)> transFunc,
             std::function<std::vector<State>(State)> epsilonTrans, std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        epsilonTrans(epsilonTrans), F(F) {}
  NFA<State>(const DFA<State> &inputDFA) // converts DFA to NFA
  {
    this->name = inputDFA.name;
    this->Q = inputDFA.Q;
    this->alphabet = inputDFA.alphabet;
    this->q0 = inputDFA.q0;
    this->transFunc = [=](State a, myChar b) -> std::vector<State> {
      return std::vector<State>{inputDFA.transFunc(a, b)};
    };
    this->epsilonTrans = [](State) -> std::vector<State> { return std::vector<State>{}; }; // epsilon transitions don't exist in DFAs
    this->F = inputDFA.F;
  }

  std::string name;
  std::function<bool(State &)> Q; // list of possible states for this dfa
  std::vector<myChar> alphabet;
  State q0;                                                   // start state
  std::function<std::vector<State>(State, myChar)> transFunc; // transition function
  std::function<std::vector<State>(State)> epsilonTrans;      // returns what the state transitions to on epsilon
  std::function<bool(State &)> F;                             // accept states

  bool accepts(myString &inputString) // does NFA accept inputString?
  {
    std::vector<State> currentStates{this->q0}; // keeps track of current states
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc

      for (State x : currentStates) // for each state  in current states
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        if (tempVector.size() != 0)
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        if (tempVector.size() != 0)
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

  bool traceTree(myString &inputString) // creates tree of all possible traces
  {
    std::vector<State> currentStates{this->q0}; // keeps track of current states
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc

      for (State x : currentStates) // for each state  in current states
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        if (tempVector.size() != 0)
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        if (tempVector.size() != 0)
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

  // returns whether or not the given trace is a valid execution of the NFA
  // returns true or false through isValid boolean
  void oracle(myString &inputString, myString &trace, bool &isValid)
  {

    myString *tempTrace = &trace; // pointer to first state in given trace

    if (tempTrace->charValue() != this->q0.charValue())
    {
      isValid = false;
      return; // invalid if first element of trace is not NFA's start state
    }

    std::vector<State> currentStates{this->q0};
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;

    // step through NFA with the input string and at each step compare with trace
    while (temp->isEmpty() != true)
    {
      isValid = false;
      newStates.clear(); // prepare to get new set of states from transFunc
      for (auto x : currentStates)
      {                                                // print out each current state separated by a space
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      currentStates.clear();
      currentStates = newStates;

      for (myChar x : currentStates) // compare current states with given trace
      {
        if (x.getVal() == tempTrace->charValue())
        {
          isValid = true;
          break;
        }
        else
        {
          isValid = false;
        }
      }

      if (isValid == false)
      {
        break; // done stepping through input string if trace has been proven invalid
      }

      tempTrace = tempTrace->next(); // move to next state in trace
      temp = temp->next();           // move to next character in the string
    }
    return;
  }

private:
};

#endif