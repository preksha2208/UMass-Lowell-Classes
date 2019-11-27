// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include "mainHeaders.hpp"

template <class State>
NFA<State> regex2NFA(regex *r);
template <class State1, class State2>
NFA<NFAComboState<State1, State2>> concatRegex2NFA(regex *r);
template <class State1, class State2>
NFA<NFAComboState<State1, State2>> unionRegex2NFA(regex *r);
template <class State>
NFA<State> kleeneStarRegex2NFA(regex *r);

NFA<myChar> emptySetRegex2NFA();

NFA<myChar> epsilonRegex2NFA();

NFA<myChar> charRegex2NFA(regex *r);

// generate myString linked list from given std::string
oneString genMyString(std::string &str)
{
  oneString finalStr = oneString(str[0], NULL);
  myString *temp = &finalStr;

  for (int i = 1; i < str.length(); i++)
  {
    temp->setNext(new oneString(str[i], NULL));
    temp = temp->next();
  }
  temp->setNext(new emptyString);
  return finalStr;
}

// Creates DFA that only accepts a string of length one of the inputChar
DFA<myChar> oneCharDFA(myChar inputChar)
{
  return DFA<myChar>(
      "onlyAccepts" + std::string(1, inputChar.getVal()),
      [=](myChar a) -> bool { return (a == myChar('A') || a == myChar('B') || a == myChar('C')); },
      myVector<myChar>{inputChar}, myChar('A'),
      [&](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && (b.getVal() == inputChar.getVal()))
          return myChar('B');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });
}

// Creates DFA that is the complement of given DFA
template <class State>
DFA<State> complementDFA(DFA<State> inputDFA)
{
  return DFA<State>("complement of " + inputDFA.name, inputDFA.Q, inputDFA.alphabet, inputDFA.q0,
                    inputDFA.transFunc, [=](State a) -> bool {
                      return (!inputDFA.F(a));
                    });
}

// Creates DFA that is union of two DFAs
template <class State1, class State2>
DFA<myPair<State1, State2>> unionDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  myVector<myChar> a = dfa1.alphabet;
  myVector<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs
  std::sort(a.v.begin(), a.v.end(), [](const myChar &lhs, const myChar &rhs) {
    return (lhs.getVal() < rhs.getVal());
  });
  a.v.erase(std::unique(a.v.begin(), a.v.end()), a.v.end());

  return DFA<myPair<State1, State2>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                        // alphabet
      myPair<State1, State2>(dfa1.q0, dfa2.q0), // start state
      [=](myPair<State1, State2> a, myChar b) -> myPair<State1, State2> {
        return (myPair<State1, State2>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State1, State2> a) -> bool { // accept states
        return ((dfa1.F(a.first)) || (dfa2.F(a.second)));
      });
}

// Creates DFA that is intersection of two DFAs
template <class State1, class State2>
DFA<myPair<State1, State2>> intersectionDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  myVector<myChar> a = dfa1.alphabet;
  myVector<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs
  std::sort(a.v.begin(), a.v.end(), [](const myChar &lhs, const myChar &rhs) {
    return (lhs.getVal() < rhs.getVal());
  });
  a.v.erase(std::unique(a.v.begin(), a.v.end()), a.v.end());

  return DFA<myPair<State1, State2>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                        // alphabet
      myPair<State1, State2>(dfa1.q0, dfa2.q0), // start state
      [=](myPair<State1, State2> a, myChar b) -> myPair<State1, State2> {
        return (myPair<State1, State2>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State1, State2> a) -> bool {             // accept states
        return ((dfa1.F(a.first)) && (dfa2.F(a.second))); // only difference from unionDFA function
      });
}

// Returns whether first DFA is subset of the second DFA
template <class State1, class State2>
bool subsetDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  DFA<myPair<State1, State2>> dfa3 = intersectionDFA(dfa1, complementDFA(dfa2));
  return (!(dfa3.acceptedString().first)); // if dfa3 accepts nothing, then dfa is subset of dfa2
}

// Returns whether two DFAs are equivalent
template <class State1, class State2>
bool equalityDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  DFA<myPair<myPair<State1, State2>, myPair<State1, State2>>> dfa3 = unionDFA(intersectionDFA(dfa1, complementDFA(dfa2)), intersectionDFA(complementDFA(dfa1), dfa2));
  return (!dfa3.acceptedString().first); // if dfa3 accepts nothing, then dfa1 = dfa2
}

// Converts DFA -> NFA
template <class State>
NFA<State> DFA2NFA(DFA<State> &inputDFA)
{
  return NFA<State>(
      inputDFA.name,
      inputDFA.Q,
      inputDFA.alphabet,
      inputDFA.q0,
      [=](State a, myChar b) -> myVector<State> {
        return myVector<State>{inputDFA.transFunc(a, b)};
      },
      [](State) -> myVector<State> { return myVector<State>{}; }, // epsilon transitions don't exist in DFAs
      inputDFA.F);
}

// Converts NFA -> DFA
template <class State>
DFA<myVector<State>> NFA2DFA(NFA<State> nfa)
{
  myVector<State> start{nfa.q0};
  myVector<State> epsiStart = nfa.epsilonTrans(nfa.q0);
  start.insert(start.end(), epsiStart.begin(), epsiStart.end());

  return DFA<myVector<State>>(
      nfa.name + " in DFA form",
      [=](myVector<State> &a) -> bool {
        for (State x : a)
        {
          if (!nfa.Q(x))
            return false;
        }
        return true; // all elements of input vector are valid nfa states
      },
      nfa.alphabet, // uses same alphabet as given nfa
      start,
      [=](myVector<State> a, myChar b) -> myVector<State> { // transition function
        myVector<State> tempVector;
        myVector<State> newStates;
        myVector<State> currentStates = a;
        myVector<State> epsilonStates;

        for (int i = 0; i < currentStates.size(); i++)
        {
          tempVector = nfa.epsilonTrans(currentStates[i]);
          currentStates.insert(currentStates.end(), tempVector.begin(), tempVector.end());
        }
        std::sort(currentStates.v.begin(), currentStates.v.end(), [](const State &lhs, const State &rhs) {
          return (lhs < rhs);
        });
        currentStates.v.erase(std::unique(currentStates.v.begin(), currentStates.v.end()), currentStates.v.end());

        for (State x : currentStates) // generate new sets of states from input char w/ each current state
        {
          tempVector = nfa.transFunc(x, b);
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        }

        std::sort(newStates.v.begin(), newStates.v.end(), [](const State &lhs, const State &rhs) {
          return (lhs < rhs);
        });
        newStates.v.erase(std::unique(newStates.v.begin(), newStates.v.end()), newStates.v.end());

        return newStates; // return new state generated from the current state
      },
      [=](myVector<State> &a) -> bool {
        for (State x : a)
        {
          if (nfa.F(x)) // make states in vector are all accepted by the original nfa
            return true;
        }
        return false;
      });
}

// Creates NFA that is the union of two NFAs
template <class State1, class State2>
NFA<NFAComboState<State1, State2>> unionNFA(NFA<State1> nfa1, NFA<State2> nfa2)
{
  typedef NFAComboState<State1, State2> nfaState;
  myVector<myChar> a = nfa1.alphabet;
  myVector<myChar> b = nfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both NFAs
  std::sort(a.v.begin(), a.v.end(), [](const myChar &lhs, const myChar &rhs) {
    return (lhs.getVal() < rhs.getVal());
  });
  a.v.erase(std::unique(a.v.begin(), a.v.end()), a.v.end());

  return NFA<nfaState>(
      "Union of " + nfa1.name + " and " + nfa2.name,
      [=](nfaState a) -> bool { // Q function
        if (a.isStartState)
          return true;
        else if (a.isFromX)
          return nfa1.Q(a.fromX); // need to check whether this state actually exists in nfa1
        else
          return nfa2.Q(a.fromY);
      },
      a,           // combined alphabets
      nfaState(0), // start state object
      [=](nfaState a, myChar b) -> myVector<nfaState> {
        if (a.isStartState)
          return myVector<nfaState>{};
        else if (a.isAcceptState)
          return myVector<nfaState>{};
        else if (a.isFromX)
        {
          myVector<State1> xVec = nfa1.transFunc(a.fromX, b); // get State1 objects from nfa1's transition function
          std::sort(xVec.v.begin(), xVec.v.end(), [](const State1 &lhs, const State1 &rhs) {
            return (lhs < rhs);
          });
          xVec.v.erase(std::unique(xVec.v.begin(), xVec.v.end()), xVec.v.end());

          myVector<nfaState> xStateVec;
          for (State1 x : xVec) // create vector of nfaState objects out of State1 objects
            xStateVec.push_back(nfaState(x, 1));
          return xStateVec;
        }
        else
        {
          myVector<State2> yVec = nfa2.transFunc(a.fromY, b); // get State2 objects from nfa2's transition function
          std::sort(yVec.v.begin(), yVec.v.end(), [](const State2 &lhs, const State2 &rhs) {
            return (lhs < rhs);
          });
          yVec.v.erase(std::unique(yVec.v.begin(), yVec.v.end()), yVec.v.end());

          myVector<nfaState> yStateVec;
          for (State2 y : yVec) // create vector of nfaState objects out of State2 objects
            yStateVec.push_back(nfaState(1, y));
          return yStateVec;
        }
      },
      [=](nfaState a) -> myVector<nfaState> { // epsilon transition function
        if (a.isStartState)
          return myVector<nfaState>{nfaState(nfa1.q0, 1), nfaState(1, nfa2.q0)};
        else if (a.isFromX)
        {
          myVector<State1> xVec = nfa1.epsilonTrans(a.fromX); // get State1 objects from nfa1's epsiTrans function
          std::sort(xVec.v.begin(), xVec.v.end(), [](const State1 &lhs, const State1 &rhs) {
            return (lhs < rhs);
          });
          xVec.v.erase(std::unique(xVec.v.begin(), xVec.v.end()), xVec.v.end());

          myVector<nfaState> xStateVec;
          for (State1 x : xVec)
            xStateVec.push_back(nfaState(x, 1));
          return xStateVec;
        }
        else
        {
          myVector<State2> yVec = nfa2.epsilonTrans(a.fromY); // get State2 objects from nfa2's epsiTrans function
          std::sort(yVec.v.begin(), yVec.v.end(), [](const State2 &lhs, const State2 &rhs) {
            return (lhs < rhs);
          });
          yVec.v.erase(std::unique(yVec.v.begin(), yVec.v.end()), yVec.v.end());

          myVector<nfaState> yStateVec;
          for (State2 y : yVec)
            yStateVec.push_back(nfaState(1, y));
          return yStateVec;
        }
      },
      [=](nfaState a) -> bool {
        if (a.isFromX)
          return nfa1.F(a.fromX);
        else if (a.isFromY)
          return nfa2.F(a.fromY);
        else
          return false;
      });
}

