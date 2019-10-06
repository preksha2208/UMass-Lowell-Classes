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

// class that is essentially std::pair, but with an overloaded << operator
template <class State1, class State2>
class myPair
{
public:
  myPair<State1, State2>(State1 data1, State2 data2) : first(data1), second(data2) {}

  State1 first;
  State2 second;
  friend myPair<State1, State2> make_MyPair(State1 data1, State2 data2)
  {
    return myPair<State1, State2>(data1, data2);
  }
  friend std::ostream &operator<<(std::ostream &os, const myPair &p)
  {
    os << p.first << p.second;
    return os;
  }
};

// class to represent char values
class myChar
{
public:
  explicit myChar(char c) { this->c = c; } // initialize char variable
  myChar() {}
  myChar(const myChar &charB) { c = charB.c; }
  void print() { std::cout << c; }
  char getVal() { return c; }
  myChar &operator=(const myChar &charB)
  {
    c = charB.c;
    return *this;
  }
  friend bool operator==(const myChar &a, const myChar &b)
  {
    return (a.c == b.c);
  }
  friend std::ostream &operator<<(std::ostream &os, const myChar &dt)
  {
    os << dt.c;
    return os;
  }

private:
  char c;
};

// class to represent strings; the strings are implemented as a
// linked list, with each node being a letter
class myString
{
public:
  myString(char c, myString *nextString) : nextString(nextString)
  {
    this->c = myChar(c);
  }
  myString() {}
  virtual void print()
  {
    c.print();
  } // prints the char value within this string
  virtual bool isEmpty() { return false; }
  virtual myString *next()
  {
    return nextString;
  } // returns the next myString object; i.e. the next letter in the string
  virtual void setNext(myString *next) { nextString = next; }
  virtual char charValue()
  {
    return c.getVal();
  } // returns the char value held by the myChar object
  virtual myChar charObject()
  {
    return c;
  } // returns the actual myChar object, rather than the char value held by the
    // myChar object

private:
  myChar c;
  myString *nextString;
};

// node of string that contains a myChar object (i.e. a letter)
class oneString : public myString
{
public:
  oneString(char c, myString *nextString) : nextString(nextString)
  {
    this->c = myChar(c);
  }
  oneString() {}
  myString *next() { return nextString; }
  void setNext(myString *next) { nextString = next; }
  bool isEmpty() { return false; }
  void print() { c.print(); }
  char charValue() { return c.getVal(); }
  myChar charObject() { return c; }

private:
  myChar c;
  myString *nextString;
};

// epsilon
class emptyString : public myString
{
public:
  emptyString() { this->c = myChar('E'); } // needs to change E->actual epsilon
  bool isEmpty() { return true; }
  myString *next() { return NULL; }
  void setNext(myString *next)
  {
  } // only doing this so that the parent function is not called
  void print() { c.print(); }
  char charValue() { return c.getVal(); }
  myChar charObject() { return c; }

private:
  myChar c;
};

template <class State>
class DFA
{
public:
  DFA<State>(std::string name, std::function<bool(State)> Q,
             std::list<myChar> alphabet, State q0,
             std::function<State(State, myChar)> transFunc,
             std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        F(F) {}

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
        std::cout << stringStack.top().getVal();
        stringStack.pop();
        temp = temp->next();
      }
      temp->setNext(new emptyString);
      std::cout << std::endl;

      return std::make_pair<bool, myString *>(true, &beginning);
    }
  }

  std::string name;
  std::function<bool(State)> Q; // list of possible states for this dfa
  std::list<myChar> alphabet;
  State q0;                                      // start state
  std::function<State(State, myChar)> transFunc; // transition function
  std::function<bool(State)> F;                  // accept states

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

