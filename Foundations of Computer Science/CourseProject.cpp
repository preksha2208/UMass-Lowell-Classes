// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <functional> // for std::function
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
#include <utility> // for std::pair
#include <stack>
#include <type_traits>
#include <algorithm>
#include "DFA.hpp"
#include "myPair.hpp"
#include "NFA.hpp"

// creates a DFA that only excepts a string of length one of just inputChar
DFA<myChar> oneCharDFA(myChar inputChar)
{
  return DFA<myChar>(
      "onlyAccepts" + std::string(1, inputChar.getVal()),
      [=](myChar a) -> bool { return (a == myChar('A') || a == myChar('B') || a == myChar('C')); },
      std::vector<myChar>{inputChar}, myChar('A'),
      [&](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && (b.getVal() == inputChar.getVal()))
          return myChar('B');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });
}

// returns a DFA that is the complement of the inputDFA
template <class State>
DFA<State> complementDFA(DFA<State> inputDFA)
{
  return DFA<State>("complement of " + inputDFA.name, inputDFA.Q, inputDFA.alphabet, inputDFA.q0,
                    inputDFA.transFunc, [=](State a) -> bool {
                      return (!inputDFA.F(a));
                    });
}

// creates a DFA that is the union of dfa1 and dfa2
template <class State1, class State2>
DFA<myPair<State1, State2>> unionDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  std::vector<myChar> a = dfa1.alphabet;
  std::vector<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs
  std::sort(a.begin(), a.end(), [](myChar &a, myChar &b) {
    return (a.getVal() < b.getVal());
  });

  return DFA<myPair<State1, State2>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                                                  // alphabet
      myPair<State1, State2>(dfa1.q0, dfa2.q0),                           // start state, need to figure this one out
      [=](myPair<State1, State2> a, myChar b) -> myPair<State1, State2> { // transition function; not correct as is
        return (myPair<State1, State2>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State1, State2> a) -> bool { // accept states
        return ((dfa1.F(a.first)) || (dfa2.F(a.second)));
      });
}

// creates a DFA that is the intersection of dfa1 and dfa2
template <class State1, class State2>
DFA<myPair<State1, State2>> intersectionDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  std::vector<myChar> a = dfa1.alphabet;
  std::vector<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs

  return DFA<myPair<State1, State2>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                        // alphabet
      myPair<State1, State2>(dfa1.q0, dfa2.q0), // start state, need to figure this one out
      [=](myPair<State1, State2> a, myChar b) -> myPair<State1, State2> {
        return (myPair<State1, State2>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State1, State2> a) -> bool {             // accept states
        return ((dfa1.F(a.first)) && (dfa2.F(a.second))); // only difference from unionDFA function
      });
}

// returns true/false to indicate whether dfa1 is a subset of dfa2
template <class State1, class State2>
bool subsetDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  DFA<myPair<State1, State2>> dfa3 = intersectionDFA(dfa1, complementDFA(dfa2));
  return (!(dfa3.acceptedString().first)); // if dfa3 accepts nothing, then dfa is subset of dfa2
}

template <class State1, class State2>
bool equalityDFA(DFA<State1> dfa1, DFA<State2> dfa2)
{
  DFA<myPair<myPair<State1, State2>, myPair<State1, State2>>> dfa3 = unionDFA(intersectionDFA(dfa1, complementDFA(dfa2)), intersectionDFA(complementDFA(dfa1), dfa2));
  return (!dfa3.acceptedString().first); // if dfa3 accepts nothing, then dfa1 = dfa2
}