// Creates NFA that is the concatenation of two NFAs
template <class State1, class State2>
NFA<NFAComboState<State1, State2>> concatenationNFA(NFA<State1> nfa1, NFA<State2> nfa2)
{
  typedef NFAComboState<State1, State2> nfaState;
  myVector<myChar> a = nfa1.alphabet;
  myVector<myChar> b = nfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both NFAs
  std::sort(a.v.begin(), a.v.end(), [](const myChar &lhs, const myChar &rhs) {
    return (lhs.getVal() < rhs.getVal());
  });
  a.v.erase(std::unique(a.v.begin(), a.v.end()), a.v.end());

  return NFA<nfaState>(
      "Concatenation of " + nfa1.name + " and " + nfa2.name,
      [=](nfaState a) -> bool { // Q function
        if (a.isStartState)
          return true;
        else if (a.isAcceptState)
          return true;
        else if (a.isFromX)
          return nfa1.Q(a.fromX);
        else if (a.isFromY)
          return nfa2.Q(a.fromY);
        else
          return false;
      },
      a,                    // combined alphabets
      nfaState(nfa1.q0, 1), // nfa 1's start state is first nfa's start state
      [=](nfaState a, myChar b) -> myVector<nfaState> {
        if (a.isFromX) // the state is within nfa1
        {
          //std::cout << "calling nfa1 trans function" << std::endl;

          myVector<State1> xVec = nfa1.transFunc(a.fromX, b);
          myVector<nfaState> xStateVec;
          for (State1 x : xVec) // create vector of nfaState objects out of State1 objects
            xStateVec.push_back(nfaState(x, 1));
          return xStateVec;
        }
        else // the state is within nfa2
        {
          //  std::cout << "calling nfa2 trans function" << std::endl;

          myVector<State2> yVec = nfa2.transFunc(a.fromY, b);
          myVector<nfaState> yStateVec;
          for (State2 y : yVec)
            yStateVec.push_back(nfaState(1, y));
          return yStateVec;
        }
      },
      [=](nfaState a) -> myVector<nfaState> { // epsilon transition function
        if (a.isFromX)
        {
          myVector<State1> xVec = nfa1.epsilonTrans(a.fromX);
          myVector<nfaState> xStateVec;
          if (nfa1.F(a.fromX))
          {
            xStateVec.push_back(nfaState(1, nfa2.q0)); // nfa1's accept state epsilon transitions to nfa2's start state
          }
          for (State1 x : xVec)
            xStateVec.push_back(nfaState(x, 1));

          return xStateVec;
        }
        else if (a.isFromY)
        {
          myVector<State2> yVec = nfa2.epsilonTrans(a.fromY);
          myVector<nfaState> yStateVec;

          for (State2 y : yVec)
            yStateVec.push_back(nfaState(1, y));

          return yStateVec;
        }
        else
          return myVector<nfaState>{};
      },
      [=](nfaState a) -> bool {
        if (a.isFromY)
        {
          // std::cout << "is " << a << " an accept state of nfa2? " << std::endl;
          return nfa2.F(a.fromY);
        }
        else
          return false;
      });
}

// Creates NFA that is the Kleene star of the given NFA
NFA<myChar> kleeneStarNFA(NFA<myChar> nfa)
{
  return NFA<myChar>(
      nfa.name + "*",
      [=](myChar a) -> bool {
        if (a == myChar('|'))
          return true;
        else
          return nfa.Q(a);
      },
      nfa.alphabet, myChar('|'), // start state may have to be changed to something else
      [=](myChar a, myChar b) -> myVector<myChar> {
        if (a == myChar('|'))
          return myVector<myChar>{};
        else
          return nfa.transFunc(a,b);
      },
      [=](myChar a) -> myVector<myChar> {
        if (a == myChar('|'))
          return myVector<myChar>{nfa.q0};
        else if (nfa.F(a))
          return myVector<myChar>{nfa.q0};
        else
          return nfa.epsilonTrans(a);
      },
      [=](myChar a) -> bool {
        if (a == myChar('|'))
          return true;
        else
          return nfa.F(a);
      });
}

// generate nth string of alphabet's lexicographical ordering
myString *lexi(int n, std::list<myChar> alphabet)
{
  return NULL;
}

void regexPrinter(regex &r)
{
  r.print();
  std::cout << std::endl;
}

template <class State>
NFA<State> regex2NFA(regex *r)
{
  // checks what the regex type is for regex pointer r
  if (r->isCharacter)
    return charRegex2NFA(r);
  else if (r->isEmptySet)
    return emptySetRegex2NFA();
  else if (r->isEpsilon)
    return epsilonRegex2NFA();
  else if (r->isUnion)
    return unionRegex2NFA<State>(r);
  else if (r->isConcat)
    return concatRegex2NFA<State>(r);
  else
    return kleeneStarRegex2NFA<State>(r);
}

NFA<myChar> charRegex2NFA(regex *r)
{
  return NFA<myChar>("charRegex2NFA", // name
                     [=](myChar a) -> bool {
                       return (a.getVal() == 'A' || a.getVal() == 'B');
                     },                                            // states function
                     myVector<myChar>{r->c},                       // alphabet
                     myChar('A'),                                  // start state
                     [=](myChar a, myChar b) -> myVector<myChar> { // transition function
                       if (a.getVal() == 'A' && b.getVal() == r->c.getVal())
                         return myVector<myChar>{myChar('B')};
                       else
                         return myVector<myChar>{}; // may not be correct
                     },
                     [](myChar a) -> myVector<myChar> { // epsilon transition
                       return myVector<myChar>{};
                     },
                     [=](myChar a) -> bool { // accept states
                       return (a.getVal() == 'B');
                     });
}

NFA<myChar> emptySetRegex2NFA()
{
  return NFA<myChar>("only accepts epsilon", // name
                     [](myChar a) -> bool {
                       return (a.getVal() == 'A' || a.getVal() == 'B');
                     },                                           // states function
                     myVector<myChar>{},                          // alphabet
                     myChar('A'),                                 // start state
                     [](myChar a, myChar b) -> myVector<myChar> { // transition function
                       return myVector<myChar>{};
                     },
                     [](myChar a) -> myVector<myChar> { // epsilon transition
                       return myVector<myChar>{};
                     },
                     [](myChar a) -> bool { // accept states
                       return false;
                     });
}

NFA<myChar> epsilonRegex2NFA()
{
  return NFA<myChar>("only accepts epsilon", // name
                     [](myChar a) -> bool {
                       return (a.getVal() == 'A' || a.getVal() == 'B');
                     },                                           // states function
                     myVector<myChar>{},                          // alphabet
                     myChar('A'),                                 // start state
                     [](myChar a, myChar b) -> myVector<myChar> { // transition function
                       return myVector<myChar>{};
                     },
                     [](myChar a) -> myVector<myChar> { // epsilon transition
                       if (a.getVal() == 'A')
                         return myVector<myChar>{myChar('B')};
                       else
                         return myVector<myChar>{};
                     },
                     [](myChar a) -> bool { // accept states
                       return (a.getVal() == 'B');
                     });
}

template <class State1, class State2>
NFA<NFAComboState<State1, State2>> concatRegex2NFA(regex *r)
{
  return concatenationNFA(regex2NFA<State1>(r->left), regex2NFA<State2>(r->right));
}

template <class State1, class State2>
NFA<NFAComboState<State1, State2>> unionRegex2NFA(regex *r)
{
  return unionNFA(regex2NFA<State1>(r->left), regex2NFA<State2>(r->right));
}

template <class State>
NFA<State> kleeneStarRegex2NFA(regex *r)
{
  return kleeneStarNFA(regex2NFA<State>(r));
}

// Takes in dfa, vector of test strings and expected values for the test strings on the given dfa
// Boolean values are at same index in bool vector as their corresponding test string in the other vector
template <class State>
void DFAtester(DFA<State> &a, myVector<myString *> &testStrings, myVector<bool> &expected)
{
  int passed = 0; // keep track of tests passed or failed
  int failed = 0;

  for (int i = 0; i < testStrings.size(); i++)
  {
    std::cout << i + 1 << ": ";
    if (a.accepts(*testStrings[i]) == expected[i])
    {
      std::cout << "Test Passed!";
      passed++;
    }
    else
    {
      std::cout << "Test Failed.";
      failed++;
    }
    std::cout << " --- Trace: ";
    a.trace(*testStrings[i]);
    std::cout << std::endl;
  }

  std::cout << "TESTS PASSED: " << passed << std::endl; // print out results
  std::cout << "TESTS FAILED: " << failed << std::endl;
  std::cout << std::endl;
}