// returns a DFA that only accepts a string of length one that only contains inputChar
DFA<myChar> oneCharDFA(myChar inputChar)
{
  return DFA<myChar>(
      "onlyAccepts" + std::string(1, inputChar.getVal()),
      [=](myChar a) -> bool { return (a == myChar('A') || a == myChar('B') || a == myChar('C')); },
      std::list<myChar>{inputChar}, myChar('A'),
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
template <class State>
DFA<myPair<State, State>> unionDFA(DFA<State> dfa1, DFA<State> dfa2)
{
  std::list<myChar> a = dfa1.alphabet;
  std::list<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs

  return DFA<myPair<State, State>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State, State> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                                              // alphabet
      myPair<State, State>(dfa1.q0, dfa2.q0),                         // start state, need to figure this one out
      [=](myPair<State, State> a, myChar b) -> myPair<State, State> { // transition function; not correct as is
        return (myPair<State, State>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State, State> a) { // accept states
        return ((dfa1.F(a.first)) || (dfa2.F(a.second)));
      });
}

// creates a DFA that is the intersection of dfa1 and dfa2
template <class State>
DFA<myPair<State, State>> intersectionDFA(DFA<State> dfa1, DFA<State> dfa2)
{
  std::list<myChar> a = dfa1.alphabet;
  std::list<myChar> b = dfa2.alphabet;
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs

  return DFA<myPair<State, State>>(
      "Union of " + dfa1.name + " and " + dfa2.name,
      [=](myPair<State, State> a) -> bool { // function for possible states
        return (dfa1.Q(a.first) && dfa2.Q(a.second));
      },
      a,                                      // alphabet
      myPair<State, State>(dfa1.q0, dfa2.q0), // start state, need to figure this one out
      [=](myPair<State, State> a, myChar b) -> myPair<State, State> {
        return (myPair<State, State>(dfa1.transFunc(a.first, b), dfa2.transFunc(a.second, b)));
      },
      [=](myPair<State, State> a) {                       // accept states
        return ((dfa1.F(a.first)) && (dfa2.F(a.second))); // only difference from unionDFA function
      });
}

// returns true/false to indicate whether dfa1 is a subset of dfa2
template <class State>
bool subsetDFA(DFA<State> dfa1, DFA<State> dfa2)
{
  DFA<State> dfa3 = intersectionDFA(dfa1, complementDFA(dfa2));
  return (dfa3.acceptedString().first ? false : true); // if dfa3 accepts nothing, then dfa1 is a subset of dfa2
}

template <class State>
bool equalityDFA(DFA<State> dfa1, DFA<State> dfa2)
{
  DFA<State> dfa3 = unionDFA(intersectionDFA(dfa1, complementDFA(dfa2)), intersectionDFA(complementDFA(dfa1), dfa2));
  return dfa3.acceptedString().first ? false : true; // if dfa3 accepts nothing, then dfa1 and dfa2 are equal
}

// takes in dfa, vector of test strings and expected values for the test strings on the given dfa
// bool values are at same index in bool vector as their corresponding test string in the other vector
template <class State>
void DFAtester(DFA<State> a, std::vector<myString *> testStrings, std::vector<bool> expected)
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

void makeAndTestDFAs() // creates 12 DFAs, runs 12 tests on each DFA, prints results
{
  // Declarations of DFAs
  DFA<myChar> evenLengthBinary( // returns whether length of inputted binary
                                // number is even
      "EvenLengthBinary",       // name
      [](myChar a) -> bool {    // state function
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('0'), myChar('1')}, // alphabet
      myChar('A'),                                 // start state
      [](myChar a, myChar b) -> myChar {           // transition function
        if ((a.getVal() == 'A') && ((b.getVal() == '0') || (b.getVal() == '1')))
          return myChar('B');
        else if ((a.getVal() == 'B') &&
                 ((b.getVal() == '0') || (b.getVal() == '1')))
          return myChar('A');
        else
          return a;
      },
      [](myChar a) -> bool { // accept states
        return (a.getVal() == 'A');
      });

  DFA<myChar> acceptsNothing( // accepts nothing, as the name implies
      "AcceptsNothing", [](myChar a) -> bool { return false; },
      std::list<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar { return a; },
      [](myChar a) -> bool { return false; });

  DFA<myChar> onlyAcceptsEmptyString( // only accepted input is the empty string
      "OnlyAcceptsEmptyString",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('E')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() != 'E')
          return myChar('B');
        else if (a.getVal() == 'B')
          return myChar('B');
        else
          return a;
      },
      [](myChar a) -> bool { return (a == myChar('A')); });
  DFA<myChar> evenBinaryNumber( // returns whether the inputted binary number is even
      "EvenBinaryNumber",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '0')
          return myChar('B');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });

  DFA<myChar> oddBinaryNumber( // returns whether the inputted binary number is odd
      "OddBinaryNumber",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if ((a.getVal() == 'A' || a.getVal() == 'B') && b.getVal() == '1')
          return myChar('B');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });

  DFA<myChar> containsCAM( // returns whether the input contains my name
                           // somewhere in it
      "ContainsCAM",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      std::list<myChar>{myChar('C'), myChar('A'), myChar('M')}, myChar('A'),
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
      std::list<myChar>{myChar('/')}, myChar('A'),
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
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '1')
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
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> threeConsecutiveOnesBinary( // returns whether the inputted binary number
                                          // has three consecutive ones in it
      "ThreeConsecutiveOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '1')
          return myChar('C');
        else if (a.getVal() == 'C' && b.getVal() == '1')
          return myChar('D');
        else if (a.getVal() == 'D' && b.getVal() == '1')
          return myChar('B');
        else if (a.getVal() == 'D' && b.getVal() == '0')
          return myChar('D');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> oddNumberOfOnesBinary( // returns whether the inputted binary
                                     // number has an odd number of ones in it
      "OddNumberOfOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });
  DFA<myChar> evenNumberOfOnesBinary( // returns whether the inputted number has
                                      // an even number of ones in it
      "NumberOfOnesBinary",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
          return myChar('A');
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
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
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
  std::vector<bool> expectedlineCommentAndEvenNumberOfOnes{true, true, false, true, true, false, true, false, false, false, false, false};
  DFAtester(lineCommentAndEvenNumberOfOnes, lineCommentAndEvenNumberOfOnesStrings, expectedlineCommentAndEvenNumberOfOnes);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      DFA COMPLEMENT TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing complement of intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myPair<myChar, myChar>> lineCommentAndEvenNumberOfOnesComp = complementDFA<myPair<myChar, myChar>>(lineCommentAndEvenNumberOfOnes);
  std::vector<myString *> lineCommentAndEvenNumberOfOnesCompStrings{&comment1, &comment2, &comment3, &comment4, &comment5, &epsi, &comment6, &comment7, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedlineCommentAndEvenNumberOfOnesComp{false, false, true, false, false, true, false, true, true, true, true, true};
  DFAtester(lineCommentAndEvenNumberOfOnesComp, lineCommentAndEvenNumberOfOnesCompStrings, expectedlineCommentAndEvenNumberOfOnesComp);

  std::cout << "Testing complement of oddNumberOfOnesBinary DFA" << std::endl;
  DFA<myChar> oddNumberOfOnesBinaryComp = complementDFA<myChar>(oddNumberOfOnesBinary);
  std::vector<myString *> oddNumberOfOnesBinaryCompStrings{&OZ, &ZO, &OZOO, &ZZZZ, &OOOOOO, &epsi, &O, &Z, &ZOZ, &ZZZZZ, &OOO, &ZOZOZ};
  std::vector<bool> expectedOddNumberOfOnesBinaryComp{false, false, false, true, true, true, false, true, false, true, false, true};
  DFAtester(oddNumberOfOnesBinaryComp, oddNumberOfOnesBinaryCompStrings, expectedOddNumberOfOnesBinaryComp);

  std::cout << "---------------------------------------------------------------" << std::endl;
  std::cout << "                      oneCharDFA TESTS                     " << std::endl;
  std::cout << "---------------------------------------------------------------" << std::endl
            << std::endl;

  std::cout << "Testing complement of intersection of containsLineComment and evenNumberOfOnesBinary DFAs" << std::endl;
  DFA<myChar> onlyAcceptsC = oneCharDFA(myChar('c'));
  std::vector<myString *> onlyAcceptsCStrings{&a , &b, &c, &d, &e, &f, &ccc, &epsi, &OCAMO, &CACAMM, &CAMCAM, &CAMERA};
  std::vector<bool> expectedOnlyAcceptsCStrings{false, false, true, false, false, false, false, false, false, false, false, false};
  DFAtester(onlyAcceptsC, onlyAcceptsCStrings, expectedOnlyAcceptsCStrings);
}

int main()
{
  makeAndTestDFAs();
  return 0;
}