/*
// creates a DFA that is the union of dfa1 and dfa2
template <class State1, class State2>
NFA<myPair<State1, State2>> unionNFA(NFA<State1> nfa1, NFA<State2> nfa2)
{
  std::vector<myChar> a = nfa1.alphabet;
  std::vector<myChar> b = nfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both NFAs
  std::sort(a.begin(), a.end(), [](myChar &a, myChar &b) {
    return (a.getVal() < b.getVal());
  });

  return NFA<myPair<State1, State2>>(
      "Union of " + nfa1.name + " and " + nfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (nfa1.Q(a.first) && nfa2.Q(a.second));
      },
      a,                                                                  // alphabet
      myPair<State1, State2>(nfa1.q0, nfa2.q0),                           // start state
      [=](myPair<State1, State2> a, myChar b) -> std::vector<myPair<State1, State2>> { // transition function
        return  std::vector<myPair<State1, State>> // need
        //myPair<State1, State2>(nfa1.transFunc(a.first, b), nfa2.transFunc(a.second, b)));
      },
      [=](myPair<State1, State2> a) -> std::vector<myPair<State1, State2>> {
        return myPair<State1, State2>(nfa1.epsilonTrans(a.first), nfa2.epsilonTrans(a.second));
      },
      [=](myPair<State1, State2> a) -> bool { // accept states
        return ((nfa1.F(a.first)) || (nfa2.F(a.second)));
      });
}

template <class State1, class State2>
NFA<myPair<State1, State2>> concatenationNFA(NFA<State1> nfa1, NFA<State2> nfa2)
{
  std::vector<myChar> a = nfa1.alphabet;
  std::vector<myChar> b = nfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both NFAs
  std::sort(a.begin(), a.end(), [](myChar &a, myChar &b) {
    return (a.getVal() < b.getVal());
  });

  return NFA<myPair<State1, State2>>(
      "Union of " + nfa1.name + " and " + nfa2.name,
      [=](myPair<State1, State2> a) -> bool { // function for possible states
        return (nfa1.Q(a.first) && nfa2.Q(a.second));
      },
      a,                                                                               // alphabet
      myPair<State1, State2>(nfa1.q0, nfa2.q0),                                        // start state
      [=](myPair<State1, State2> a, myChar b) -> std::vector<myPair<State1, State2>> { // transition function
        // check if nfa1 is in accept state
        if (nfa1.F(a.first)) 
        // if yes, then do transFunc only on nfa2
          return  myPair<State1, State2>>{a.first, nfa2.transFunc(a.second, b)}; 
        else
          return myPair<State1, State2>(nfa1.transFunc(a.first, b), a.second);  // otherwise, do transFunc only on nfa1
      },
      [=](myPair<State1, State2> a) -> std::vector<myPair<State1, State2>> { // epsilon transitions
        if (nfa1.F(a.first))  // check if first nfa has reached its accept state
          return  myPair<State1, State2>(a.first, nfa2.epsilonTrans(a.second));  // if yes, then do epsilonTrans only on second NFA
        else
          return myPair<State1, State2>(nfa1.epsilonTrans(a.first), a.second); // otherwise, do epsilonFunc only on nfa1
      },
      [=](myPair<State1, State2> a) -> bool { // accept states
        return ((nfa1.F(a.first)) && (nfa2.F(a.second))); 
      });
}

NFA<myChar> kleeneStarNFA(NFA<myChar> nfa)
{
  return NFA<myChar>(
      nfa.name + " Kleene Star",
      [=](myChar a) -> bool { 
        if(a == myChar('|'))
          return true;
        else
         return nfa.Q(a);
      },
      nfa.alphabet, myChar('|'),  // start state may have to be changed to something else
      nfa.transFunc,
      [=](myChar a) -> std::vector<myChar> {
        if (a == myChar('|'))   // 
          return std::vector<myChar>{nfa.q0};
        else if (nfa.F(a))
          return std::vector<myChar>{nfa.q0};
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
*/

// generate nth string of alphabet's lexicographical ordering
myString *lexi(int n, std::list<myChar> alphabet)
{
  int size = alphabet.size();
  if (size == 0)
    return new emptyString;

  int i = alphabet.size();
  while (i < n)
  {
    i = i * i;
  }
  std::cout << "i: " << i;

  myString *l = new emptyString;

  return l;
}

// takes in dfa, vector of test strings and expected values for the test strings on the given dfa
// bool values are at same index in bool vector as their corresponding test string in the other vector
template <class State>
void DFAtester(DFA<State> &a, std::vector<myString *> &testStrings, std::vector<bool> &expected)
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

