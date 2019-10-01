// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
#include <functional>

class myChar // class to represent char values
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

class myString // class to represent strings; the strings are implemented as a linked list, with each node being a letter
{
public:
  myString(char c, myString *nextString) : nextString(nextString)
  {
    this->c = myChar(c);
  }
  myString() {}
  virtual void print() { c.print(); } // prints the char value within this string
  virtual bool isEmpty() { return false; }
  virtual myString *next() { return nextString; } // returns the next myString object; i.e. the next letter in the string
  virtual void setNext(myString *next) { nextString = next; }
  virtual char charValue() { return c.getVal(); } // returns the char value held by the myChar object
  virtual myChar charObject() { return c; }       // returns the actual myChar object, rather than the char value held by the myChar object

private:
  myChar c;
  myString *nextString;
};

class oneString : public myString // node of string that contains a myChar object (i.e. a letter)
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

class emptyString : public myString
{
public:
  emptyString() { this->c = myChar('E'); } // needs to change E->actual epsilon
  bool isEmpty() { return true; }
  myString *next() { return NULL; }
  void setNext(myString *next) {} // only doing this so that the parent function is not called
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
  DFA<State>(std::string name, std::function<bool(State)> Q, std::list<myChar> alphabet,
             State q0, std::function<State(State, myChar)> transFunc, std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        F(F) {}
  std::string getName() { return name; }
  void printName() { std::cout << name << std::endl; }
  std::list<myChar> getAlphabet() { return alphabet; }

  bool accepts(myString &inputString) // returns true/false to indicate whether this DFA accepts the inputString
  {                                   // does DFA accept inputString?
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

  void trace(myString &inputString) // returns the trace for the inputString on this DFA
  {                                 // does DFA accept inputString?
    State qi = this->q0;
    myString *temp = &inputString;
    std::cout << qi;
    // step through DFA with the input string
    while (temp->isEmpty() != true)
    {
      qi = transFunc(qi, temp->charObject());
      temp = temp->next();
      std::cout << qi;
    }
  }

  bool acceptStates(myChar b) { return F(b); }
  State transitionFunction(State a, myChar b) { return transFunc(a, b); }

  DFA<State> complementDFA(DFA<State> inputDFA)
  {
    std::function<bool(State)> F2 = [=](State a) -> bool {
      if (F(a) == true)
        return false;
      return true;
    };

  return DFA<State>("complement of " + inputDFA.name,
                  Q,
                  alphabet,
                  q0,
                  transFunc,
                  F2);
  }

private:
  //auto acceptedString(myString* currentNode, std::unordered_map<int, myChar>);
  std::string name;
  std::function<bool(State)> Q; // list of possible states for this dfa
  std::list<myChar> alphabet;
  State q0;                                      // start state
  std::function<State(State, myChar)> transFunc; // DFA transition function
  std::function<bool(State)> F;                  // accept states
};

/*
template <class State>
DFA<State> UnionDFA(DFA<State> dfa1, DFA<State> dfa2)
{
  std::list<myChar> a = dfa1.getAlphabet();
  std::list<myChar> b = dfa2.getAlphabet();
  a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs

  return DFA("Union of " + dfa1.getName() + " and " + dfa2.getName(),
             [](State a) -> bool { // function for possible states

             },
             a,                      // alphabet
             myChar('A'),            // start state, need to figure this one out
             [](State a, myChar b) { // transition function; not correct as is
             },
             [](State a) { // accept states
               return ((dfa1.acceptStates(a)) || (dfa2.acceptStates(a)));
             }

  );
} 
*/
/*
myString lexi(std::list<myString> alphabet)
{
}
*/

// makes a DFA that only accepts a string of just one of the inputted Char
DFA<myChar> oneCharDFA(myChar inputChar)
{
  std::function<bool(myChar)> sFunc = [=](myChar a) -> bool {
    return (a == myChar('A') || a == myChar('B'));
  };

  std::function<myChar(myChar, myChar)> tFunc = [&](myChar a, myChar b) -> myChar {
    if (a.getVal() == 'A' && (b.getVal() == inputChar.getVal()))
      return myChar('B');
    else
      return myChar('C');
  };

  std::function<bool(myChar)> fFunc = [](myChar a) -> bool {
    return (a == myChar('B'));
  };

  return DFA<myChar>("onlyAccepts" + std::string(1, inputChar.getVal()),
                     sFunc,
                     std::list<myChar>{inputChar},
                     myChar('A'),
                     tFunc,
                     fFunc);
}

void makeAndTestDFAs() // creates 12 DFAs, runs 12 tests on each DFA, and prints the results to the console
{
  // Declarations of DFAs
  DFA<myChar> evenLengthBinary( // returns whether length of inputted binary number is even
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

  DFA<myChar> containsCAM( // returns whether the input contains my name somewhere in it
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

  DFA<myChar> containsLineComment( // returns whether the input contains a double slash somewhere in it, indicating a comment
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

  DFA<myChar> threeConsecutiveZerosBinary( // returns whether the inputted binary number has three consecutive zeros in it
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

  DFA<myChar> threeConsecutiveOnesBinary( // returns whether the inputted binary number has three consecutive ones in it
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

  DFA<myChar> oddNumberOfOnesBinary( // returns whether the inputted binary number has an odd number of ones in it
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
  DFA<myChar> evenNumberOfOnesBinary( // returns whether the inputted number has an even number of ones in it
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

  DFA<myChar> evenNumberOfZerosAndSingleOne( // returns whether the inputted number has an even number of zeroes followed by a single one in it
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
  oneString ZOZO = oneString('0', new oneString('1', new oneString('0', new oneString('1', new emptyString))));
  oneString OO = oneString('1', new oneString('1', new emptyString));
  oneString ZZZOO = oneString('0', new oneString('0', new oneString('0', new oneString('1', new oneString('1', new emptyString)))));
  oneString OZZZO = oneString('1', new oneString('0', new oneString('0', new oneString('0', new oneString('1', new emptyString)))));
  oneString OOZZZ = oneString('1', new oneString('1', new oneString('0', new oneString('0', new oneString('0', new emptyString)))));
  oneString ZOZOOO = oneString('0', new oneString('1', new oneString('0', new oneString('1', new oneString('1', new oneString('1', new emptyString))))));
  oneString ZOZZZ = oneString('0', new oneString('1', new oneString('0', new oneString('0', new oneString('0', new emptyString)))));
  oneString ZZ = oneString('0', new oneString('0', new emptyString));
  oneString ZZZZO = oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('1', new emptyString)))));
  oneString OOOOOO = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              '1', new oneString(
                       '1', new oneString(
                                '1', new oneString('1', new emptyString))))));

  emptyString epsi;
  oneString OOOZZZ = oneString('1', new oneString('1', new oneString('1', new oneString('0', new oneString('0', new oneString('0', new emptyString))))));
  oneString ZOOOZZ = oneString('0', new oneString('1', new oneString('1', new oneString('1', new oneString('0', new oneString('0', new emptyString))))));
  oneString ZZOOOZ = oneString('0', new oneString('0', new oneString('1', new oneString('1', new oneString('1', new oneString('0', new emptyString))))));
  oneString ZZZOOO = oneString('0', new oneString('0', new oneString('0', new oneString('1', new oneString('1', new oneString('1', new emptyString))))));
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
  oneString ZZO = oneString('0', new oneString('0', new oneString('1', new emptyString)));
  oneString ZZZZZZO = oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('0', new oneString('1', new emptyString)))))));
  oneString CAMOO = oneString('C', new oneString('A', new oneString('M', new oneString('1', new oneString('1', new emptyString)))));
  oneString OOCAM = oneString('1', new oneString('1', new oneString('C', new oneString('A', new oneString('M', new emptyString)))));
  oneString OCAMO = oneString('1', new oneString('C', new oneString('A', new oneString('M', new oneString('1', new emptyString)))));
  oneString CACAMM = oneString('C', new oneString('A', new oneString('C', new oneString('A', new oneString('M', new oneString('M', new emptyString))))));
  oneString CACM = oneString('C', new oneString('A', new oneString('C', new oneString('M', new emptyString))));
  oneString CAMCAM = oneString('C', new oneString('A', new oneString('M', new oneString('C', new oneString('A', new oneString('M', new emptyString))))));
  oneString CAMERA = oneString('C', new oneString('A', new oneString('M', new oneString('E', new oneString('R', new oneString('A', new emptyString))))));
  oneString MAC = oneString('M', new oneString('A', new oneString('C', new emptyString)));
  oneString comment1 = oneString('/', new oneString('/', new oneString('H', new emptyString)));
  oneString comment2 = oneString('H', new oneString('/', new oneString('/', new emptyString)));
  oneString comment3 = oneString('/', new oneString('H', new oneString('/', new emptyString)));
  oneString comment4 = oneString('/', new oneString('/', new oneString('/', new emptyString)));
  oneString comment5 = oneString('/', new oneString('/', new oneString('/', new oneString('H', new emptyString))));
  oneString comment6 = oneString('H', new oneString('/', new oneString('/', new oneString('/', new emptyString))));
  oneString comment7 = oneString('/', new emptyString);

  std::cout << std::boolalpha;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenLengthBinary DFA: " << std::endl; // Tests for EvenLengthBinaryDFA
  std::cout << "2: Does EvenLengthBinary accept 10?" << evenLengthBinary.accepts(OZ);
  std::cout << "Trace for 10: ";
  evenLengthBinary.trace(OZ); // calls trace function
  std::cout << std::endl;
  std::cout << "2: Does EvenLengthBinary accept 01? " << evenLengthBinary.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  evenLengthBinary.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does EvenLengthBinary accept 1011? " << evenLengthBinary.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  evenLengthBinary.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does EvenLengthBinary accept 0000? " << evenLengthBinary.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  evenLengthBinary.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does EvenLengthBinary accept 111111? "
            << evenLengthBinary.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  evenLengthBinary.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does EvenLengthBinary accept the empty string? "
            << evenLengthBinary.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  evenLengthBinary.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does EvenLengthBinary accept 1? " << evenLengthBinary.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  evenLengthBinary.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does EvenLengthBinary accept 0? " << evenLengthBinary.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenLengthBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does EvenLengthBinary accept 010? " << evenLengthBinary.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenLengthBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does EvenLengthBinary accept 00000? " << evenLengthBinary.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  evenLengthBinary.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does EvenLengthBinary accept 111? " << evenLengthBinary.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  evenLengthBinary.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does EvenLengthBinary accept 01010? " << evenLengthBinary.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  evenLengthBinary.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing AcceptsNothing DFA" << std::endl; // tests for AcceptsNothing DFA
  std::cout << "1: Does AcceptsNothing accept 10? " << acceptsNothing.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  acceptsNothing.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does AcceptsNothing accept 01? " << acceptsNothing.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  acceptsNothing.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does AcceptsNothing accept 1011? " << acceptsNothing.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  acceptsNothing.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does AcceptsNothing accept 0000? " << acceptsNothing.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  acceptsNothing.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does AcceptsNothing accept 111111? "
            << acceptsNothing.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  acceptsNothing.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does AcceptsNothing accept the empty string? "
            << acceptsNothing.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  acceptsNothing.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does AcceptsNothing accept 1? " << acceptsNothing.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  acceptsNothing.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does AcceptsNothing accept 0? " << acceptsNothing.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  acceptsNothing.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does AcceptsNothing accept 010? " << acceptsNothing.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  acceptsNothing.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does AcceptsNothing accept 00000? " << acceptsNothing.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  acceptsNothing.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does AcceptsNothing accept 111? " << acceptsNothing.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  acceptsNothing.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does AcceptsNothing accept 01010? " << acceptsNothing.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  acceptsNothing.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OnlyAcceptsEmptyString DFA" << std::endl; // tests for onlyAcceptsEmptyString DFA
  std::cout << "1: Does OnlyAcceptsEmptyString accept 10? " << onlyAcceptsEmptyString.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  onlyAcceptsEmptyString.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does OnlyAcceptsEmptyString accept 01? " << onlyAcceptsEmptyString.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  onlyAcceptsEmptyString.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does OnlyAcceptsEmptyString accept 1011? " << onlyAcceptsEmptyString.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  onlyAcceptsEmptyString.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does OnlyAcceptsEmptyString accept 0000? " << onlyAcceptsEmptyString.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  onlyAcceptsEmptyString.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does OnlyAcceptsEmptyString accept 111111? "
            << onlyAcceptsEmptyString.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  onlyAcceptsEmptyString.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does OnlyAcceptsEmptyString accept the empty string? "
            << onlyAcceptsEmptyString.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  onlyAcceptsEmptyString.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does OnlyAcceptsEmptyString accept 1? " << onlyAcceptsEmptyString.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  onlyAcceptsEmptyString.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does OnlyAcceptsEmptyString accept 0? " << onlyAcceptsEmptyString.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  onlyAcceptsEmptyString.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does OnlyAcceptsEmptyString accept 010? " << onlyAcceptsEmptyString.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  onlyAcceptsEmptyString.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does OnlyAcceptsEmptyString accept 00000? " << onlyAcceptsEmptyString.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  acceptsNothing.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does OnlyAcceptsEmptyString accept 111? " << acceptsNothing.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  onlyAcceptsEmptyString.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does OnlyAcceptsEmptyString accept 01010? " << onlyAcceptsEmptyString.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  onlyAcceptsEmptyString.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenBinaryNumber DFA" << std::endl; // tests for evenBinaryNumber DFA
  std::cout << "1: Does EvenBinaryNumber accept 10? " << evenBinaryNumber.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  evenBinaryNumber.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does EvenBinaryNumber accept 01? " << evenBinaryNumber.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  evenBinaryNumber.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does EvenBinaryNumber accept 1011? " << evenBinaryNumber.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  evenBinaryNumber.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does EvenBinaryNumber accept 0000? " << evenBinaryNumber.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  evenBinaryNumber.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does EvenBinaryNumber accept 111111? "
            << evenBinaryNumber.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  evenBinaryNumber.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does EvenBinaryNumber accept the empty string? "
            << evenBinaryNumber.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  evenBinaryNumber.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does EvenBinaryNumber accept 1? " << evenBinaryNumber.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  evenBinaryNumber.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does EvenBinaryNumber accept 0? " << evenBinaryNumber.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenBinaryNumber.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does EvenBinaryNumber accept 010? " << evenBinaryNumber.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenBinaryNumber.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does EvenBinaryNumber accept 00000? " << evenBinaryNumber.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  evenBinaryNumber.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does EvenBinaryNumber accept 111? " << evenBinaryNumber.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  evenBinaryNumber.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does EvenBinaryNumber accept 01010? " << evenBinaryNumber.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  evenBinaryNumber.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddBinaryNumber DFA" << std::endl; // tests for OddBinaryNumber DFA
  std::cout << "1: Does OddBinaryNumber accept 10? " << oddBinaryNumber.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  oddBinaryNumber.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does OddBinaryNumber accept 01? " << oddBinaryNumber.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  oddBinaryNumber.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does OddBinaryNumber accept 1011? " << oddBinaryNumber.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  oddBinaryNumber.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does OddBinaryNumber accept 0000? " << oddBinaryNumber.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  oddBinaryNumber.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does OddBinaryNumber accept 111111? "
            << oddBinaryNumber.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  oddBinaryNumber.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does OddBinaryNumber accept the empty string? "
            << oddBinaryNumber.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  oddBinaryNumber.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does OddBinaryNumber accept 1? " << oddBinaryNumber.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  oddBinaryNumber.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does OddBinaryNumber accept 0? " << oddBinaryNumber.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  oddBinaryNumber.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does OddBinaryNumber accept 010? " << oddBinaryNumber.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  oddBinaryNumber.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does OddBinaryNumber accept 00000? " << oddBinaryNumber.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  oddBinaryNumber.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does OddBinaryNumber accept 111? " << oddBinaryNumber.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  oddBinaryNumber.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does OddBinaryNumber accept 01010? " << oddBinaryNumber.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  oddBinaryNumber.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsCAM DFA" << std::endl; // tests for containsCAM DFA
  std::cout << "1: Does ContainsCAM accept 10? " << containsCAM.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  containsCAM.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does ContainsCAM accept 01? " << containsCAM.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  containsCAM.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does ContainsCAM accept 1011? " << containsCAM.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  containsCAM.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does ContainsCAM accept 0000? " << containsCAM.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  containsCAM.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does ContainsCAM accept MAC? "
            << containsCAM.accepts(MAC) << std::endl;
  std::cout << "Trace for MAC: ";
  containsCAM.trace(MAC);
  std::cout << std::endl;
  std::cout << "6: Does ContainsCAM accept the empty string? "
            << containsCAM.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  containsCAM.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does ContainsCAM accept CAM11? " << containsCAM.accepts(CAMOO)
            << std::endl;
  std::cout << "Trace for CAM11: ";
  containsCAM.trace(CAMOO);
  std::cout << std::endl;
  std::cout << "8: Does ContainsCAM accept 11CAM? " << containsCAM.accepts(OOCAM)
            << std::endl;
  std::cout << "Trace for 11CAM: ";
  containsCAM.trace(OOCAM);
  std::cout << std::endl;
  std::cout << "9: Does ContainsCAM accept 1CAM1? " << containsCAM.accepts(OCAMO)
            << std::endl;
  std::cout << "Trace for 1CAM1: ";
  containsCAM.trace(OCAMO);
  std::cout << std::endl;
  std::cout << "10:Does ContainsCAM accept CACAMM? " << containsCAM.accepts(CACAMM)
            << std::endl;
  std::cout << "Trace for CACAMM: ";
  containsCAM.trace(CACAMM);
  std::cout << std::endl;
  std::cout << "11: Does ContainsCAM accept CAMCAM? " << containsCAM.accepts(CAMCAM)
            << std::endl;
  std::cout << "Trace for CAMCAM: ";
  containsCAM.trace(CAMCAM);
  std::cout << std::endl;
  std::cout << "12: Does ContainsCAM accept CAMERA? " << containsCAM.accepts(CAMERA)
            << std::endl;
  std::cout << "Trace for CAMERA: ";
  containsCAM.trace(CAMERA);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsLineComment DFA" << std::endl; // tests for containsLineComment DFA
  std::cout << "1: Does ContainsLineComment accept //H? " << containsLineComment.accepts(comment1)
            << std::endl;
  std::cout << "Trace for //H: ";
  containsLineComment.trace(comment1);
  std::cout << std::endl;
  std::cout << "2: Does ContainsLineComment accept H//? " << containsLineComment.accepts(comment2)
            << std::endl;
  std::cout << "Trace for 01: ";
  containsLineComment.trace(comment2);
  std::cout << std::endl;
  std::cout << "3: Does ContainsLineComment accept /H/? " << containsLineComment.accepts(comment3)
            << std::endl;
  std::cout << "Trace for /H/: ";
  containsLineComment.trace(comment3);
  std::cout << std::endl;
  std::cout << "4: Does ContainsLineComment accept ///? " << containsLineComment.accepts(comment4)
            << std::endl;
  std::cout << "Trace for ///: ";
  containsLineComment.trace(comment4);
  std::cout << std::endl;
  std::cout << "5: Does ContainsLineComment accept ///H? "
            << containsLineComment.accepts(comment5) << std::endl;
  std::cout << "Trace for ///H: ";
  containsLineComment.trace(comment5);
  std::cout << std::endl;
  std::cout << "6: Does ContainsLineComment accept the empty string? "
            << containsLineComment.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  containsLineComment.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does ContainsLineComment accept H///? " << containsLineComment.accepts(comment6)
            << std::endl;
  std::cout << "Trace for H///: ";
  containsLineComment.trace(comment6);
  std::cout << std::endl;
  std::cout << "8: Does ContainsLineComment accept /? " << containsLineComment.accepts(comment7)
            << std::endl;
  std::cout << "Trace for /: ";
  containsLineComment.trace(comment7);
  std::cout << std::endl;
  std::cout << "9: Does ContainsLineComment accept 1CAM1? " << containsLineComment.accepts(OCAMO)
            << std::endl;
  std::cout << "Trace for 1CAM1: ";
  containsLineComment.trace(OCAMO);
  std::cout << std::endl;
  std::cout << "10:Does ContainsLineComment accept CACAMM? " << containsLineComment.accepts(CACAMM)
            << std::endl;
  std::cout << "Trace for CACAMM: ";
  containsLineComment.trace(CACAMM);
  std::cout << std::endl;
  std::cout << "11: Does ContainsLineComment accept CAMCAM? " << containsLineComment.accepts(CAMCAM)
            << std::endl;
  std::cout << "Trace for CAMCAM: ";
  containsLineComment.trace(CAMCAM);
  std::cout << std::endl;
  std::cout << "12: Does ContainsLineComment accept CAMERA? " << containsLineComment.accepts(CAMERA)
            << std::endl;
  std::cout << "Trace for CAMERA: ";
  containsLineComment.trace(CAMERA);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveZerosBinary DFA" << std::endl; // tests for threeConsecutiveZerosBinary DFA
  std::cout << "1: Does ThreeConsecutiveZerosBinary accept 0000 " << threeConsecutiveZerosBinary.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  threeConsecutiveZerosBinary.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "2: Does ThreeConsecutiveZerosBinary accept 00011? " << threeConsecutiveZerosBinary.accepts(ZZZOO)
            << std::endl;
  std::cout << "Trace for 00011: ";
  threeConsecutiveZerosBinary.trace(ZZZOO);
  std::cout << std::endl;
  std::cout << "3: Does ThreeConsecutiveZerosBinary accept 01000? " << threeConsecutiveZerosBinary.accepts(ZOZZZ)
            << std::endl;
  std::cout << "Trace for 01000: ";
  threeConsecutiveZerosBinary.trace(ZOZZZ);
  std::cout << std::endl;
  std::cout << "4: Does ThreeConsecutiveZerosBinary accept 00000? " << threeConsecutiveZerosBinary.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  threeConsecutiveZerosBinary.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does ThreeConsecutiveZerosBinary accept 11000? "
            << threeConsecutiveZerosBinary.accepts(OOZZZ) << std::endl;
  std::cout << "Trace for 11000: ";
  threeConsecutiveZerosBinary.trace(OOZZZ);
  std::cout << std::endl;
  std::cout << "6: Does ThreeConsecutiveZerosBinary accept the empty string? "
            << threeConsecutiveZerosBinary.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  threeConsecutiveZerosBinary.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does ThreeConsecutiveZerosBinary accept 10001? " << threeConsecutiveZerosBinary.accepts(OZZZO)
            << std::endl;
  std::cout << "Trace for 10001: ";
  threeConsecutiveZerosBinary.trace(OZZZO);
  std::cout << std::endl;
  std::cout << "8: Does ThreeConsecutiveZerosBinary accept 0? " << threeConsecutiveZerosBinary.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  threeConsecutiveZerosBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does ThreeConsecutiveZerosBinary accept MAC? " << threeConsecutiveZerosBinary.accepts(MAC)
            << std::endl;
  std::cout << "Trace for MAC: ";
  threeConsecutiveZerosBinary.trace(MAC);
  std::cout << std::endl;
  std::cout << "10:Does ThreeConsecutiveZerosBinary accept 10? " << threeConsecutiveZerosBinary.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  threeConsecutiveZerosBinary.trace(OZ);
  std::cout << std::endl;
  std::cout << "11: Does ThreeConsecutiveZerosBinary accept 111? " << threeConsecutiveZerosBinary.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  threeConsecutiveZerosBinary.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does ThreeConsecutiveZerosBinary accept 01010? " << threeConsecutiveZerosBinary.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  threeConsecutiveZerosBinary.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveOnesBinary DFA" << std::endl; // tests for threeConsecutiveOnesBinary DFA
  std::cout << "1: Does ThreeConsecutiveOnesBinary accept 0000 " << threeConsecutiveOnesBinary.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  threeConsecutiveOnesBinary.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "2: Does ThreeConsecutiveOnesBinary accept 00011? " << threeConsecutiveOnesBinary.accepts(ZZZOO)
            << std::endl;
  std::cout << "Trace for 00011: ";
  threeConsecutiveOnesBinary.trace(ZZZOO);
  std::cout << std::endl;
  std::cout << "3: Does ThreeConsecutiveOnesBinary accept 01000? " << threeConsecutiveOnesBinary.accepts(ZOZZZ)
            << std::endl;
  std::cout << "Trace for 01000: ";
  threeConsecutiveOnesBinary.trace(ZOZZZ);
  std::cout << std::endl;
  std::cout << "4: Does ThreeConsecutiveOnesBinary accept 00000? " << threeConsecutiveOnesBinary.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  threeConsecutiveOnesBinary.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does ThreeConsecutiveOnesBinary accept 11000? "
            << threeConsecutiveOnesBinary.accepts(OOZZZ) << std::endl;
  std::cout << "Trace for 11000: ";
  threeConsecutiveOnesBinary.trace(OOZZZ);
  std::cout << std::endl;
  std::cout << "6: Does ThreeConsecutiveOnesBinary accept the empty string? "
            << threeConsecutiveOnesBinary.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  threeConsecutiveOnesBinary.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does ThreeConsecutiveOnesBinary accept 111111? " << threeConsecutiveOnesBinary.accepts(OOOOOO)
            << std::endl;
  std::cout << "Trace for 111111: ";
  threeConsecutiveOnesBinary.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "8: Does ThreeConsecutiveOnesBinary accept 111000? " << threeConsecutiveOnesBinary.accepts(OOOZZZ)
            << std::endl;
  std::cout << "Trace for 111000: ";
  threeConsecutiveOnesBinary.trace(OOOZZZ);
  std::cout << std::endl;
  std::cout << "9: Does ThreeConsecutiveOnesBinary accept 011100? " << threeConsecutiveOnesBinary.accepts(ZOOOZZ)
            << std::endl;
  std::cout << "Trace for 011100: ";
  threeConsecutiveOnesBinary.trace(ZOOOZZ);
  std::cout << std::endl;
  std::cout << "10:Does ThreeConsecutiveOnesBinary accept 001110? " << threeConsecutiveOnesBinary.accepts(ZZOOOZ)
            << std::endl;
  std::cout << "Trace for 001110: ";
  threeConsecutiveOnesBinary.trace(ZZOOOZ);
  std::cout << std::endl;
  std::cout << "11: Does ThreeConsecutiveOnesBinary accept 111? " << threeConsecutiveOnesBinary.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  threeConsecutiveOnesBinary.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does ThreeConsecutiveOnesBinary accept 000111? " << threeConsecutiveOnesBinary.accepts(ZZZOOO)
            << std::endl;
  std::cout << "Trace for 000111: ";
  threeConsecutiveOnesBinary.trace(ZZZOOO);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddNumberOfOnesBinary DFA" << std::endl; // tests for oddNumberOfOnesBinary DFA
  std::cout << "1: Does OddNumberOfOnesBinary accept 10? " << oddNumberOfOnesBinary.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  oddNumberOfOnesBinary.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does OddNumberOfOnesBinary accept 01? " << oddNumberOfOnesBinary.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  oddNumberOfOnesBinary.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does OddNumberOfOnesBinary accept 1011? " << oddNumberOfOnesBinary.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  oddNumberOfOnesBinary.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does OddNumberOfOnesBinary accept 0000? " << oddNumberOfOnesBinary.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  oddNumberOfOnesBinary.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does OddNumberOfOnesBinary accept 111111? "
            << oddNumberOfOnesBinary.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  oddNumberOfOnesBinary.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does OddNumberOfOnesBinary accept the empty string? "
            << oddNumberOfOnesBinary.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  oddNumberOfOnesBinary.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does OddNumberOfOnesBinary accept 1? " << oddNumberOfOnesBinary.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  oddNumberOfOnesBinary.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does OddNumberOfOnesBinary accept 0? " << oddNumberOfOnesBinary.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  oddNumberOfOnesBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does OddNumberOfOnesBinary accept 010? " << oddNumberOfOnesBinary.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  oddNumberOfOnesBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does OddNumberOfOnesBinary accept 00000? " << oddNumberOfOnesBinary.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  oddNumberOfOnesBinary.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does OddNumberOfOnesBinary accept 111? " << oddNumberOfOnesBinary.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  oddNumberOfOnesBinary.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does OddNumberOfOnesBinary accept 01010? " << oddNumberOfOnesBinary.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  oddNumberOfOnesBinary.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfOnesBinary DFA" << std::endl; // tests for EvenNumberOfOnesBinary DFA
  std::cout << "1: Does EvenNumberOfOnesBinary accept 10? " << evenNumberOfOnesBinary.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  evenNumberOfOnesBinary.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does EvenNumberOfOnesBinary accept 01? " << evenNumberOfOnesBinary.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  evenNumberOfOnesBinary.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does EvenNumberOfOnesBinary accept 1011? " << evenNumberOfOnesBinary.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  evenNumberOfOnesBinary.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does EvenNumberOfOnesBinary accept 0000? " << evenNumberOfOnesBinary.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  evenNumberOfOnesBinary.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does EvenNumberOfOnesBinary accept 111111? "
            << evenNumberOfOnesBinary.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  evenNumberOfOnesBinary.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does EvenNumberOfOnesBinary accept the empty string? "
            << evenNumberOfOnesBinary.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  evenNumberOfOnesBinary.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does EvenNumberOfOnesBinary accept 1? " << evenNumberOfOnesBinary.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  evenNumberOfOnesBinary.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does EvenNumberOfOnesBinary accept 0? " << evenNumberOfOnesBinary.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenNumberOfOnesBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does EvenNumberOfOnesBinary accept 010? " << evenNumberOfOnesBinary.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenNumberOfOnesBinary.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does EvenNumberOfOnesBinary accept 00000? " << evenNumberOfOnesBinary.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  evenNumberOfOnesBinary.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does EvenNumberOfOnesBinary accept 00011? " << evenNumberOfOnesBinary.accepts(ZZZOO)
            << std::endl;
  std::cout << "Trace for 00011: ";
  evenNumberOfOnesBinary.trace(ZZZOO);
  std::cout << std::endl;
  std::cout << "12: Does EvenNumberOfOnesBinary accept 01010? " << evenNumberOfOnesBinary.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  evenNumberOfOnesBinary.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfZerosAndSingleOne DFA" << std::endl; // tests for EvenNumberOfZerosAndSingleOne DFA
  std::cout << "1: Does EvenNumberOfZerosAndSingleOne accept 10? " << evenNumberOfZerosAndSingleOne.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  evenNumberOfZerosAndSingleOne.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does EvenNumberOfZerosAndSingleOne accept 0101? " << evenNumberOfZerosAndSingleOne.accepts(ZOZO)
            << std::endl;
  std::cout << "Trace for 0101: ";
  evenNumberOfZerosAndSingleOne.trace(ZOZO);
  std::cout << std::endl;
  std::cout << "3: Does EvenNumberOfZerosAndSingleOne accept 0000001? " << evenNumberOfZerosAndSingleOne.accepts(ZZZZZZO)
            << std::endl;
  std::cout << "Trace for 0000001: ";
  evenNumberOfZerosAndSingleOne.trace(ZZZZZZO);
  std::cout << std::endl;
  std::cout << "4: Does EvenNumberOfZerosAndSingleOne accept 001 " << evenNumberOfZerosAndSingleOne.accepts(ZZO)
            << std::endl;
  std::cout << "Trace for 001: ";
  evenNumberOfZerosAndSingleOne.trace(ZZO);
  std::cout << std::endl;
  std::cout << "5: Does EvenNumberOfZerosAndSingleOne accept 111111? "
            << evenNumberOfZerosAndSingleOne.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  evenNumberOfZerosAndSingleOne.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does EvenNumberOfZerosAndSingleOne accept the empty string? "
            << evenNumberOfZerosAndSingleOne.accepts(epsi) << std::endl;
  std::cout << "Trace for empty string: ";
  evenNumberOfZerosAndSingleOne.trace(epsi);
  std::cout << std::endl;
  std::cout << "7: Does EvenNumberOfZerosAndSingleOne accept 1? " << evenNumberOfZerosAndSingleOne.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  evenNumberOfZerosAndSingleOne.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does EvenNumberOfZerosAndSingleOne accept 0? " << evenNumberOfZerosAndSingleOne.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenNumberOfZerosAndSingleOne.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does EvenNumberOfZerosAndSingleOne accept 010? " << evenNumberOfZerosAndSingleOne.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenNumberOfZerosAndSingleOne.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does EvenNumberOfZerosAndSingleOne accept 00001? " << evenNumberOfZerosAndSingleOne.accepts(ZZZZO)
            << std::endl;
  std::cout << "Trace for 00001: ";
  evenNumberOfZerosAndSingleOne.trace(ZZZZO);
  std::cout << std::endl;
  std::cout << "11: Does EvenNumberOfZerosAndSingleOne accept 111? " << evenNumberOfZerosAndSingleOne.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  evenNumberOfZerosAndSingleOne.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does EvenNumberOfZerosAndSingleOne accept 01010? " << evenNumberOfZerosAndSingleOne.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  evenNumberOfZerosAndSingleOne.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;
}

int main()
{
  makeAndTestDFAs();

  return 0;
}