// Creates DFAs and runs tests on them
void makeAndTestDFAs()
{
  // Declarations of DFAs
  DFA<myChar> evenLengthBinary( // returns whether length of inputted binary
                                // number is even
      "EvenLengthBinary",       // name
      [](myChar a) -> bool {    // state function
        return ((a.getVal() == 'A') || (a.getVal() == 'B') || (a.getVal() == 'C'));
      },
      myVector<myChar>{myChar('0'), myChar('1')}, // alphabet
      myChar('A'),                                // start state
      [](myChar a, myChar b) -> myChar {          // transition function
        if ((a.getVal() == 'A') && ((b.getVal() == '0') || (b.getVal() == '1')))
          return myChar('B');
        else if ((a.getVal() == 'B') &&
                 ((b.getVal() == '0') || (b.getVal() == '1')))
          return myChar('A');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { // accept states
        return (a.getVal() == 'A');
      });

  DFA<myChar> acceptsNothing( // accepts nothing, as the name implies
      "AcceptsNothing", [](myChar a) -> bool { return false; },
      myVector<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar { return a; },
      [](myChar a) -> bool { return false; });

  DFA<myChar> onlyAcceptsEmptyString( // only accepted input is the empty string
      "OnlyAcceptsEmptyString",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      myVector<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        return myChar('B');
      },
      [](myChar a) -> bool { return (a == myChar('A')); });
  DFA<myChar> evenBinaryNumber( // returns whether the inputted binary number is even
      "EvenBinaryNumber",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '0')
          return myChar('B');
        else if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '1')
          return myChar('A');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });

  DFA<myChar> oddBinaryNumber( // returns whether the inputted binary number is odd
      "OddBinaryNumber",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') || (a.getVal() == 'C'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '1')
          return myChar('B');
        else if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '0')
          return myChar('A');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });

  DFA<myChar> containsCAM( // returns whether the input contains my name
                           // somewhere in it
      "ContainsCAM",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      myVector<myChar>{myChar('C'), myChar('A'), myChar('M')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (b.getVal() == 'C')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == 'A')
          return myChar('C');
        else if (a.getVal() == 'C' && b.getVal() == 'M')
          return myChar('D');
        else if (a.getVal() == 'D')
          return myChar('D'); // contains CAM, so we will remain in accept state
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> containsLineComment( // returns whether the input contains a double slash
                                   // somewhere in it, indicating a comment
      "ContainsLineComment",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C'));
      },
      myVector<myChar>{myChar('/')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '/')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '/')
          return myChar('C');
        else if (a.getVal() == 'C')
          return myChar('C');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('C')); });

  DFA<myChar> threeConsecutiveZerosBinary( // returns whether the inputted binary number
                                           // has three consecutive zeros in it
      "ThreeConsecutiveZerosBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D') || (a.getVal() == 'E'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (((a.getVal() == 'C') || (a.getVal() == 'B' || (a.getVal() == 'A'))) && b.getVal() == '1')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '0')
          return myChar('C');
        else if (a.getVal() == 'C' && b.getVal() == '0')
          return myChar('D');
        else if (a.getVal() == 'D')
          return myChar('D');
        else
          return myChar('E');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> threeConsecutiveOnesBinary( // returns whether the inputted binary number
                                          // has three consecutive ones in it
      "ThreeConsecutiveOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D') || (a.getVal() == 'E'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if ((a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C') && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '1')
          return myChar('C');
        else if (a.getVal() == 'C' && b.getVal() == '1')
          return myChar('D');
        else if (a.getVal() == 'D')
          return myChar('D');
        else
          return myChar('E');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> oddNumberOfOnesBinary( // returns whether the inputted binary
                                     // number has an odd number of ones in it
      "OddNumberOfOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') || (a.getVal() == 'C'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '1')
          return myChar('A');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });
  DFA<myChar> evenNumberOfOnesBinary( // returns whether the inputted number has
                                      // an even number of ones in it
      "NumberOfOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') || (a.getVal() == 'C'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '1')
          return myChar('A');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('A')); });

  DFA<myChar> evenNumberOfZerosAndSingleOne( // returns whether the inputted number has
                                             // an even number of zeroes followed by a
                                             // single one in it
      "EvenNumberOfZerosAndSingleOne",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('C');
        else if (a.getVal() == 'C' && b.getVal() == '1')
          return myChar('D');
        else if (a.getVal() == 'D')
          return myChar('D');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('C')); });

  // Declarations of string objects used in tests
  oneString OZ = oneString('1', new oneString('0', new emptyString));
  oneString ZO = oneString('0', new oneString('1', new emptyString));
  oneString OZOO = oneString(
      '1', new oneString(
               '0', new oneString('1', new oneString('1', new emptyString))));
  oneString ZZZZ = oneString(
      '0', new oneString(
               '0', new oneString('0', new oneString('0', new emptyString))));
  oneString ZOZO = oneString(
      '0', new oneString(
               '1', new oneString('0', new oneString('1', new emptyString))));
  oneString OO = oneString('1', new oneString('1', new emptyString));
  oneString ZZZOO = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '0', new oneString('1', new oneString('1', new emptyString)))));
  oneString OZZZO = oneString(
      '1',
      new oneString(
          '0',
          new oneString(
              '0', new oneString('0', new oneString('1', new emptyString)))));
  oneString OOZZZ = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              '0', new oneString('0', new oneString('0', new emptyString)))));
  oneString ZOZOOO = oneString(
      '0',
      new oneString(
          '1',
          new oneString(
              '0', new oneString(
                       '1', new oneString(
                                '1', new oneString('1', new emptyString))))));
  oneString ZOZZZ = oneString(
      '0',
      new oneString(
          '1',
          new oneString(
              '0', new oneString('0', new oneString('0', new emptyString)))));
  oneString ZZ = oneString('0', new oneString('0', new emptyString));
  oneString ZZZZO = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '0', new oneString('0', new oneString('1', new emptyString)))));
  oneString OOOOOO = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              '1', new oneString(
                       '1', new oneString(
                                '1', new oneString('1', new emptyString))))));

  emptyString epsi;
  oneString OOOZZZ = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              '1', new oneString(
                       '0', new oneString(
                                '0', new oneString('0', new emptyString))))));
  oneString ZOOOZZ = oneString(
      '0',
      new oneString(
          '1',
          new oneString(
              '1', new oneString(
                       '1', new oneString(
                                '0', new oneString('0', new emptyString))))));
  oneString ZZOOOZ = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '1', new oneString(
                       '1', new oneString(
                                '1', new oneString('0', new emptyString))))));
  oneString ZZZOOO = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '0', new oneString(
                       '1', new oneString(
                                '1', new oneString('1', new emptyString))))));
  oneString O = oneString('1', new emptyString);
  oneString Z = oneString('0', new emptyString);
  oneString OOO =
      oneString('1', new oneString('1', new oneString('1', new emptyString)));
  oneString ZOZ =
      oneString('0', new oneString('1', new oneString('0', new emptyString)));
  oneString ZZZZZ = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '0', new oneString('0', new oneString('0', new emptyString)))));
  oneString ZOZOZ = oneString(

      '0',
      new oneString(
          '1',
          new oneString(
              '0', new oneString('1', new oneString('0', new emptyString)))));
  oneString ZZO =
      oneString('0', new oneString('0', new oneString('1', new emptyString)));
  oneString ZZZZZZO = oneString(
      '0',
      new oneString(
          '0',
          new oneString(
              '0', new oneString(
                       '0', new oneString(
                                '0', new oneString(
                                         '0', new oneString(
                                                  '1', new emptyString)))))));
  oneString CAMOO = oneString(
      'C',
      new oneString(
          'A',
          new oneString(
              'M', new oneString('1', new oneString('1', new emptyString)))));
  oneString OOCAM = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              'C', new oneString('A', new oneString('M', new emptyString)))));
  oneString OCAMO = oneString(
      '1',
      new oneString(
          'C',
          new oneString(
              'A', new oneString('M', new oneString('1', new emptyString)))));
  oneString CACAMM = oneString(
      'C',
      new oneString(
          'A',
          new oneString(
              'C', new oneString(
                       'A', new oneString(
                                'M', new oneString('M', new emptyString))))));
  oneString CACM = oneString(
      'C', new oneString(
               'A', new oneString('C', new oneString('M', new emptyString))));
  oneString CAMCAM = oneString(
      'C',
      new oneString(
          'A',
          new oneString(
              'M', new oneString(
                       'C', new oneString(
                                'A', new oneString('M', new emptyString))))));
  oneString CAMERA = oneString(
      'C',
      new oneString(
          'A',
          new oneString(
              'M', new oneString(
                       'E', new oneString(
                                'R', new oneString('A', new emptyString))))));
  oneString MAC =
      oneString('M', new oneString('A', new oneString('C', new emptyString)));
  oneString comment1 =
      oneString('/', new oneString('/', new oneString('H', new emptyString)));
  oneString comment2 =
      oneString('H', new oneString('/', new oneString('/', new emptyString)));
  oneString comment3 =
      oneString('/', new oneString('H', new oneString('/', new emptyString)));
  oneString comment4 =
      oneString('/', new oneString('/', new oneString('/', new emptyString)));
  oneString comment5 = oneString(
      '/', new oneString(
               '/', new oneString('/', new oneString('H', new emptyString))));
  oneString comment6 = oneString(
      'H', new oneString(
               '/', new oneString('/', new oneString('/', new emptyString))));
  oneString comment7 = oneString('/', new emptyString);
  oneString a = oneString('a', new emptyString);
  oneString b = oneString('b', new emptyString);
  oneString c = oneString('c', new emptyString);
  oneString ccc = oneString('c', new oneString('c', new oneString('c', new emptyString)));
  oneString d = oneString('d', new emptyString);
  oneString e = oneString('e', new emptyString);
  oneString f = oneString('f', new emptyString);

  std::cout << std::boolalpha; // makes the console print true/false rather than 1/0
  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA CLASS TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenLengthBinary DFA: " << std::endl; // Tests for EvenLengthBinaryDFA
  myVector<myString *> evenLengthStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedEvenLength{true, true, true, true, true, true, false, false, false, false, false, false};
  DFAtester(evenLengthBinary, evenLengthStrings, expectedEvenLength);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing AcceptsNothing DFA" << std::endl; // tests for AcceptsNothing DFA
  myVector<myString *> acceptsNothingStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedAcceptsNothing{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(acceptsNothing, acceptsNothingStrings, expectedAcceptsNothing);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OnlyAcceptsEmptyString DFA" << std::endl; // tests for onlyAcceptsEmptyString DFA
  myVector<myString *> onlyAcceptsEmptyStringStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedOnlyAcceptsEmptyString{false, false, false, false, false, true, false, false, false, false, false, false};
  DFAtester(onlyAcceptsEmptyString, onlyAcceptsEmptyStringStrings, expectedOnlyAcceptsEmptyString);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenBinaryNumber DFA"
            << std::endl; // tests for evenBinaryNumber DFA
  myVector<myString *> evenBinaryNumberStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedEvenBinaryNumber{true, false, false, true, false, false, false, true, true, true, false, true};
  DFAtester(evenBinaryNumber, evenBinaryNumberStrings, expectedEvenBinaryNumber);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddBinaryNumber DFA" << std::endl; // tests for OddBinaryNumber DF
  myVector<myString *> oddBinaryNumberStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedOddBinaryNumber{false, true, true, false, true, false, true, false, false, false, true, false};
  DFAtester(oddBinaryNumber, oddBinaryNumberStrings, expectedOddBinaryNumber);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsCAM DFA"
            << std::endl; // tests for containsCAM DFA
  myVector<myString *> ContainsCAMStrings{&OZ, &ZO, &OZOO, &ZZZZ, &MAC, &epsi, &CAMOO, &OOCAM, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  myVector<bool> expectedContainsCAM{false, false, false, false, false, false, true, true, true, true, true, true};
  DFAtester(containsCAM, ContainsCAMStrings, expectedContainsCAM);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsLineComment DFA" << std::endl; // tests for containsLineComment DFA
  myVector<myString *> containsLineCommentStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  myVector<bool> expectedContainsLineComment{true, true, false, true, true, false, true, false, false, false, false, false};
  DFAtester(containsLineComment, containsLineCommentStrings, expectedContainsLineComment);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveZerosBinary DFA" << std::endl; // tests for threeConsecutiveZerosBinary DFA
  myVector<myString *> threeConsecutiveZerosStrings{&ZZZZ, &ZZZOO, &ZOZZZ, &ZZZZZ, &OOZZZ, &epsi, &OZZZO, &Z, &MAC, &OZ, &OOO, &ZOZOZ};
  myVector<bool> expectedThreeConsecutiveZeros{true, true, true, true, true, false, true, false, false, false, false, false};
  DFAtester(threeConsecutiveZerosBinary, threeConsecutiveZerosStrings, expectedThreeConsecutiveZeros);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveOnesBinary DFA"
            << std::endl; // tests for threeConsecutiveOnesBinary DFA
  myVector<myString *> threeConsecutiveOnesStrings{&ZZZZ, &ZZZOO, &ZOZZZ, &ZZZZZ, &OOZZZ, &epsi, &ZZZZZ, &OOOZZZ, &ZOOOZZ, &ZZOOOZ, &OOO, &ZZZOOO};
  myVector<bool> expectedConsecutiveOnes{false, false, false, false, false, false, false, true, true, true, true, true};
  DFAtester(threeConsecutiveOnesBinary, threeConsecutiveOnesStrings, expectedConsecutiveOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddNumberOfOnesBinary DFA"
            << std::endl; // tests for oddNumberOfOnesBinary DFA
  myVector<myString *> oddNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedOddNumberOfOnes{true, true, true, false, false, false, true, false, true, false, true, false};
  DFAtester(oddNumberOfOnesBinary, oddNumberOfOnesStrings, expectedOddNumberOfOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfOnesBinary DFA"
            << std::endl; // tests for EvenNumberOfOnesBinary DFA
  myVector<myString *> evenNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedEvenNumberOfOnes{false, false, false, true, true, true, false, true, false, true, false, true};
  DFAtester(evenNumberOfOnesBinary, evenNumberOfOnesStrings, expectedEvenNumberOfOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfZerosAndSingleOne DFA"
            << std::endl; // tests for EvenNumberOfZerosAndSingleOne DFA
  myVector<myString *> evenNumberOfZerosAndSingleOneStrings{&OZ, &ZOZO, &ZZZZZZO, &ZZO, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZO, &OOO, &ZOZOZ};
  myVector<bool> expectedEvenNumberZerosAndSingleOne{false, false, true, true, false, false, true, false, false, true, false, false};
  DFAtester(evenNumberOfZerosAndSingleOne, evenNumberOfZerosAndSingleOneStrings, expectedEvenNumberZerosAndSingleOne);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA UNION TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing union of oddNumberOfOnesBinary and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> anyNumberOfOnesBinary = unionDFA<myChar>(oddNumberOfOnesBinary, evenNumberOfOnesBinary);
  myVector<myString *> anyNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedAnyNumberOfOnes{true, true, true, true, true, true, true, true, true, true, true, true};
  DFAtester(anyNumberOfOnesBinary, anyNumberOfOnesStrings, expectedAnyNumberOfOnes);

  std::cout << "Testing union of OddBinaryNumber and EvenBinaryNumber DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> evenOrOddBinary = unionDFA<myChar>(evenBinaryNumber, oddBinaryNumber);
  myVector<myString *> evenOrOddBinaryStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedEvenOrOddBinary{true, true, true, true, true, false, true, true, true, true, true, true};
  DFAtester(evenOrOddBinary, evenOrOddBinaryStrings, expectedEvenOrOddBinary);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA INTERSECTION TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing intersection of oddNumberOfOnesBinary and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> noNumberOfOnesBinary = intersectionDFA<myChar>(oddNumberOfOnesBinary, evenNumberOfOnesBinary);
  myVector<myString *> noNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedNoNumberOfOnes{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(noNumberOfOnesBinary, noNumberOfOnesStrings, expectedNoNumberOfOnes);

  std::cout << "Testing intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> lineCommentAndEvenNumberOfOnes = intersectionDFA<myChar>(containsLineComment, evenNumberOfOnesBinary);
  myVector<myString *> lineCommentAndEvenNumberOfOnesStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  myVector<bool> expectedlineCommentAndEvenNumberOfOnes{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(lineCommentAndEvenNumberOfOnes, lineCommentAndEvenNumberOfOnesStrings, expectedlineCommentAndEvenNumberOfOnes);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA COMPLEMENT TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing complement of intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> lineCommentAndEvenNumberOfOnesComp = complementDFA<myPair<myChar, myChar>>(lineCommentAndEvenNumberOfOnes);
  myVector<myString *> lineCommentAndEvenNumberOfOnesCompStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  myVector<bool> expectedlineCommentAndEvenNumberOfOnesComp{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(lineCommentAndEvenNumberOfOnesComp, lineCommentAndEvenNumberOfOnesCompStrings, expectedlineCommentAndEvenNumberOfOnesComp);

  std::cout << "Testing complement of oddNumberOfOnesBinary DFA" << std::endl;
  DFA<myChar> oddNumberOfOnesBinaryComp = complementDFA<myChar>(oddNumberOfOnesBinary);
  myVector<myString *> oddNumberOfOnesBinaryCompStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  myVector<bool> expectedOddNumberOfOnesBinaryComp{false, false, false, true, true, true, false, true, false, true, false, true};
  DFAtester(oddNumberOfOnesBinaryComp, oddNumberOfOnesBinaryCompStrings, expectedOddNumberOfOnesBinaryComp);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      ONE CHAR DFA TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing oneCharDFA onlyAcceptsC" << std::endl;
  DFA<myChar> onlyAcceptsC = oneCharDFA(myChar('c'));
  myVector<myString *> onlyAcceptsCStrings{&a, &b, &c, &d, &e, &f, &ccc, &epsi, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  myVector<bool> expectedOnlyAcceptsCStrings{false, false, true, false, false, false, false, false, false, false, false, false};
  DFAtester(onlyAcceptsC, onlyAcceptsCStrings, expectedOnlyAcceptsCStrings);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                  DFA EQUALITY FUNCTION TESTS                  " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Is OddNumberOfOnesBinary == oddNumberOfOnesBinary? " << equalityDFA(oddNumberOfOnesBinary, oddNumberOfOnesBinary);
  std::cout << std::endl;
  std::cout << "Is OddNumberOfOnesBinary == EvenNumberOfOnesBinary? " << equalityDFA(oddNumberOfOnesBinary, evenNumberOfOnesBinary);
  std::cout << std::endl;
  std::cout << "Is OnlyAcceptsC == OnlyAcceptsC? " << equalityDFA(onlyAcceptsC, onlyAcceptsC);
  std::cout << std::endl;
  std::cout << "Is OnlyAcceptsC == complement of intersection of containsLineComment and evenNumberOfOnesBinary DFAs? "
            << equalityDFA(onlyAcceptsC, lineCommentAndEvenNumberOfOnesComp);
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                   DFA SUBSET FUNCTION TESTS                   " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Is OddNumberOfOnesBinary a subset of the union of OddNumberOfOnesBinary and EvenNumberOfOnesBinary? " << subsetDFA(oddNumberOfOnesBinary, unionDFA(oddNumberOfOnesBinary, evenNumberOfOnesBinary));
  std::cout << std::endl;
  std::cout << "Is OddNumberOfOnesBinary a subset of the complement of itself? " << subsetDFA(oddNumberOfOnesBinary, complementDFA(oddNumberOfOnesBinary));
  std::cout << std::endl;
  std::cout << "Is OnlyAcceptsEmptyString a subset of EvenLengthBinary? " << subsetDFA(onlyAcceptsEmptyString, evenLengthBinary);
  std::cout << std::endl;
  std::cout << "Is ContainsCAM a subset of ContainsLineComment? " << subsetDFA(containsCAM, containsLineComment);
  std::cout << std::endl;
  std::cout << "Is AcceptsNothing a subset of ContainsLineComment? " << subsetDFA(acceptsNothing, containsLineComment);
  std::cout << std::endl;

  DFA<myChar> anyBinary(     // returns whether it is a binary number
      "AnyBinary",           // name
      [](myChar a) -> bool { // state function
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      myVector<myChar>{myChar('1'), myChar('0')}, // alphabet
      myChar('B'),                                // start state
      [](myChar a, myChar b) -> myChar {          // transition function
        if ((a.getVal() == 'A' || a.getVal() == 'B') && ((b.getVal() == '0') || (b.getVal() == '1')))
          return myChar('A');
        else
          return myChar('B');
      },
      [](myChar a) -> bool { // accept states
        return (a.getVal() == 'A');
      });

  DFA<myChar> acceptsEverything( // accepts any given input
      "AcceptsEverything",       // name
      [](myChar a) -> bool {     // state function
        return (a.getVal() == 'A');
      },
      myVector<myChar>{},                // alphabet
      myChar('A'),                       // start state
      [](myChar a, myChar b) -> myChar { // transition function
        return myChar('A');
      },
      [](myChar a) -> bool { // accept states
        return (a.getVal() == 'A');
      });

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "   VERIFYING UNION, COMPLEMENT, AND INTERSECT USING EQUALITY                " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;
  std::cout << "Is AnyBinary == (union of OddBinaryNumber and EvenBinaryNumber)? " << equalityDFA(anyBinary, unionDFA(oddBinaryNumber, evenBinaryNumber));
  std::cout << std::endl;
  std::cout << "Is OddBinaryNumber == (union of OddBinaryNumber and AcceptsNothing)? " << equalityDFA(oddBinaryNumber, unionDFA(oddBinaryNumber, acceptsNothing));
  std::cout << std::endl;
  std::cout << "Is AcceptsEverything == (complement of AcceptsNothing)? " << equalityDFA(acceptsEverything, complementDFA(acceptsNothing));
  std::cout << std::endl;
  std::cout << "Is AcceptsNothing == complement of (intersection of ContainsCAM and EvenBinaryNumber)? " << equalityDFA(acceptsNothing, intersectionDFA(containsCAM, evenBinaryNumber));
  std::cout << std::endl
            << std::endl;
}

// creates NFAs and runs tests on them
void makeAndTestNFAs()
{
  // example NFAs
  NFA<myChar> oneIsThirdFromEnd("OneIsThirdFromEnd", // name
                                [](myChar a) -> bool {
                                  return (a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C' || a.getVal() == 'D' || a.getVal() == 'E');
                                },                                           // states function
                                myVector<myChar>{myChar('0'), myChar('1')},  // alphabet
                                myChar('A'),                                 // start state
                                [](myChar a, myChar b) -> myVector<myChar> { // transition function
                                  if (a.getVal() == 'A' && b.getVal() == '1')
                                    return myVector<myChar>{myChar('A'), myChar('B')};
                                  else if (a.getVal() == 'A' && b.getVal() == '0')
                                    return myVector<myChar>{myChar('A')};
                                  else if (a.getVal() == 'B' && (b.getVal() == '0' || b.getVal() == '1'))
                                    return myVector<myChar>{myChar('C')};
                                  else if (a.getVal() == 'C' && (b.getVal() == '1' || b.getVal() == '0'))
                                    return myVector<myChar>{myChar('D')};
                                  else if (a.getVal() == 'D' && (b.getVal() == '1' || b.getVal() == '0'))
                                    return myVector<myChar>{myChar('A')};
                                  else
                                    return myVector<myChar>{}; // return empty set
                                },
                                [](myChar a) -> myVector<myChar> { // epsilon transition
                                  return myVector<myChar>{};
                                },
                                [](myChar a) -> bool { // accept states
                                  return (a.getVal() == 'D');
                                });

  NFA<myChar> numZerosIsMultipleOfTwoOrThree("NumZerosIsMultipleOfTwoOrThree", // name
                                             [](myChar a) -> bool {
                                               return (a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C' || a.getVal() == 'D' || a.getVal() == 'E' || a.getVal() == 'F' || a.getVal() == 'G');
                                             },                                           // states function
                                             myVector<myChar>{myChar('0'), myChar('1')},  // alphabet
                                             myChar('A'),                                 // start state
                                             [](myChar a, myChar b) -> myVector<myChar> { // transition function
                                                                                          // if (a.getVal() == 'A')
                                               //return myVector<myChar>{a};
                                               if (a.getVal() == 'B' && b.getVal() == '0')
                                                 return myVector<myChar>{myChar('C')};
                                               else if (a.getVal() == 'C' && b.getVal() == '0')
                                                 return myVector<myChar>{myChar('B')};
                                               else if (a.getVal() == 'D' && b.getVal() == '0')
                                                 return myVector<myChar>{myChar('E')};
                                               else if (a.getVal() == 'E' && b.getVal() == '0')
                                                 return myVector<myChar>{myChar('F')};
                                               else if (a.getVal() == 'F' && b.getVal() == '0')
                                                 return myVector<myChar>{myChar('D')};
                                               else if (b.getVal() == '1')
                                                 return myVector<myChar>{a};
                                               else
                                                 return myVector<myChar>{};
                                             },
                                             [](myChar a) -> myVector<myChar> { // epsilon transition
                                               if (a.getVal() == 'A')
                                                 return myVector<myChar>{myChar('B'), myChar('D')};
                                               else
                                                 return myVector<myChar>{};
                                             },
                                             [](myChar a) -> bool { // accept states
                                               return (a.getVal() == 'B' || a.getVal() == 'D');
                                             });

  NFA<myChar> containsOZOorOO("ContainsOZOorOO", // name
                              [](myChar a) -> bool {
                                return (a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C' || a.getVal() == 'D' || a.getVal() == 'E');
                              },                                           // states function
                              myVector<myChar>{myChar('0'), myChar('1')},  // alphabet
                              myChar('A'),                                 // start state
                              [](myChar a, myChar b) -> myVector<myChar> { // transition function
                                if (a.getVal() == 'A' && b.getVal() == '1')
                                  return myVector<myChar>{myChar('A'), myChar('B')};
                                else if (a.getVal() == 'A' && b.getVal() == '0')
                                  return myVector<myChar>{myChar('A')};
                                else if (a.getVal() == 'B' && (b.getVal() == '0'))
                                  return myVector<myChar>{myChar('C')};
                                else if (a.getVal() == 'C' && (b.getVal() == '1'))
                                  return myVector<myChar>{myChar('D')};
                                else if (a.getVal() == 'D' && (b.getVal() == '1' || b.getVal() == '0'))
                                  return myVector<myChar>{myChar('D')};
                                else
                                  return myVector<myChar>{}; // return empty set
                              },
                              [](myChar a) -> myVector<myChar> { // epsilon transition
                                if (a.getVal() == 'B')
                                  return myVector<myChar>{myChar('C')};
                                else
                                  return myVector<myChar>{};
                              },
                              [](myChar a) -> bool { // accept states
                                return (a.getVal() == 'D');
                              });

 
  // strings for tests. I started being smarter and created a genMyString function after a while.
  oneString OZZ = oneString('1', new oneString('0', new oneString('0', new emptyString)));
  oneString OZ = oneString('1', new oneString('0', new emptyString));
  oneString OO = oneString('1', new oneString('1', new emptyString));
  oneString OOZ = oneString('1', new oneString('1', new oneString('0', new emptyString)));
  oneString OOOZ = oneString('1', new oneString('1', new oneString('1', new oneString('0', new emptyString))));
  oneString OZOO = oneString('1', new oneString('0', new oneString('1', new oneString('1', new emptyString))));
  oneString OOZOO = oneString('1', new oneString('1', new oneString('0', new oneString('1', new oneString('1', new emptyString)))));
  emptyString epsi;
  oneString ZZ = oneString('0', new oneString('0', new emptyString));
  oneString ZZZ = oneString('0', new oneString('0', new oneString('0', new emptyString)));
  oneString ZZZZZ = oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('0', new emptyString)))));
  std::string s000 = "111";
  oneString OOO = genMyString(s000);
  std::string sOZZOOO = "100111";
  oneString OZZOOO = genMyString(sOZZOOO);
  std::string sOZO = "101";
  oneString OZO = genMyString(sOZO);
  std::string sZZZZ = "0000";
  oneString ZZZZ = genMyString(sZZZZ);
  std::string sZ = "0";
  oneString Z = genMyString(sZ);
  std::string sO = "1";
  oneString O = genMyString(sO);
  std::string sZOZZ = "0100";
  oneString ZOZZ = genMyString(sZOZZ);
  std::string sOZZZ = "1000";
  oneString OZZZ = genMyString(sOZZZ);
  std::string sOZZO = "1001";
  oneString OZZO = genMyString(sOZZO);
  std::string sZZZZZOO = "0000011";
  oneString ZZZZZOO = genMyString(sZZZZZOO);
  std::string sZZZZZO = "000001";
  oneString ZZZZZO = genMyString(sZZZZZO);
  std::string sOZZOZ = "10010";
  oneString OZZOZ = genMyString(sOZZOZ);
  std::string s1 = "10000";
  oneString OZZZZ = genMyString(s1);
  std::string sOZZOO = "10011";
  oneString OZZOO = genMyString(sOZZOO);
  std::string sOZZZZZ = "100000";
  oneString OZZZZZ = genMyString(sOZZZZZ);
  std::string sOZZOZO = "100101";
  oneString OZZOZO = genMyString(sOZZOZO);
  std::string sOOOOO = "11111";
  oneString OOOOO = genMyString(sOOOOO);
  std::string sOZOZOZZ = "1010100";
  oneString OZOZOZZ = genMyString(sOZOZOZZ);
  std::string sOZZZOOOZ = "10001110";
  oneString OZZZOOOZ = genMyString(sOZZZOOOZ);
  std::string s2 = "100100";
  oneString OZZOZZ = genMyString(s2);
  std::string s3 = "100100100";
  oneString OZZOZZOZZ = genMyString(s3);
  std::string sABCD= "ABCD";
  oneString ABCD = genMyString(sABCD);
  std::string s4 = "10111";
  oneString OZOOO = genMyString(s4);
  std::string s5 = "11101";
  oneString OOOZO = genMyString(s5);
  std::string s6 = "101123";
  oneString OZOO23 = genMyString(s6);

  // tests for oneIsThirdFromEnd
  std::cout << std::boolalpha;
  std::cout << "Does oneIsThirdFromEnd accept OZZ (should be true)? " << oneIsThirdFromEnd.accepts(OZZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OOZ (should be true)? " << oneIsThirdFromEnd.accepts(OOZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OOOZ (should be true)? " << oneIsThirdFromEnd.accepts(OOOZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OZOO (should be false)? " << oneIsThirdFromEnd.accepts(OZOO) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept the empty string (should be false)? " << oneIsThirdFromEnd.accepts(epsi) << std::endl;
  std::cout << std::endl;

  // tests for numZerosIsMultipleOfTwoOrThree
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the empty string (should be true)? " << numZerosIsMultipleOfTwoOrThree.accepts(epsi) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZ (should be true)? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZZ (should be true)? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZZZZ (should be false)? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept OOZ (should be false)? " << numZerosIsMultipleOfTwoOrThree.accepts(OOZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept OZOO (should be false)? " << numZerosIsMultipleOfTwoOrThree.accepts(OZOO) << std::endl;
  std::cout << std::endl;

  // tests for containsOZOorOO
  std::cout << "Does containsOZOorOO accept the empty string (should be false)? " << containsOZOorOO.accepts(epsi) << std::endl;
  std::cout << "Does containsOZOorOO accept OZOO (should be true)? " << containsOZOorOO.accepts(OZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept ZZZ (should be false)? " << containsOZOorOO.accepts(ZZZ) << std::endl;
  std::cout << "Does containsOZOorOO accept OOOZ (should be true)? " << containsOZOorOO.accepts(OOOZ) << std::endl;
  std::cout << "Does containsOZOorOO accept OOZOO (should be true)? " << containsOZOorOO.accepts(OOZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept OO (should be true)? " << containsOZOorOO.accepts(OO) << std::endl;
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    Oracle Function Tests                      " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;
  typedef tracePairNode<myChar> tpNode; // NOTE: constructor is tracePairNode<State>(State state, myChar input, tracePairNode* next)

  // traces
  tpNode tp1 = tpNode(myChar('A'), NULL);
  tpNode tp2 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('_'), myChar('B'), NULL));
  tpNode tp3 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('_'), myChar('B'), new tpNode(myChar('B'), myChar('_'), myChar('C'), NULL)));
  tpNode tp4 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('_'), myChar('B'),
                                              new tpNode(myChar('B'), myChar('0'), myChar('C'),
                                                         new tpNode(myChar('C'), myChar('0'), myChar('B'), NULL))));
  tpNode tp5 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('_'), myChar('D'),
                                              new tpNode(myChar('D'), myChar('0'), myChar('E'),
                                                         new tpNode(myChar('E'), myChar('0'), myChar('F'), NULL))));
  tpNode tp6 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('A'),
                                              new tpNode(myChar('A'), myChar('1'), myChar('A'),
                                                         new tpNode(myChar('A'), myChar('1'), myChar('A'), NULL))));
  tpNode tp7 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('B'),
                                              new tpNode(myChar('B'), myChar('0'), myChar('C'),
                                                         new tpNode(myChar('C'), myChar('0'), myChar('D'), NULL))));
  tpNode tp8 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('B'),
                                              new tpNode(myChar('B'), myChar('0'), myChar('C'),
                                                         new tpNode(myChar('C'), myChar('0'), myChar('D'),
                                                                    new tpNode(myChar('D'), myChar('1'), myChar('A'),
                                                                               new tpNode(myChar('A'), myChar('1'), myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('A'), NULL)))))));
  tpNode tp9 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('A'), NULL)));
  tpNode tp10 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('B'),
                                               new tpNode(myChar('B'), myChar('0'), myChar('C'),
                                                          new tpNode(myChar('C'), myChar('1'), myChar('D'), NULL))));
  tpNode tp11 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('B'),
                                               new tpNode(myChar('B'), myChar('_'), myChar('C'),
                                                          new tpNode(myChar('C'), myChar('1'), myChar('D'), NULL))));
  tpNode tp12 = tpNode(myChar('A'), new tpNode(myChar('A'), myChar('1'), myChar('B'),
                                               new tpNode(myChar('B'), myChar('0'), myChar('C'), NULL)));
  

  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of emptyString, and trace A (should return true) " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, tp1);
  std::cout << std::endl;
  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of emptyString, and trace AB (should return true) " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, tp2);
  std::cout << std::endl;
  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of emptyString, and trace ABC (should return false) " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, tp3);
  std::cout << std::endl;
  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of 00, and trace ABCB (should return true) " << numZerosIsMultipleOfTwoOrThree.oracle(ZZ, tp4);
  std::cout << std::endl;
  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of 00, and trace ADEF (should return true) " << numZerosIsMultipleOfTwoOrThree.oracle(ZZ, tp5);
  std::cout << std::endl;
  std::cout << "testing oracle with numZerosIsMultipleOfTwoOrThree, input of 00000, and trace ADEF (should return false) " << numZerosIsMultipleOfTwoOrThree.oracle(ZZZZZ, tp5);
  std::cout << std::endl << std::endl;

  std::cout << "testing oracle with oneIsThirdFromEnd, input of 111, and trace AAAA (should return true) " << oneIsThirdFromEnd.oracle(OOO, tp6);
  std::cout << std::endl;
  std::cout << "testing oracle with oneIsThirdFromEnd, input of 100, and trace ABCD (should return true) " << oneIsThirdFromEnd.oracle(OZZ, tp7);
  std::cout << std::endl;
  std::cout << "testing oracle with oneIsThirdFromEnd, input of 100, and trace AB (should return false) " << oneIsThirdFromEnd.oracle(OZZ, tp2);
  std::cout << std::endl;
  std::cout << "testing oracle with oneIsThirdFromEnd, input of 100111, and trace ABCDAAA (should return true) " << oneIsThirdFromEnd.oracle(OZZOOO, tp8);
  std::cout << std::endl;
  std::cout << "testing oracle with oneIsThirdFromEnd, input of 100111, and trace AB (should return false) " << oneIsThirdFromEnd.oracle(OZZOOO, tp2);
  std::cout << std::endl;
  std::cout << "testing oracle with oneIsThirdFromEnd, input of 100111, and trace ABCD (should return false) " << oneIsThirdFromEnd.oracle(OZZOOO, tp7);
  std::cout << std::endl << std::endl;

  std::cout << "testing oracle with containsOZOorOO, input of 11, and trace AAA (should return true) " << containsOZOorOO.oracle(OO, tp9);
  std::cout << std::endl;
  std::cout << "testing oracle with containsOZOorOO, input of 11, and trace ABCD (should return true) " << containsOZOorOO.oracle(OO, tp11);
  std::cout << std::endl;
  std::cout << "testing oracle with containsOZOorOO, input of 101, and trace ABCD (should return true) " << containsOZOorOO.oracle(OZO, tp10);
  std::cout << std::endl;
  std::cout << "testing oracle with containsOZOorOO, input of 10, and trace ABC (should return true) " << containsOZOorOO.oracle(OZ, tp12);
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    Trace Tree Function Tests                      " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  // example trace trees
  std::string tree1 = "{A} [A/(BD)] -> {A/NO B/YES D/NO}"; // numZerosIsMultipleOfTwoOrThree with emptyString

  std::string tree2 = "{A} [A/(BD)] -> {B/0/C, D/0/E} [] -> {C/NO E/NO}"; // numZerosIsMultipleOfTwoOrThree with 0

  std::string tree3 = "{A} [A/(BD)] -> {B/0/C, D/0/E} [] -> {C/0/B E/0/F} [] -> {B/NO F/NO}"; // numZerosIsMultipleOfTwoOrThree with 00

  std::string tree4 = "{A} [A/(BD)] -> {B/0/C, D/0/E} [] -> {C/0/B E/0/F} [] -> {B/0/C, F/0/D} [] -> {C/NO D/YES}"; // numZerosIsMultipleOfTwoOrThree with 000

  std::string tree5 = "{A} [A/(BD)] -> {B/0/C, D/0/E} [] -> {C/0/B E/0/F} [] -> {B/0/C, F/0/D} [] -> {C/0/B, D/0/E} [] -> {B/NO E/YES}"; // numZerosIsMultipleOfTwoOrThree with 0000

  std::string tree6 = "{A} [] -> {A/NO}"; // containsOZOorOO with emptyString

  std::string tree7 = "{A} -> {A/1/(AB)} [B/C] -> {A/1/(AB), C/1/D} [B/C] { A/NO B/NO C/NO D/YES }"; // containsOZOorOO with 11

  std::string tree8 = "{A} -> {A/1/(AB)} [B/C] -> {A/0/A, B/0/C} [] -> {A/0/A} [] { A/NO }"; // containsOZOorOO with 100

  std::string tree9 = "{A} -> {A/1/(AB)} [B/C] -> {A/0/A, B/0/C} [] -> {A/1(AB), C/1/D} [B/C] { A/NO B/NO C/NO D/YES }"; // containsOZOorOO with 101

  std::string tree10 = "{A} [] -> {A/NO}"; // oneIsThirdFromEnd with emptyString

  std::string tree11 = "{A} [] -> {A/0/A} [] -> {A/1/(AB)} [] -> {A/0/A, B/0/C} [] -> {A/0/A, C/0/D} [] -> { A/NO D/YES }"; // oneIsThirdFromEnd with 0100

  std::string tree12 = "{A} [] -> {A/1/AB} [] -> { A/NO B/NO }"; // oneIsThirdFromEnd with 1

  std::string tree13 = "{A} [] -> {A/0/A} [] -> { A/NO }"; // oneIsThirdFromEnd with 0

  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with emptyString: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(epsi);
  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with 0: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(Z);
  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with 00: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(ZZ);
  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with 000: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(ZZZ);
  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with 0000: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(ZZZZ);
  std::cout << "Trace tree of containsOZOorOO with emptyString: ";
  containsOZOorOO.traceTree(epsi);
  std::cout << "Trace tree of containsOZOorOO with 11: ";
  containsOZOorOO.traceTree(OO);
  std::cout << "Trace tree of containsOZOorOO with 100: ";
  containsOZOorOO.traceTree(OZZ);
  std::cout << "Trace tree of containsOZOorOO with 101: ";
  containsOZOorOO.traceTree(OZO);
  std::cout << "Trace tree of oneIsThirdFromEnd with emptyString: ";
  oneIsThirdFromEnd.traceTree(epsi);
  std::cout << "Trace tree of oneIsThirdFromEnd with 0100: ";
  oneIsThirdFromEnd.traceTree(ZOZZ);
  std::cout << "Trace tree of oneIsThirdFromEnd with 1: ";
  oneIsThirdFromEnd.traceTree(O);
  std::cout << "Trace tree of oneIsThirdFromEnd with 0: ";
  oneIsThirdFromEnd.traceTree(Z);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Union Tests                           " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  auto unionNFA1 = unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd);
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept the emptyString (should be true)? " << unionNFA1.accepts(epsi);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept 111 (should be true)? " << unionNFA1.accepts(OOO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept 100 (should be true)? " << unionNFA1.accepts(OZZ);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept 1000 (should be true)? " << unionNFA1.accepts(OZZZ);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept 00000 (should be false)? " << unionNFA1.accepts(ZZZZZ);
  std::cout << std::endl
            << std::endl;

  auto unionNFA2 = unionNFA(oneIsThirdFromEnd, containsOZOorOO);
  std::cout << "Does unionNFA(oneIsThirdFromEnd, containsOZOorOO) accept 100 (should be true)? " << unionNFA2.accepts(OZZ);
  std::cout << std::endl;
  std::cout << "Does unionNFA(oneIsThirdFromEnd, containsOZOorOO) accept emptyString (should be false)? " << unionNFA2.accepts(epsi);
  std::cout << std::endl;
  std::cout << "Does unionNFA(oneIsThirdFromEnd, containsOZOorOO) accept 1110 (should be true)? " << unionNFA2.accepts(OOOZ);
  std::cout << std::endl;
  std::cout << "Does unionNFA(oneIsThirdFromEnd, containsOZOorOO) accept 1011 (should be true)? " << unionNFA2.accepts(OZOO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(oneIsThirdFromEnd, containsOZOorOO) accept 10010 (should be false)? " << unionNFA2.accepts(OZZOZ);
  std::cout << std::endl;
  std::cout << std::endl << std::endl;


  auto unionNFA3 = unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO);
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept 111 (should be true)? " << unionNFA3.accepts(OOO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept 1001 (should be true)? " << unionNFA3.accepts(OZZO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept emptyString (should be true)? " << unionNFA3.accepts(epsi);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept 1110 (should be true)? " << unionNFA3.accepts(OOOZ);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept 1011 (should be true)? " << unionNFA3.accepts(OZOO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, containsOZOorOO) accept 000001 (should be false)? " << unionNFA3.accepts(ZZZZZO);
  std::cout << std::endl << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Concatenation Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  

  auto concatNFA1 = concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree);
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree) accept 10000 (should be true)? " << concatNFA1.accepts(OZZZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree) accept 100000 (should be false)? " << concatNFA1.accepts(OZZZZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree) accept 00000 (should be false)? " << concatNFA1.accepts(ZZZZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree) accept emptyString (should be false)? " << concatNFA1.accepts(epsi);
  std::cout << std::endl << std::endl;

  auto concatNFA2 = concatenationNFA(oneIsThirdFromEnd, containsOZOorOO);
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, containsOZOorOO) accept 100 (should be false)? " << concatNFA2.accepts(OZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, containsOZOorOO) accept 10000 (should be false)? " << concatNFA2.accepts(OZZZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, containsOZOorOO) accept 10011 (should be true)? " << concatNFA2.accepts(OZZOO);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, containsOZOorOO) accept 100101 (should be true)? " << concatNFA2.accepts(OZZOZO);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(oneIsThirdFromEnd, containsOZOorOO) accept emptyString (should be false)? " << concatNFA2.accepts(epsi);
  std::cout << std::endl << std::endl;
  
  auto concatNFA3 = concatenationNFA(containsOZOorOO, oneIsThirdFromEnd);
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 100 (should be false)? " << concatNFA3.accepts(OZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 10000 (should be false)? " << concatNFA3.accepts(OZZZZ);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 10011 (should be false)? " << concatNFA3.accepts(OZZOO);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 11111 (should be true)? " << concatNFA3.accepts(OOOOO);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 111 (should be false)? " << concatNFA3.accepts(OOO);
  std::cout << std::endl;
  std::cout << "Does concatenationNFA(containsOZOorOO, oneIsThirdFromEnd) accept 1010100 (should be true)? " << concatNFA3.accepts(OZOZOZZ);
  std::cout << std::endl << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Kleene Star Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  
  NFA<myChar> kleene1 = kleeneStarNFA(oneIsThirdFromEnd);
  std::cout << "Does (oneIsThirdFromEnd)* accept 100100 (should be true)? " << kleene1.accepts(OZZOZZ); // should return true
  std::cout << std::endl;
  std::cout << "Does (oneIsThirdFromEnd)* accept emptyString (should be true)? " << kleene1.accepts(epsi); // should return true
  std::cout << std::endl;
  std::cout << "Does (oneIsThirdFromEnd)* accept ABCD (should be false)? " << kleene1.accepts(ABCD); // should return false
  std::cout << std::endl;
  std::cout << "Does (oneIsThirdFromEnd)* accept 0000 (should be false)? " << kleene1.accepts(ZZZ); // should return false
  std::cout << std::endl;
  std::cout << "Does (oneIsThirdFromEnd)* accept 100 (should be true)? " << kleene1.accepts(OZZ); // should return true
  std::cout << std::endl;
  std::cout << "Does (oneIsThirdFromEnd)* accept 100100100 (should be true)? " << kleene1.accepts(OZZOZZOZZ); // should return true
  std::cout << std::endl;
  std::cout << std::endl;

  NFA<myChar> kleene2 = kleeneStarNFA(containsOZOorOO);
  std::cout << "Does (containsOZOorOO)* accept 10111 (should be true)? " << kleene2.accepts(OZOOO); // should return true
  std::cout << std::endl;
  std::cout << "Does (containsOZOorOO)* accept 11101 (should be true)? " << kleene2.accepts(OOOZO); // should return true
  std::cout << std::endl;
  std::cout << "Does (containsOZOorOO)* accept 101123 (should be false)? " << kleene2.accepts(OZOO23); // should return false
  std::cout << std::endl;
  std::cout << "Does (containsOZOorOO)* accept emptyString (should be true)? " << kleene2.accepts(epsi); // should return true
  std::cout << std::endl;
  std::cout << std::endl;

  NFA<myChar> kleene3 = kleeneStarNFA(numZerosIsMultipleOfTwoOrThree);
  std::cout << "Does (numZerosIsMultipleOfTwoOrThree)* accept Z (should be false)? " << kleene3.accepts(Z); // should return true
  std::cout << std::endl;
  std::cout << "Does (numZerosIsMultipleOfTwoOrThree)* accept ZZZZZ (should be true)? " << kleene3.accepts(ZZZZZ); // should return true
  std::cout << std::endl;
  std::cout << "Does (numZerosIsMultipleOfTwoOrThree)* accept ABCD (should be false)? " << kleene3.accepts(ABCD); // should return false
  std::cout << std::endl;
  std::cout << "Does (numZerosIsMultipleOfTwoOrThree)* accept emptyString (should be true)? " << kleene3.accepts(epsi); // should return true
  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA2DFA Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  DFA<myVector<myChar>> dfa1 = NFA2DFA(containsOZOorOO);
  std::cout << "Does dfa containsOZOorOO accept the empty string (should be false)? " << dfa1.accepts(epsi) << std::endl;
  std::cout << "Does dfa containsOZOorOO accept OZOO (should be true)? " << dfa1.accepts(OZOO) << std::endl;
  std::cout << "Does dfa containsOZOorOO accept ZZZ (should be false)? " << dfa1.accepts(ZZZ) << std::endl;
  std::cout << "Does dfa containsOZOorOO accept OOOZ (should be true)? " << dfa1.accepts(OOOZ) << std::endl;
  std::cout << "Does dfa containsOZOorOO accept OOZOO (should be true)? " << dfa1.accepts(OOZOO) << std::endl;
  std::cout << "Does dfa containsOZOorOO accept OO (should be true)? " << dfa1.accepts(OO) << std::endl;
  std::cout << std::endl;

  DFA<myVector<myChar>> dfa2 = NFA2DFA(oneIsThirdFromEnd);
  std::cout << "Does dfa oneIsThirdFromEnd accept OZZ (should be true)? " << dfa2.accepts(OZZ) << std::endl;
  std::cout << "Does dfa oneIsThirdFromEnd accept OOZ (should be true)? " << dfa2.accepts(OOZ) << std::endl;
  std::cout << "Does dfa oneIsThirdFromEnd accept OOOZ (should be true)? " << dfa2.accepts(OOOZ) << std::endl;
  std::cout << "Does dfa oneIsThirdFromEnd accept OZOO (should be false)? " << dfa2.accepts(OZOO) << std::endl;
  std::cout << "Does dfa oneIsThirdFromEnd accept the empty string (should be false)? " << dfa2.accepts(epsi) << std::endl;
  std::cout << std::endl;

  DFA<myVector<myChar>> dfa3 = NFA2DFA(numZerosIsMultipleOfTwoOrThree);
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept the empty string (should be true)? " << dfa3.accepts(epsi) << std::endl;
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept ZZ (should be true)? " << dfa3.accepts(ZZ) << std::endl;
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept ZZZ (should be true)? " << dfa3.accepts(ZZZ) << std::endl;
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept ZZZZZ (should be false)? " << dfa3.accepts(ZZZZZ) << std::endl;
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept OOZ (should be false)? " << dfa3.accepts(OOZ) << std::endl;
  std::cout << "Does dfa numZerosIsMultipleOfTwoOrThree accept OZOO (should be false)? " << dfa3.accepts(OZOO) << std::endl;
  std::cout << std::endl << std::endl;

  NFA<myChar> exampleNFA1("NFA txtbook example to convert to DFA", // name
                                 [](myChar a) -> bool {
                                   return (a.getVal() == '1' || a.getVal() == '2' || a.getVal() == '3');
                                 },                                           // states function
                                 myVector<myChar>{myChar('a'), myChar('b')},  // alphabet
                                 myChar('1'),                                 // start state
                                 [](myChar a, myChar b) -> myVector<myChar> { // transition function
                                   if (a.getVal() == '1' && b.getVal() == 'b')
                                     return myVector<myChar>{myChar('2')};
                                   else if (a.getVal() == '2' && b.getVal() == 'a')
                                     return myVector<myChar>{myChar('2'), myChar('3')};
                                   else if (a.getVal() == '2' && b.getVal() == 'b')
                                     return myVector<myChar>{myChar('3')};
                                   else if (a.getVal() == '3' && b.getVal() == 'a')
                                     return myVector<myChar>{myChar('1')};
                                   else
                                     return myVector<myChar>{};
                                 },
                                 [](myChar a) -> myVector<myChar> { // epsilon transition
                                   if (a.getVal() == '1')
                                     return myVector<myChar>{myChar('3')};
                                   else
                                     return myVector<myChar>{};
                                 },
                                 [](myChar a) -> bool { // accept states
                                   return (a.getVal() == '1');
                                 });

  // textbookExampleNFA converted manually to a DFA (by following the textbook's example)
  DFA<myVector<myChar>> manuallyConvertedExampleNFA1(
      "textbookExampleNFA -> DFA",
      [](myVector<myChar> &a) -> bool {
        if (a == myVector<myChar>{})
          return true;
        else if (a == myVector<myChar>{myChar('1'), myChar('3')})
          return true;
        else if (a == myVector<myChar>{myChar('2')})
          return true;
        else if (a == myVector<myChar>{myChar('3')})
          return true;
        else if (a == myVector<myChar>{myChar('2'), myChar('3')})
          return true;
        else if (a == myVector<myChar>{myChar('1'), myChar('2'), myChar('3')})
          return true;
        else
          return false;
      },
      myVector<myChar>{myChar('a'), myChar('b')}, // alphabet
      myVector<myChar>{myChar('1'), myChar('3')}, // start state
      [](myVector<myChar> a, myChar b) -> myVector<myChar> {
        if (a == myVector<myChar>{} && (b.getVal() == 'a' || b.getVal() == 'b'))
          return a;
        else if (a == myVector<myChar>{myChar('2')} && b.getVal() == 'a')
          return myVector<myChar>{myChar('2'), myChar('3')};
        else if (a == myVector<myChar>{myChar('2')} && b.getVal() == 'b')
          return myVector<myChar>{myChar('3')};
        else if (a == myVector<myChar>{myChar('3')} && b.getVal() == 'b')
          return myVector<myChar>{};
        else if (a == myVector<myChar>{myChar('3')} && b.getVal() == 'a')
          return myVector<myChar>{myChar('1'), myChar('3')};
        else if (a == myVector<myChar>{myChar('2'), myChar('3')} && b.getVal() == 'b')
          return myVector<myChar>{myChar('3')};
        else if (a == myVector<myChar>{myChar('2'), myChar('3')} && b.getVal() == 'a')
          return myVector<myChar>{myChar('1'), myChar('2'), myChar('3')};
        else if (a == myVector<myChar>{myChar('1'), myChar('2'), myChar('3')} && b.getVal() == 'a')
          return a;
        else if (a == myVector<myChar>{myChar('1'), myChar('2'), myChar('3')} && b.getVal() == 'b')
          return myVector<myChar>{myChar('2'), myChar('3')};
        else if (a == myVector<myChar>{myChar('1'), myChar('3')} && b.getVal() == 'a')
          return a;
        else if (a == myVector<myChar>{myChar('1'), myChar('3')} && b.getVal() == 'b')
          return myVector<myChar>{myChar('2')};
        else
          return a;
      },
      [](myVector<myChar> &a) -> bool {
        return (a == myVector<myChar>{myChar('1'), myChar('3')} || a == myVector<myChar>{myChar('1'), myChar('2'), myChar('3')});
      });
  // textbookExampleNFA automatically converted to DFA using NFA2DFA function
  DFA<myVector<myChar>> autoConvertedExampleNFA1 = NFA2DFA(exampleNFA1);
  NFA<myChar> exampleNFA2("NFA example from Youtube video to convert to DFA", // name
                                 [](myChar a) -> bool {
                                   return (a.getVal() == 'A' || a.getVal() == 'B');
                                 },                                           // states function
                                 myVector<myChar>{myChar('0'), myChar('1')},  // alphabet
                                 myChar('A'),                                 // start state
                                 [](myChar a, myChar b) -> myVector<myChar> { // transition function
                                  if(a.getVal() == 'A' && b.getVal() == '1')
                                    return myVector<myChar> {myChar('A'), myChar('B')};
                                  else if (a.getVal() == 'A' && b.getVal() == '0')
                                    return myVector<myChar> {myChar('A')};
                                  else
                                     return myVector<myChar>{};
                                 },
                                 [](myChar a) -> myVector<myChar> { // epsilon transition
                                  return myVector<myChar>{};
                                 },
                                 [](myChar a) -> bool { // accept states
                                   return (a.getVal() == 'B');
                                 });
  DFA<myVector<myChar>> manuallyConvertedExampleNFA2("NFA example from Youtube video manually converted to DFA", // name
                                 [](myVector<myChar> &a) -> bool {
                                  if (a == myVector<myChar>{myChar('A')})
                                    return true;
                                  else if (a == myVector<myChar>{myChar('A'), myChar('B')})
                                    return true;
                                  else
                                    return false;
                                 },                                           // states function
                                 myVector<myChar>{myChar('0'), myChar('1')},  // alphabet
                                 myVector<myChar>{myChar('A')},                                 // start state
                                 [](myVector<myChar> a, myChar b) -> myVector<myChar> { // transition function
                                  if(a == myVector<myChar>{myChar('A')} && b.getVal() == '0')
                                    return myVector<myChar>{myChar('A')};
                                  else if (a == myVector<myChar>{myChar('A')} && b.getVal() == '1')
                                    return myVector<myChar> {myChar('A'), myChar('B')};
                                  else if (a == myVector<myChar>{myChar('A'), myChar('B')} && b.getVal() == '1')
                                    return myVector<myChar>{myChar('A'), myChar('B')};
                                  else if (a == myVector<myChar>{myChar('A'), myChar('B')} && b.getVal() == '0')
                                    return myVector<myChar>{myChar('A')};
                                  else
                                     return myVector<myChar>{};
                                 },                           
                                 [](myVector<myChar> &a) -> bool { // accept states
                                   return (a == myVector<myChar>{myChar('A'), myChar('B')});
                                 });
  DFA<myVector<myChar>> autoConvertedExampleNFA2 = NFA2DFA(exampleNFA2);
  
  std::cout << "Does dfa1 == dfa2 (should be false)? " << equalityDFA(dfa1, dfa2) << std::endl;
  std::cout << "Does dfa2 == dfa3 (should be false)? " << equalityDFA(dfa2, dfa3) << std::endl;
  std::cout << "Does dfa2 == dfa2 (should be true)? " << equalityDFA(dfa2, dfa2) << std::endl;
  std::cout << "Is manually-converted example NFA1 == auto-converted example NFA1 (should be true)? " << equalityDFA(autoConvertedExampleNFA1, manuallyConvertedExampleNFA1);
  std::cout << std::endl;
  std::cout << "Is manually-converted example NFA2 == auto-converted example NFA2 (should be true)? " << equalityDFA(autoConvertedExampleNFA2, manuallyConvertedExampleNFA2);
  std::cout << std::endl;
  std::cout << "Is auto-converted example NFA1 == auto-converted example NFA2 (should be false)? " << equalityDFA(autoConvertedExampleNFA1, autoConvertedExampleNFA2);
  std::cout << std::endl;
}