// creates DFAs and runs tests on them
void makeAndTestDFAs()
{
  // Declarations of DFAs
  DFA<myChar> evenLengthBinary( // returns whether length of inputted binary
                                // number is even
      "EvenLengthBinary",       // name
      [](myChar a) -> bool {    // state function
        return ((a.getVal() == 'A') || (a.getVal() == 'B') || (a.getVal() == 'C'));
      },
      std::vector<myChar>{myChar('0'), myChar('1')}, // alphabet
      myChar('A'),                                   // start state
      [](myChar a, myChar b) -> myChar {             // transition function
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
      std::vector<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar { return a; },
      [](myChar a) -> bool { return false; });

  DFA<myChar> onlyAcceptsEmptyString( // only accepted input is the empty string
      "OnlyAcceptsEmptyString",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::vector<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        return myChar('B');
      },
      [](myChar a) -> bool { return (a == myChar('A')); });
  DFA<myChar> evenBinaryNumber( // returns whether the inputted binary number is even
      "EvenBinaryNumber",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('C'), myChar('A'), myChar('M')}, myChar('A'),
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
      std::vector<myChar>{myChar('/')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
      std::vector<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
  std::vector<myString *> evenLengthStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedEvenLength{true, true, true, true, true, true, false, false, false, false, false, false};
  DFAtester(evenLengthBinary, evenLengthStrings, expectedEvenLength);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing AcceptsNothing DFA" << std::endl; // tests for AcceptsNothing DFA
  std::vector<myString *> acceptsNothingStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedAcceptsNothing{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(acceptsNothing, acceptsNothingStrings, expectedAcceptsNothing);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OnlyAcceptsEmptyString DFA" << std::endl; // tests for onlyAcceptsEmptyString DFA
  std::vector<myString *> onlyAcceptsEmptyStringStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedOnlyAcceptsEmptyString{false, false, false, false, false, true, false, false, false, false, false, false};
  DFAtester(onlyAcceptsEmptyString, onlyAcceptsEmptyStringStrings, expectedOnlyAcceptsEmptyString);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenBinaryNumber DFA"
            << std::endl; // tests for evenBinaryNumber DFA
  std::vector<myString *> evenBinaryNumberStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedEvenBinaryNumber{true, false, false, true, false, false, false, true, true, true, false, true};
  DFAtester(evenBinaryNumber, evenBinaryNumberStrings, expectedEvenBinaryNumber);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddBinaryNumber DFA" << std::endl; // tests for OddBinaryNumber DF
  std::vector<myString *> oddBinaryNumberStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedOddBinaryNumber{false, true, true, false, true, false, true, false, false, false, true, false};
  DFAtester(oddBinaryNumber, oddBinaryNumberStrings, expectedOddBinaryNumber);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsCAM DFA"
            << std::endl; // tests for containsCAM DFA
  std::vector<myString *> ContainsCAMStrings{&OZ, &ZO, &OZOO, &ZZZZ, &MAC, &epsi, &CAMOO, &OOCAM, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedContainsCAM{false, false, false, false, false, false, true, true, true, true, true, true};
  DFAtester(containsCAM, ContainsCAMStrings, expectedContainsCAM);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsLineComment DFA" << std::endl; // tests for containsLineComment DFA
  std::vector<myString *> containsLineCommentStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedContainsLineComment{true, true, false, true, true, false, true, false, false, false, false, false};
  DFAtester(containsLineComment, containsLineCommentStrings, expectedContainsLineComment);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveZerosBinary DFA" << std::endl; // tests for threeConsecutiveZerosBinary DFA
  std::vector<myString *> threeConsecutiveZerosStrings{&ZZZZ, &ZZZOO, &ZOZZZ, &ZZZZZ, &OOZZZ, &epsi, &OZZZO, &Z, &MAC, &OZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedThreeConsecutiveZeros{true, true, true, true, true, false, true, false, false, false, false, false};
  DFAtester(threeConsecutiveZerosBinary, threeConsecutiveZerosStrings, expectedThreeConsecutiveZeros);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveOnesBinary DFA"
            << std::endl; // tests for threeConsecutiveOnesBinary DFA
  std::vector<myString *> threeConsecutiveOnesStrings{&ZZZZ, &ZZZOO, &ZOZZZ, &ZZZZZ, &OOZZZ, &epsi, &ZZZZZ, &OOOZZZ, &ZOOOZZ, &ZZOOOZ, &OOO, &ZZZOOO};
  std::vector<bool> expectedConsecutiveOnes{false, false, false, false, false, false, false, true, true, true, true, true};
  DFAtester(threeConsecutiveOnesBinary, threeConsecutiveOnesStrings, expectedConsecutiveOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddNumberOfOnesBinary DFA"
            << std::endl; // tests for oddNumberOfOnesBinary DFA
  std::vector<myString *> oddNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedOddNumberOfOnes{true, true, true, false, false, false, true, false, true, false, true, false};
  DFAtester(oddNumberOfOnesBinary, oddNumberOfOnesStrings, expectedOddNumberOfOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfOnesBinary DFA"
            << std::endl; // tests for EvenNumberOfOnesBinary DFA
  std::vector<myString *> evenNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedEvenNumberOfOnes{false, false, false, true, true, true, false, true, false, true, false, true};
  DFAtester(evenNumberOfOnesBinary, evenNumberOfOnesStrings, expectedEvenNumberOfOnes);

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfZerosAndSingleOne DFA"
            << std::endl; // tests for EvenNumberOfZerosAndSingleOne DFA
  std::vector<myString *> evenNumberOfZerosAndSingleOneStrings{&OZ, &ZOZO, &ZZZZZZO, &ZZO, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZO, &OOO, &ZOZOZ};
  std::vector<bool> expectedEvenNumberZerosAndSingleOne{false, false, true, true, false, false, true, false, false, true, false, false};
  DFAtester(evenNumberOfZerosAndSingleOne, evenNumberOfZerosAndSingleOneStrings, expectedEvenNumberZerosAndSingleOne);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA UNION TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing union of oddNumberOfOnesBinary and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> anyNumberOfOnesBinary = unionDFA<myChar>(oddNumberOfOnesBinary, evenNumberOfOnesBinary);
  std::vector<myString *> anyNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedAnyNumberOfOnes{true, true, true, true, true, true, true, true, true, true, true, true};
  DFAtester(anyNumberOfOnesBinary, anyNumberOfOnesStrings, expectedAnyNumberOfOnes);

  std::cout << "Testing union of OddBinaryNumber and EvenBinaryNumber DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> evenOrOddBinary = unionDFA<myChar>(evenBinaryNumber, oddBinaryNumber);
  std::vector<myString *> evenOrOddBinaryStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedEvenOrOddBinary{true, true, true, true, true, false, true, true, true, true, true, true};
  DFAtester(evenOrOddBinary, evenOrOddBinaryStrings, expectedEvenOrOddBinary);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA INTERSECTION TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing intersection of oddNumberOfOnesBinary and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> noNumberOfOnesBinary = intersectionDFA<myChar>(oddNumberOfOnesBinary, evenNumberOfOnesBinary);
  std::vector<myString *> noNumberOfOnesStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedNoNumberOfOnes{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(noNumberOfOnesBinary, noNumberOfOnesStrings, expectedNoNumberOfOnes);

  std::cout << "Testing intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> lineCommentAndEvenNumberOfOnes = intersectionDFA<myChar>(containsLineComment, evenNumberOfOnesBinary);
  std::vector<myString *> lineCommentAndEvenNumberOfOnesStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedlineCommentAndEvenNumberOfOnes{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(lineCommentAndEvenNumberOfOnes, lineCommentAndEvenNumberOfOnesStrings, expectedlineCommentAndEvenNumberOfOnes);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA COMPLEMENT TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing complement of intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> lineCommentAndEvenNumberOfOnesComp = complementDFA<myPair<myChar, myChar>>(lineCommentAndEvenNumberOfOnes);
  std::vector<myString *> lineCommentAndEvenNumberOfOnesCompStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedlineCommentAndEvenNumberOfOnesComp{false, false, false, false, false, false, false, false, false, false, false, false};
  DFAtester(lineCommentAndEvenNumberOfOnesComp, lineCommentAndEvenNumberOfOnesCompStrings, expectedlineCommentAndEvenNumberOfOnesComp);

  std::cout << "Testing complement of oddNumberOfOnesBinary DFA" << std::endl;
  DFA<myChar> oddNumberOfOnesBinaryComp = complementDFA<myChar>(oddNumberOfOnesBinary);
  std::vector<myString *> oddNumberOfOnesBinaryCompStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedOddNumberOfOnesBinaryComp{false, false, false, true, true, true, false, true, false, true, false, true};
  DFAtester(oddNumberOfOnesBinaryComp, oddNumberOfOnesBinaryCompStrings, expectedOddNumberOfOnesBinaryComp);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      ONE CHAR DFA TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing oneCharDFA onlyAcceptsC" << std::endl;
  DFA<myChar> onlyAcceptsC = oneCharDFA(myChar('c'));
  std::vector<myString *> onlyAcceptsCStrings{&a, &b, &c, &d, &e, &f, &ccc, &epsi, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedOnlyAcceptsCStrings{false, false, true, false, false, false, false, false, false, false, false, false};
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
      std::vector<myChar>{myChar('1'), myChar('0')}, // alphabet
      myChar('B'),                                   // start state
      [](myChar a, myChar b) -> myChar {             // transition function
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
      std::vector<myChar>{},             // alphabet
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
                                },                                              // states function
                                std::vector<myChar>{myChar('0'), myChar('1')},  // alphabet
                                myChar('A'),                                    // start state
                                [](myChar a, myChar b) -> std::vector<myChar> { // transition function
                                  if (a.getVal() == 'A' && b.getVal() == '1')
                                    return std::vector<myChar>{myChar('A'), myChar('B')};
                                  else if (a.getVal() == 'A' && b.getVal() == '0')
                                    return std::vector<myChar>{myChar('A')};
                                  else if (a.getVal() == 'B' && (b.getVal() == '0' || b.getVal() == '1'))
                                    return std::vector<myChar>{myChar('C')};
                                  else if (a.getVal() == 'C' && (b.getVal() == '1' || b.getVal() == '0'))
                                    return std::vector<myChar>{myChar('D')};
                                  else if (a.getVal() == 'D' && (b.getVal() == '1' || b.getVal() == '0'))
                                    return std::vector<myChar>{myChar('A')};
                                  else
                                    return std::vector<myChar>{myChar('E')}; // may need to change this
                                },
                                [](myChar a) -> std::vector<myChar> { // epsilon transition
                                  return std::vector<myChar>{};
                                },
                                [](myChar a) -> bool { // accept states
                                  return (a.getVal() == 'D');
                                });

  NFA<myChar> numZerosIsMultipleOfTwoOrThree("NumZerosIsMultipleOfTwoOrThree", // name
                                             [](myChar a) -> bool {
                                               return (a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C' || a.getVal() == 'D' || a.getVal() == 'E' || a.getVal() == 'F' || a.getVal() == 'G');
                                             },                                              // states function
                                             std::vector<myChar>{myChar('0')},               // alphabet
                                             myChar('A'),                                    // start state
                                             [](myChar a, myChar b) -> std::vector<myChar> { // transition function
                                               if (a.getVal() == 'B' && b.getVal() == '0')
                                                 return std::vector<myChar>{myChar('C')};
                                               else if (a.getVal() == 'C' && b.getVal() == '0')
                                                 return std::vector<myChar>{myChar('B')};
                                               else if (a.getVal() == 'D' && b.getVal() == '0')
                                                 return std::vector<myChar>{myChar('E')};
                                               else if (a.getVal() == 'E' && b.getVal() == '0')
                                                 return std::vector<myChar>{myChar('F')};
                                               else if (a.getVal() == 'F' && b.getVal() == '0')
                                                 return std::vector<myChar>{myChar('D')};
                                               else
                                                 return std::vector<myChar>{myChar('G')}; // may need to change this
                                             },
                                             [](myChar a) -> std::vector<myChar> { // epsilon transition
                                               if (a.getVal() == 'A')
                                                 return std::vector<myChar>{myChar('B'), myChar('D')};
                                               else
                                                 return std::vector<myChar>{};
                                             },
                                             [](myChar a) -> bool { // accept states
                                               return (a.getVal() == 'B' || a.getVal() == 'D');
                                             });

  NFA<myChar> containsOZOorOO("ContainsOZOorOO", // name
                              [](myChar a) -> bool {
                                return (a.getVal() == 'A' || a.getVal() == 'B' || a.getVal() == 'C' || a.getVal() == 'D' || a.getVal() == 'E');
                              },                                              // states function
                              std::vector<myChar>{myChar('0'), myChar('1')},  // alphabet
                              myChar('A'),                                    // start state
                              [](myChar a, myChar b) -> std::vector<myChar> { // transition function
                                if (a.getVal() == 'A' && b.getVal() == '1')
                                  return std::vector<myChar>{myChar('A'), myChar('B')};
                                else if (a.getVal() == 'A' && b.getVal() == '0')
                                  return std::vector<myChar>{myChar('A')};
                                else if (a.getVal() == 'B' && (b.getVal() == '0'))
                                  return std::vector<myChar>{myChar('C')};
                                else if (a.getVal() == 'C' && (b.getVal() == '1'))
                                  return std::vector<myChar>{myChar('D')};
                                else if (a.getVal() == 'D' && (b.getVal() == '1' || b.getVal() == '0'))
                                  return std::vector<myChar>{myChar('D')};
                                else
                                  return std::vector<myChar>{myChar('E')}; // may need to change this
                              },
                              [](myChar a) -> std::vector<myChar> { // epsilon transition
                                if (a.getVal() == 'B')
                                  return std::vector<myChar>{myChar('C')};
                                else
                                  return std::vector<myChar>{};
                              },
                              [](myChar a) -> bool { // accept states
                                return (a.getVal() == 'D');
                              });

  // inputStrings
  oneString OZZ = oneString('1', new oneString('0', new oneString('0', new emptyString)));
  oneString OO = oneString('1', new oneString('1', new emptyString));
  oneString OOZ = oneString('1', new oneString('1', new oneString('0', new emptyString)));
  oneString OOOZ = oneString('1', new oneString('1', new oneString('1', new oneString('0', new emptyString))));
  oneString OZOO = oneString('1', new oneString('0', new oneString('1', new oneString('1', new emptyString))));
  oneString OOZOO = oneString('1', new oneString('1', new oneString('0', new oneString('1', new oneString('1', new emptyString)))));
  emptyString epsi;
  oneString ZZ = oneString('0', new oneString('0', new emptyString));
  oneString ZZZ = oneString('0', new oneString('0', new oneString('0', new emptyString)));
  oneString ZZZZZ = oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('0', new emptyString)))));

  //example traces for oneIsThirdFromEnd
  oneString A = oneString('A', new emptyString);
  oneString ABCD = oneString('A', new oneString('B', new oneString('C', new oneString('D', new emptyString))));
  oneString HI = oneString('H', new oneString('I', new emptyString));

  //example traces for numZerosIsMultipleOfTwoOrThree
  oneString ABCB = oneString('A', new oneString('B', new oneString('C', new oneString('B', new emptyString))));
  oneString ADEFD = oneString('A', new oneString('D', new oneString('E', new oneString('F', new oneString('D', new emptyString)))));
  oneString AD = oneString('A', new oneString('D', new emptyString));
  oneString AB = oneString('A', new oneString('B', new emptyString));


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
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the ZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the ZZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the ZZZZZ? " << numZerosIsMultipleOfTwoOrThree.accepts(ZZZZZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the OOZ? " << numZerosIsMultipleOfTwoOrThree.accepts(OOZ) << std::endl;
  std::cout << "Does numZerosIsMultipleOfTwoOrThree accept the OZOO? " << numZerosIsMultipleOfTwoOrThree.accepts(OZOO) << std::endl;

  // tests for containsOZOorOO
  std::cout << "Does containsOZOorOO accept the empty string? " << containsOZOorOO.accepts(epsi) << std::endl;
  std::cout << "Does containsOZOorOO accept the OZOO? " << containsOZOorOO.accepts(OZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept the ZZZ? " << containsOZOorOO.accepts(ZZZ) << std::endl;
  std::cout << "Does containsOZOorOO accept the OOOZ? " << containsOZOorOO.accepts(OOOZ) << std::endl;
  std::cout << "Does containsOZOorOO accept the OOZOO? " << containsOZOorOO.accepts(OOZOO) << std::endl;
  std::cout << "Does containsOZOorOO accept the OO? " << containsOZOorOO.accepts(OO) << std::endl;
}

void showMenu()
{
  std::cout << "1: Make and Test DFAs" << std::endl;
  std::cout << "2: Make and Test NFAs" << std::endl;
  std::cout << "3: Exit" << std::endl;
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
