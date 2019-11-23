#ifndef NFA_HPP
#define NFA_HPP

#include <iostream>
#include "oneString.hpp"
#include "emptyString.hpp"
#include "myPair.hpp"
#include "myVector.hpp"
#include "DFA.hpp"
#include "tracePairNode.hpp"

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
  bool oracle(myString &inputString, tracePairNode<State> &trace)
  {
    bool isValid = false;
    tracePairNode<State> *tempTrace = &trace; // pointer to first pair node in given trace

    myVector<State> currentStates{this->q0};
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;
    
    if (tempTrace == NULL)
      return false;

    if (tempTrace->state != this->q0)
      return false;  // first state in trace must be the NFA's start state
    tempTrace = tempTrace->next;

    if (temp->isEmpty()) // inputString is empty
    {
      while (tempTrace != NULL)  // check that the trace is an epsi transition from teh start state
      {
        newStates.clear();
        for (State x : currentStates)
        {
          tempVector = this->epsilonTrans(x);  // check for epsilon transitions
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        }

        for (State x : newStates)
        {
          if (tempTrace->state == x && tempTrace->input.getVal() == '_')
            isValid = true;
        }
        if (isValid)
        {
          currentStates = newStates;
          tempTrace = tempTrace->next;  // move to next state in trace
        }
        else
          return false;  // trace state does not match any of the epsi transitions
      }
      return true;   // trace matches one of the epsilon transitions from the NFA's start state
    }

    tempVector = epsilonTrans(this->q0); // check whether there are epsilon transitions from start state
    epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());

    for (State x : epsilonStates) // check whether one of the states reached by epsilon is part of trace
    {
      if (tempTrace->state == x && tempTrace->input.getVal() == '_')
      {
        tempTrace = tempTrace->next; // move to next state in trace so that
        break;
      }
    }
    currentStates.insert(currentStates.end(), epsilonStates.begin(), epsilonStates.end());

    for (State x : currentStates)   // generate new states using transFunc and each current state
      {
        tempVector = transFunc(x, temp->charObject()); 
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
    for (State x : newStates)  // compare newly-generated states with trace
      {
        if (x == tempTrace->state && tempTrace->input == temp->charObject())
        {
          std::cout << "matched trace with one of new states" << std::endl;
          isValid = true;
          tempTrace = tempTrace->next;
          break;
        }
      }
      if (isValid == false)
        return false;
      currentStates = newStates;  // update current states
      std::cout << "trace node before while loop: " << *tempTrace << std::endl;

      temp = temp->next(); // move to next character in the string
    int i = 0;
    // step through NFA with the input string and at each step compare with trace
    while (temp->isEmpty() != true && tempTrace != NULL)
    {
      i++;
      std::cout << "iteration: " << i << std::endl;
      std::cout << "current letter: " << temp->charValue() << std::endl;
      std::cout << "current trace node: " << *tempTrace << std::endl;
      isValid = false;  // starts off false at beginning of each loop
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();

      for (State x : currentStates)
      {
        if (tempTrace->state == x && tempTrace->input == temp->charObject() && isValid == false)
        {
          std::cout << "matching trace with one of current states" << std::endl;
          isValid = true;
          tempTrace = tempTrace->next;
        }
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions from current state
        epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());
      }
      for (State x : epsilonStates) // check whether one of the states reached by epsilon is part of trace
      {
        if (tempTrace->state == x && tempTrace->input.getVal() == '_' && isValid == false)
        {
          isValid = true;
          tempTrace = tempTrace->next; // move to next state in trace so that
          break;
        }
      }
      currentStates.insert(currentStates.end(), epsilonStates.begin(), epsilonStates.end());
      std::cout << "finished checking epsi transitions" << std::endl;
      std::cout << "current trace node: " << *tempTrace << std::endl;
      std::cout << "isValid?: " << isValid << std::endl;
      if (isValid == false)  // if none of the current states are in the trace, then return false
        return false;
      isValid = false;  // reset isValid to false and now time to check the regular transitions from these states

      
      for (State x : currentStates)   // generate new states using transFunc and each current state
      {
        tempVector = transFunc(x, temp->charObject()); 
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      for (State x : newStates)  // compare newly-generated states with trace
      {
        if (x == tempTrace->state && tempTrace->input == temp->charObject())
        {
          std::cout << "matched trace with one of new states" << std::endl;
          isValid = true;
          tempTrace = tempTrace->next;
          break;
        }
      }
      currentStates = newStates;  // prepare for next iteration through loop
      std::cout << "current trace node: " << *tempTrace << std::endl;

      temp = temp->next(); // move to next character in the string
    }
    if (tempTrace == NULL && temp->isEmpty() == false && temp->next()->isEmpty() == false)
      return false;
    return true;
  }

private:
};

#endif