void makeAndTestRegex()
{
  unionRegex r1 = unionRegex(new concatRegex(new charRegex(myChar('0')), new charRegex(myChar('1'))),
                             new concatRegex(new charRegex(myChar('1')), new charRegex(myChar('0'))));
  regexPrinter(r1);                                                                        // should print (01 U 10)
  oneString ZO = oneString('0', new oneString('1', new emptyString));                      // accepted; 01
  oneString OZ = oneString('1', new oneString('0', new emptyString));                      // accepted; 10
  oneString O = oneString('1', new emptyString);                                           // rejected; 1
  oneString ZOZ = oneString('0', new oneString('1', new oneString('0', new emptyString))); // rejected; 010

  concatRegex r2 = concatRegex(new kleeneRegex(new charRegex(myChar('0'))),
                               new concatRegex(new charRegex(myChar('1')), new kleeneRegex(new charRegex(myChar('0')))));
  regexPrinter(r2); // should prnit (0)*1(0)*
  // rejects ZO and OZ
  // accepts ZOZ

  oneString ZZOZ = oneString('0', new oneString('0', new oneString('1', new oneString('0', new emptyString)))); // accepted; 0010

  concatRegex(r3) = concatRegex(new unionRegex(new charRegex(myChar('0')), new epsiRegex()),
                                new kleeneRegex(new charRegex(myChar('1'))));
  regexPrinter(r3); // should print (0 U _)(1)*, which is equal to 01* U 1*
  // accepts ZO and 1
  // rejects ZOZ and OZ

  oneString ZOO = oneString('0', new oneString('0', new oneString('1', new oneString('1', new emptyString)))); // accepted; 011

  concatRegex r4 = concatRegex(new kleeneRegex(new charRegex(myChar('a'))), new concatRegex(new charRegex(myChar('0')),
                                                                                            new concatRegex(new charRegex(myChar('0')), new concatRegex(new charRegex(myChar('1')), new kleeneRegex(new charRegex(myChar('b')))))));
  regexPrinter(r4); // should print (a)*001(b)*
  // rejects ZOZ, O
  oneString aZZOb = oneString('a', new oneString('0', new oneString('0', new oneString('1', new oneString('b', new emptyString)))));                                           // accepted; a001b
  oneString aZZ = oneString('a', new oneString('0', new oneString('0', new emptyString)));                                                                                     // rejected; a00
  oneString aaZZObb = oneString('a', new oneString('a', new oneString('0', new oneString('0', new oneString('1', new oneString('b', new oneString('b', new emptyString))))))); // accepted; aa001bb
  oneString aZZObb = oneString('a', new oneString('0', new oneString('0', new oneString('1', new oneString('b', new oneString('b', new emptyString))))));                      // accepted; a001bb

  kleeneRegex r5 = kleeneRegex(new concatRegex(new charRegex(myChar('a')),
                                               new concatRegex(new charRegex(myChar('b')), new charRegex(myChar('c')))));
  regexPrinter(r5); // should print (abc)*
  // rejects ZOZ, O, etc.
  oneString abc = oneString('a', new oneString('b', new oneString('c', new emptyString)));                                                                // accepted; abc
  oneString abcabc = oneString('a', new oneString('b', new oneString('c', new oneString('a', new oneString('b', new oneString('c', new emptyString)))))); // accepted; abcabc

  unionRegex r6 = unionRegex(new kleeneRegex(new concatRegex(new charRegex(myChar('a')),
                                                             new concatRegex(new charRegex(myChar('b')), new charRegex(myChar('c'))))),
                             new unionRegex(new concatRegex(new charRegex(myChar('0')), new charRegex(myChar('1'))),
                                            new concatRegex(new charRegex(myChar('1')), new charRegex(myChar('0')))));
  regexPrinter(r6); // should print ((abc)* U (01 U 10))
  // accepts abc, abcabc, 01, 10
  // rejects ZOZ, O, etc.
  oneString abcZO = oneString('a', new oneString('b', new oneString('c', new oneString('0', new oneString('1', new emptyString))))); // rejected; abc01

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                     REGEX GENERATOR Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  myString *temp = r1.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  temp = r2.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  temp = r3.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  temp = r4.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  temp = r5.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  temp = r6.generator();
  while (temp->isEmpty() == false)
  {
    std::cout << temp->charValue();
    temp = temp->next();
  }
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                     REGEX -> NFA Tests                        " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;
  regex *r5temp = &r5;
  // NFA<myChar> r2n1 = regex2NFA<NFAComboState<myChar, NFAComboState<myChar, myChar>>>(r5temp);
}

void showMenu()
{
  std::cout << "1: Make and Test DFAs" << std::endl;
  std::cout << "2: Make and Test NFAs" << std::endl;
  std::cout << "3: Make and test Regex" << std::endl;
  std::cout << "4: Exit" << std::endl;
}

int main()
{
  int choice;
  char repeat;
  do
  {
    showMenu();
    std::cin >> choice;

    switch (choice)
    {
    case 1:
      makeAndTestDFAs();
      break;
    case 2:
      makeAndTestNFAs();
      break;
    case 3:
      makeAndTestRegex();
      break;
    case 4:
      return 0;
    default:
      std::cout << "That is not a valid menu choice" << std::endl;
      system("cls"); //clear screen
      showMenu();    //show menu again
    }
    std::cout << "Would you like to keep going (type Y or N): ";
    std::cin >> repeat;
  } while (repeat == 'Y');

  return 0;
}
