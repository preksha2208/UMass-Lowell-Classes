// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include "mainHeaders.hpp"

// generate myString linked list from given std::string
oneString genMyString(std::string &str)
{
  oneString finalStr = oneString(str[0], NULL);
  myString* temp = &finalStr;

  for(int i = 1; i < str.length(); i++)
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
  std::sort(a.begin(), a.end(), [](myChar &a, myChar &b) {
    return (a.getVal() < b.getVal());
  });

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
  myVector<State> startStates{nfa.epsilonTrans(nfa.q0)};
  startStates.insert(startStates.begin(), nfa.q0);

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
      startStates,
      [=](myVector<State> a, myChar b) -> myVector<State> {
        myVector<State> tempVector;
        myVector<State> epsilonStates;
        myVector<State> newStates;

        for (State x : a)
        {
          tempVector = nfa.epsilonTrans(x); // check whether there are epsilon transitions from current state
          epsilonStates.insert(epsilonStates.end(), tempVector.begin(), tempVector.end());
        }
        a.insert(startStates.end(), epsilonStates.begin(), epsilonStates.end());

        for (State x : a)
        {
          tempVector = transFunc(x, b); // generate new sets of states from input char w/ each current state
          newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        }

        return newStates; // return new state generated from the current state
      },
      [=](myVector<State> &a) -> bool {
        for (State x : a)
        {
          if (!nfa.F(x)) // make states in vector are all accepted by the original nfa
            return false;
        }
        return true;
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

  return NFA<nfaState>(
      "Union of " + nfa1.name + " and " + nfa2.name,
      [=](nfaState a) -> bool { // Q function
        if (a.isStartState)
          return true;
        else if (a.isAcceptState)
          return true;
        else if (a.isFromX)
          return nfa1.Q(a.fromX);
        else
          return nfa2.Q(a.fromY);
      },
      a,           // combined alphabets
      nfaState(0), // start state object
      [=](nfaState a, myChar b) -> myVector<nfaState> {
        if (a.isStartState)
          return myVector<nfaState>{}; // start state has no transitions other than epsi transitions
        else if (a.isAcceptState)
          return myVector<nfaState>{}; // accept state has no transitions other than epsi transitions
        else if (a.isFromX)
        {
          myVector<State1> xVec = nfa1.transFunc(a.fromX, b);  // get State1 objects from nfa1's transition function
          myVector<nfaState> xStateVec;
          for (State1 x : xVec) // create vector of nfaState objects out of State1 objects
            xStateVec.push_back(nfaState(x, 1));
          return xStateVec;
        }
        else
        {
          myVector<State2> yVec = nfa2.transFunc(a.fromY, b);  // get State2 objects from nfa2's transition function
          myVector<nfaState> yStateVec;
          for (State2 y : yVec) // create vector of nfaState objects out of State2 objects
            yStateVec.push_back(nfaState(1, y));
          return yStateVec;
        }
      },
      [=](nfaState a) -> myVector<nfaState> { // epsilon transition function
        if (a.isStartState)
          return myVector<nfaState>{nfaState(nfa1.q0, 1), nfaState(1, nfa2.q0)};
        else if (a.isAcceptState)
          return myVector<nfaState>{}; // no epsi transitinos from accept state
        else if (a.isFromX)
        {
          myVector<State1> xVec = nfa1.epsilonTrans(a.fromX);  // get State1 objects from nfa1's epsiTrans function
          myVector<nfaState> xStateVec;
          for (State1 x : xVec)
            xStateVec.push_back(nfaState(x, 1));
          if (nfa1.F(a.fromX))
            xStateVec.push_back(nfaState(1)); //nfa1's accept state epsi transitions to union NFA's accept state
          return xStateVec;
        }
        else
        {
          myVector<State2> yVec = nfa2.epsilonTrans(a.fromY);   // get State2 objects from nfa2's epsiTrans function
          myVector<nfaState> yStateVec;
          for (State2 y : yVec)
            yStateVec.push_back(nfaState(1, y));
          if (nfa2.F(a.fromY))
            yStateVec.push_back(nfaState(1)); // nfa2's accept state epsi transitions to union NFA's accept state
          return yStateVec;
        }
      },
      [=](nfaState a) -> bool {
        return a.isAcceptState;
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

  return NFA<nfaState>(
      "Concatenation of " + nfa1.name + " and " + nfa2.name,
      [=](nfaState a) -> bool { // Q function
        if (a.isStartState)
          return true;
        else if (a.isAcceptState)
          return true;
        else if (a.isFromX)
          return nfa1.Q(a.fromX);
        else
          return nfa2.Q(a.fromY);
      },
      a,                    // combined alphabets
      nfaState(nfa1.q0, 1), // start state is first nfa's start state
      [=](nfaState a, myChar b) -> myVector<nfaState> {
        if (a.isFromX) // the state is within nfa1
        {
          myVector<State1> xVec = nfa1.transFunc(a.fromX, b);
          myVector<nfaState> xStateVec;
          for (State1 x : xVec) // create vector of nfaState objects out of State1 objects
            xStateVec.push_back(nfaState(x, 1));
          return xStateVec;
        }
        else // the state is within nfa2
        {
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
          for (State1 x : xVec)
            xStateVec.push_back(nfaState(x, 1));
          if (nfa1.F(a.fromX))
            xStateVec.push_back(nfaState(1, nfa2.q0)); // nfa1's accept state epsilon transitions to nfa2's start state
          return xStateVec;
        }
        else
        {
          myVector<State2> yVec = nfa2.epsilonTrans(a.fromY);
          myVector<nfaState> yStateVec;
          for (State2 y : yVec)
            yStateVec.push_back(nfaState(1, y));
          return yStateVec;
        }
      },
      [=](nfaState a) -> bool {
        if (a.isFromY)
          return nfa2.F(a.fromY);
        else
          return false;
      }

  );
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
      nfa.transFunc,
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
                                    return myVector<myChar>{myChar('E')}; // may need to change this
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
                                             myVector<myChar>{myChar('0')},               // alphabet
                                             myChar('A'),                                 // start state
                                             [](myChar a, myChar b) -> myVector<myChar> { // transition function
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
                                               else
                                                 return myVector<myChar>{myChar('G')}; // may need to change this
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
                                  return myVector<myChar>{myChar('E')}; // may need to change this
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

  // inputStrings
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

  //example traces
  oneString A = oneString('A', new emptyString);
  oneString AA = oneString('A', new oneString('A', new emptyString));
  oneString ABCD = oneString('A', new oneString('B', new oneString('C', new oneString('D', new emptyString))));
  oneString ABCE = oneString('A', new oneString('B', new oneString('C', new oneString('E', new emptyString))));
  oneString HI = oneString('H', new oneString('I', new emptyString));
  oneString ABCB = oneString('A', new oneString('B', new oneString('C', new oneString('B', new emptyString))));
  oneString ABC = oneString('A', new oneString('B', new oneString('C', new emptyString)));
  oneString ADEFD = oneString('A', new oneString('D', new oneString('E', new oneString('F', new oneString('D', new emptyString)))));
  oneString AD = oneString('A', new oneString('D', new emptyString));
  oneString AB = oneString('A', new oneString('B', new emptyString));
  oneString B = oneString('B', new emptyString);
  oneString D = oneString('D', new emptyString);
  oneString E = oneString('E', new emptyString);

  // trace tree for numZerosIsMultipleOfTwoOrThree with emptyString
  oneString ABD = oneString('A', new oneString('B', new oneString('D', new emptyString)));
  // trace tree for numZerosIsMultipleOfTwoOrThree with ZZ
  oneString A_ABD_GCE = oneString('A', new oneString('-', new oneString('B', new oneString('D',
                                                                                           new oneString('-', new oneString('G', new oneString('C', new oneString('E', new emptyString))))))));
  // trace tree for containsOZOorOO with OZZ
  oneString A_ABC_ACE = oneString('A', new oneString('-', new oneString('A', new oneString('B', new oneString('C',
                                                                                                              new oneString('-', new oneString('A', new oneString('C', new oneString('E', new emptyString)))))))));
  // trace tree for oneIsThirdFromEnd with

  // tests for oneIsThirdFromEnd
  std::cout << std::boolalpha;
  std::cout << "Does oneIsThirdFromEnd accept OZZ? " << oneIsThirdFromEnd.accepts(OZZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OOZ? " << oneIsThirdFromEnd.accepts(OOZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OOOZ? " << oneIsThirdFromEnd.accepts(OOOZ) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept OZOO? " << oneIsThirdFromEnd.accepts(OZOO) << std::endl;
  std::cout << "Does oneIsThirdFromEnd accept the empty string? " << oneIsThirdFromEnd.accepts(epsi) << std::endl;
  std::cout << std::endl;

  // tests for numZerosIsMultipleOfTwoOrThree
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the empty string? " << numZerosIsMultipleOfTwoOrThree.accepts(epsi) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept ZZZZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept OOZ? " << numZerosIsMultipleOfTwoOrThree.accepts(OOZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept OZOO? " << numZerosIsMultipleOfTwoOrThree.accepts(OZOO) << std::endl;

  // tests for containsOZOorOO
  std::cout << "Does containsOZOorOO accept the empty string? " << containsOZOorOO.accepts(epsi) << std::endl;
  std::cout << "Does containsOZOorOO accept OZOO? " << containsOZOorOO.accepts(OZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept ZZZ? " << containsOZOorOO.accepts(ZZZ) << std::endl;
  std::cout << "Does containsOZOorOO accept OOOZ? " << containsOZOorOO.accepts(OOOZ) << std::endl;
  std::cout << "Does containsOZOorOO accept OOZOO? " << containsOZOorOO.accepts(OOZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept OO? " << containsOZOorOO.accepts(OO) << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    Oracle Function Tests                      " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  // oracle tests for oneIsThirdFromEnd
  std::cout << "Is 'A' a valid trace of oneIsThirdFromEnd with emptyString? " << oneIsThirdFromEnd.oracle(epsi, A) << std::endl;
  std::cout << "Is 'ABCD' a valid trace of oneIsThirdFromEnd with OZZ? " << oneIsThirdFromEnd.oracle(OZZ, ABCD) << std::endl;
  std::cout << "Is 'ABC' a valid trace of oneIsThirdFromEnd with OZZ? " << oneIsThirdFromEnd.oracle(OZZ, ABC) << std::endl;
  std::cout << "Is 'A' a valid trace of oneIsThirdFromEnd with OZZ? " << oneIsThirdFromEnd.oracle(OZZ, A) << std::endl;

  // oracle tests for numZerosIsMultipleOfTwoOrThree
  std::cout << "Is 'A' a valid trace of numZerosIsMultipleOfTwoOrThree with emptyString? " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, A) << std::endl;
  std::cout << "Is 'B' a valid trace of numZerosIsMultipleOfTwoOrThree with emptyString? " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, B) << std::endl;
  std::cout << "Is 'D' a valid trace of numZerosIsMultipleOfTwoOrThree with emptyString? " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, D) << std::endl;
  std::cout << "Is 'E' a valid trace of numZerosIsMultipleOfTwoOrThree with emptyString? " << numZerosIsMultipleOfTwoOrThree.oracle(epsi, E) << std::endl;
  std::cout << "Is 'ABCB' a valid trace of numZerosIsMultipleOfTwoOrThree with ZZ? " << numZerosIsMultipleOfTwoOrThree.oracle(ZZ, ABCB) << std::endl;
  std::cout << "Is 'ADEFD' a valid trace of numZerosIsMultipleOfTwoOrThree with ZZZ? " << numZerosIsMultipleOfTwoOrThree.oracle(OZZ, ADEFD) << std::endl;
  std::cout << "Is 'A' a valid trace of numZerosIsMultipleOfTwoOrThree with OZZ? " << numZerosIsMultipleOfTwoOrThree.oracle(OZZ, A) << std::endl;

  // oracle tests for containsOZOorOO
  std::cout << "Is 'A' a valid trace of containsOZOorOO with emptyString? " << containsOZOorOO.oracle(epsi, A) << std::endl;
  std::cout << "Is 'ABCD' a valid trace of containsOZOorOO with OO? " << containsOZOorOO.oracle(OO, ABCD) << std::endl;
  std::cout << "Is 'ABCE' a valid trace of containsOZOorOO with OZZ? " << containsOZOorOO.oracle(OZZ, ABCE) << std::endl;
  std::cout << "Is 'A' a valid trace of containsOZOorOO with OZZ? " << containsOZOorOO.oracle(OZZ, A) << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    Trace Tree Function Tests                      " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with ZZ: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(ZZ);
  std::cout << "Trace tree of numZerosIsMultipleOfTwoOrThree with emptyString: ";
  numZerosIsMultipleOfTwoOrThree.traceTree(epsi);
  std::cout << "Trace tree of containsOZOorOO with OO: ";
  containsOZOorOO.traceTree(OO);
  std::cout << "Trace tree of containsOZOorOO with OZZ: ";
  containsOZOorOO.traceTree(OZZ);
  std::cout << "Trace tree of oneIsThirdFromEnd with OZZ: ";
  oneIsThirdFromEnd.traceTree(OZZ);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Union Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  auto numZerosIsMultipleOfTwoOrThreeOrOneIsThirdFromEnd = unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd);
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept the emptyString? " << numZerosIsMultipleOfTwoOrThreeOrOneIsThirdFromEnd.accepts(epsi);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept OO? " << numZerosIsMultipleOfTwoOrThreeOrOneIsThirdFromEnd.accepts(OO);
  std::cout << std::endl;
  std::cout << "Does unionNFA(numZerosIsMultipleOfTwoOrThree, oneIsThirdFromEnd) accept OZ? " << numZerosIsMultipleOfTwoOrThreeOrOneIsThirdFromEnd.accepts(OZ);
  std::cout << std::endl;

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Concatenation Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;

  NFA<NFAComboState<myChar, myChar>> concatOneThirdFromEndAndNumZerosMultTwoOrThree = concatenationNFA(oneIsThirdFromEnd, numZerosIsMultipleOfTwoOrThree);
  std::string s1 = "10000";
  oneString OZZZZ = genMyString(s1);
  std::cout << "Does concatOneThirdFromEndAndNumZerosMultTwoOrThree accept 10000? " << concatOneThirdFromEndAndNumZerosMultTwoOrThree.accepts(OZZZZ);
  std::cout << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                    NFA Kleene Star Tests                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl;
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
