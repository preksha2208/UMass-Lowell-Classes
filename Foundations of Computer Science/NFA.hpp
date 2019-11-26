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

    for (int i = 0; i < currentStates.size(); i++)
    {
      tempVector = epsilonTrans(currentStates[i]);
      currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
    }

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();

      for (State x : currentStates)
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      currentStates = newStates;
      std::sort(currentStates.v.begin(), currentStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      currentStates.v.erase(std::unique(currentStates.v.begin(), currentStates.v.end()), currentStates.v.end());

      for (int i = 0; i < currentStates.size(); i++)
      {
        tempVector = epsilonTrans(currentStates[i]);
        currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
      }
      std::sort(newStates.v.begin(), newStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      newStates.v.erase(std::unique(newStates.v.begin(), newStates.v.end()), newStates.v.end());

      temp = temp->next(); // move to next character in the string
    }

    for (State x : currentStates)
    {
      if (F(x))      // check whether any of the set of current states is an accept state
        return true; // if in accept state, then that means this NFA accepts the input string
    }
    return false; // NFA does not accept the input string
  }

  /*
Trace tree function prints each layer of the tree on a new line.
All epsilon transitions from current states are printed within square brackets []
Regular transitions from current states are printed within curly brackets {}
The output format for an epsilon transition is <previous state>/<new state>
The output format for a regular transition is <previous state>/<input>/<new state>
The last line in the output displays the final states in the format <state>/<accepting? YES or NO> 
*/
  void traceTree(myString &inputString) // creates tree of all possible traces
  {
    myVector<State> currentStates{this->q0}; // keeps track of current states. Initial state is always q0
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;
    myChar c;

    std::cout << std::endl;
    std::cout << "{" << this->q0 << "} [";
    for (int i = 0; i < currentStates.size(); i++) // first need to check for any epsilon transition from current state
    {
      tempVector = epsilonTrans(currentStates[i]);
      if (tempVector.size() != 0) // if there exists an epsi transition from the current state
      {
        if (i != 0)
          std::cout << ", ";
        std::cout << currentStates[i] << "/" << tempVector; // prints as <old state>/<new state>
        currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
      }
    }
    std::cout << "]" << std::endl;

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();
      c = temp->charObject(); // stored for more efficient reference

      std::cout << "{";

      for (int i = 0, j = 0; i < currentStates.size(); i++) // check for all epsilon transition from current state
      {
        tempVector = transFunc(currentStates[i], c); // generate new sets of states from input char w/ each current state
        if (tempVector.size() != 0)                  // if there exists are a transition from the current state
        {
          if (i != 0 && j > 0)
            std::cout << ", ";
          j++;                                                            // incremented when there is a return of more than 0 elements from transition func; used to determine whether or not to put a comma before an a given element in output
          std::cout << currentStates[i] << "/" << c << "/" << tempVector; // prints as <old state>/<input>/<new state>
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        }
      }
      std::cout << "}";
      currentStates = newStates; // update newStates
      std::sort(currentStates.v.begin(), currentStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      currentStates.v.erase(std::unique(currentStates.v.begin(), currentStates.v.end()), currentStates.v.end()); // get rid of duplicates

      std::cout << " [";
      for (int i = 0, j = 0; i < currentStates.size(); i++) // check for all epsilon transition from current state
      {
        tempVector = epsilonTrans(currentStates[i]);
        if (tempVector.size() != 0) // if there exists an epsi transition from the current state
        {
          if (i != 0 && j > 0)
            std::cout << ", ";
          j++;
          std::cout << currentStates[i] << "/" << tempVector;
          currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
        }
      }
      std::cout << "]" << std::endl;
      std::sort(newStates.v.begin(), newStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      newStates.v.erase(std::unique(newStates.v.begin(), newStates.v.end()), newStates.v.end()); // get rid of duplicate states

      temp = temp->next(); // move to next character in the string
    }

    std::cout << "{";
    for (State x : currentStates)
    {
      std::cout << " " << x << "/";
      if (F(x))
        std::cout << "YES "; //
      else                   //else
        std::cout << "NO ";
    }
    std::cout << "}" << std::endl
              << std::endl;
  }

  /*
inputString is fed into the NFA and at each step the supplied trace is compared with the current states and how those states were reached
If at a given step the trace does not match any of the current states, then the trace is invalid
If the end of the inputString is reached, but the trace is not yet finished, then the trace is invalid
If the end of the trace is reached, but the inputString is not yet finished, then the trace is invalid
NOTE: an underscore _ is used as to signify an input of epsilon in the trace
*/
  bool oracle(myString &inputString, tracePairNode<State> &trace)
  {
    bool isValid = false;
    myVector<State> currentStates{this->q0}; // keeps track of current states. Initial state is always q0
    myVector<State> tempVector;
    myVector<State> newStates;
    myVector<State> epsilonStates;
    myString *temp = &inputString;
    tracePairNode<State> *tempTrace = &trace; // used to iterate through the trace since it is a linked list
    myChar c;

    if (tempTrace == NULL) // the temp trace must have at least one node in it
      return false;
    else if (tempTrace->isFirstState == false) // if the first node in the trace is not a start state, then it is invalid
      return false;
    else if (tempTrace->current != this->q0) // the first node in the trace must be the NFA's start state, otherwise it is invalid
      return false;
    tempTrace = tempTrace->next; // we know that the first node in the trace is correct, so move to next node

    for (int i = 0; i < currentStates.size(); i++) // first need to check for any epsilon transitions from current state
    {
      tempVector = epsilonTrans(currentStates[i]);
      if (tempVector.size() != 0) // if there exists an epsi transition from the current state
      {
        for (State x : tempVector) // check whether trace follows any of these epsilon transitions
        {

          if (tempTrace == NULL)
            break;
          if (tempTrace->last == currentStates[i] && tempTrace->input.getVal() == '_' && tempTrace->current == x)
            tempTrace = tempTrace->next;
        }
        currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
      }
    }

    // step through NFA with the input string and compare results with trace
    while (temp->isEmpty() != true)
    {
      if (tempTrace == NULL)
        return false;
      
      isValid = false;
      newStates.clear(); // prepare to get new set of states from transFunc
      epsilonStates.clear();
      c = temp->charObject(); // stored for more efficient reference
    
      for (int i = 0, j = 0; i < currentStates.size(); i++) // check for all non-epsilon transitions from current state
      {
        if (isValid == true)  // no need to continue checking if trace match has already been found
          break;
        tempVector = transFunc(currentStates[i], c); // generate new sets of states from input char w/ each current state

        if (tempVector.size() != 0 && isValid != true) // if there exists a transition from the current state
        {
          for (State x : tempVector)
          {
            if (tempTrace->last == currentStates[i] && tempTrace->input == c && tempTrace->current == x)
            {
              isValid = true;
              tempTrace = tempTrace->next;
              break;
            }
          }
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        }
      }
      if (isValid == false)
        return false; // trace is invalid because it did not correspond to any of the transitions
      

      currentStates = newStates; // update newStates
      std::sort(currentStates.v.begin(), currentStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      currentStates.v.erase(std::unique(currentStates.v.begin(), currentStates.v.end()), currentStates.v.end()); // get rid of duplicates

      for (int i = 0; i < currentStates.size(); i++) // need to check for any epsilon transitions from current states
      {
        tempVector = epsilonTrans(currentStates[i]);
        if (tempVector.size() != 0) // if there exists an epsi transition from the current state
        {
          for (State x : tempVector) // check whether tempTrace matches any of the transitions
          {
            if (tempTrace == NULL)
              break; // trace cannot possibly match any of these epsilon transitions because it has reached its end
            if (tempTrace->last == currentStates[i] && tempTrace->input.getVal() == '_' && tempTrace->current == x)
              tempTrace = tempTrace->next;
          }
          currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
        }
      }
      std::sort(newStates.v.begin(), newStates.v.end(), [](const State &lhs, const State &rhs) {
        return (lhs < rhs);
      });
      newStates.v.erase(std::unique(newStates.v.begin(), newStates.v.end()), newStates.v.end()); // get rid of duplicate states

      temp = temp->next(); // move to next character in the string
    }
    return (tempTrace == NULL); // if the trace has not reached its end by this point, then that means that it is invalid
  }

private:
};

#endif