// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

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

class myString // class to represent strings; the strings are implemented as a
               // linked list, with each node being a letter
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

class oneString : public myString // node of string that contains a myChar
                                  // object (i.e. a letter)
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
  std::string getName() { return name; }
  void printName() { std::cout << name << std::endl; }
  std::list<myChar> getAlphabet() { return alphabet; }

  bool accepts(myString &inputString) // returns true/false to indicate whether
                                      // this DFA accepts the inputString
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

  void trace(myString &inputString) // returns the trace for the inputString on
                                    // this DFA
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
    return DFA<State>("complement of " + inputDFA.name, Q, alphabet, q0,
                      transFunc, [=](State a) -> bool {
                        if (F(a) == true)
                          return false;
                        return true;
                      });
  }

  DFA<State> UnionDFA(DFA<State> dfa2)
  {
    std::list<myChar> a = alphabet;
    std::list<myChar> b = dfa2.getAlphabet();
    a.insert(a.end(), b.begin(), b.end()); // combine the alphabets of both DFAs

    return DFA<State>(
        "Union of " + name + " and " + dfa2.getName(),
        [](State a) -> bool { // function for possible states

        },
        a,                      // alphabet
        myChar('A'),            // start state, need to figure this one out
        [](State a, myChar b) { // transition function; not correct as is
        },
        [=](State a) { // accept states
          return ((F(a)) || (dfa2.acceptStates(a)));
        });
  }

private:
  // auto acceptedString(myString* currentNode, std::unordered_map<int,
  // myChar>);
  std::string name;
  std::function<bool(State)> Q; // list of possible states for this dfa
  std::list<myChar> alphabet;
  State q0;                                      // start state
  std::function<State(State, myChar)> transFunc; // DFA transition function
  std::function<bool(State)> F;                  // accept states
};

//myString lexi(std::list<myString> alphabet) {}

// makes a DFA that only accepts a string of just one of the inputted Char
DFA<myChar> oneCharDFA(myChar inputChar)
{
  return DFA<myChar>(
      "onlyAccepts" + std::string(1, inputChar.getVal()),
      [=](myChar a) -> bool { return (a == myChar('A') || a == myChar('B')); },
      std::list<myChar>{inputChar}, myChar('A'),
      [&](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && (b.getVal() == inputChar.getVal()))
          return myChar('B');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('B')); });
}

template <class State>
void DFAtester(DFA<State> &a, std::vector<myString> &inputStrings, std::vector<bool> &expected)
{
  for (int i = 0; i < inputStrings.size(); i++)
  {
    if (a.accepts(inputStrings[i]) == expected[i])
      std::cout << "Test Passed!";
    else
      std::cout << "Test Failed.";

    std::cout << " --- Trace: ";
    a.trace(inputStrings[i]);
    std::cout << std::endl;
  }
}

void makeAndTestDFAs() // creates 12 DFAs, runs 12 tests on each DFA, and prints
                       // the results to the console
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
  DFA<myChar>
      evenBinaryNumber( // returns whether the inputted binary number is even
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

  DFA<myChar>
      oddBinaryNumber( // returns whether the inputted binary number is odd
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

  DFA<myChar>
      containsLineComment( // returns whether the input contains a double slash
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

  DFA<myChar>
      threeConsecutiveZerosBinary( // returns whether the inputted binary number
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

  DFA<myChar>
      threeConsecutiveOnesBinary( // returns whether the inputted binary number
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

  DFA<myChar>
      evenNumberOfZerosAndSingleOne( // returns whether the inputted number has
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

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenLengthBinary DFA: " << std::endl; // Tests for EvenLengthBinaryDFA
  std::vector<myString> evenLengthStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedEvenLength{true, true, true, true, true, true, false, false, false, false, false, false};
  //DFAtester(evenLengthBinary, evenLengthStrings, expectedEvenLength);
  

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing AcceptsNothing DFA" << std::endl; // tests for AcceptsNothing DFA
  std::vector<myString> acceptsNothingStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedAcceptsNothing{false, false, false, false, false, false, false, false, false, false, false, false};


  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OnlyAcceptsEmptyString DFA" << std::endl; // tests for onlyAcceptsEmptyString DFA
  std::vector<myString> onlyAcceptsEmptyStringStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedOnlyAcceptsEmptyString{false, false, false, false, false, true, false, false, false, false, false, false};


  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenBinaryNumber DFA"
            << std::endl; // tests for evenBinaryNumber DFA
  std::vector<myString> evenBinaryNumberStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedEvenBinaryNumber{true, false, false, true, false, false, false, true, true, true, false, true};

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddBinaryNumber DFA" << std::endl; // tests for OddBinaryNumber DF
  std::vector<myString> oddBinaryNumberStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedOddBinaryNumber{false, true, true, false, true, true, true, false, false, false, true, false};

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsCAM DFA"
            << std::endl; // tests for containsCAM DFA
  std::vector<myString> ContainsCAMStrings{OZ, ZO, OZOO, ZZZZ, MAC, epsi, CAMOO, OOCAM, OCAMO, CACAMM, CAMCAM, CAMERA};
  std::vector<bool> expectedContainsCAM{false, false, false, false, false, false, true, true, true, true, true, true};
  
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ContainsLineComment DFA" << std::endl; // tests for containsLineComment DFA
  std::vector<myString> containsLineCommentStrings{ comment1, comment2, comment3, comment4, comment5, epsi, comment6, comment7, OCAMO, CACAMM, CAMCAM, CAMERA};
  std::vector<bool> expectedContainsLineComment{true, true, false, true, true, false, true, false, false, false, false, false};
  
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveZerosBinary DFA" << std::endl; // tests for threeConsecutiveZerosBinary DFA
  std::vector<myString> threeConsecutiveZerosStrings{ZZZZ, ZZZOO, ZOZZZ, ZZZZZ, OOZZZ, epsi, OZZZO, Z, MAC, OZ, OOO, ZOZOZ};
  std::vector<bool> expectedThreeConsecutiveZeros {true, true, true, true, true, false, true, false, false, false, false, false};
  
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing ThreeConsecutiveOnesBinary DFA"
            << std::endl; // tests for threeConsecutiveOnesBinary DFA
  std::vector<myString> threeConsecutiveOnesStrings{ZZZZ, ZZZOO, ZOZZZ, ZZZZZ, OOZZZ, epsi, ZZZZZ, OOOZZZ, ZOOOZZ, ZZOOOZ, OOO, ZZZOOO};
  std::vector<bool> expectedConsecutiveOnes{false, false, false, false, false, false, false, true, true, true, true, true};
  
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing OddNumberOfOnesBinary DFA"
            << std::endl; // tests for oddNumberOfOnesBinary DFA
  std::vector<myString> oddNumberOfOnesStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedOddNumberOfOnes{true, true, true, false, false, false, true, false, true, false, true, false};

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfOnesBinary DFA"
            << std::endl; // tests for EvenNumberOfOnesBinary DFA
  std::vector<myString> evenNumberOfOnesStrings{OZ, ZO, OZOO, ZZZZ, OOOOOO, epsi, O, Z, ZOZ, ZZZZZ, OOO, ZOZOZ};
  std::vector<bool> expectedEvenNumberOfOnes{false, false, false, true, true, true, false, true, false, true, false, true};

  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenNumberOfZerosAndSingleOne DFA"
            << std::endl; // tests for EvenNumberOfZerosAndSingleOne DFA
  std::vector<myString> evenNumberOfZerosAndSingleOneStrings{OZ, ZOZO, ZZZZZZO, ZZO, OOOOOO, epsi, O, Z, ZOZ,ZZZZO, OOO, ZOZOZ};
  std::vector<bool> expectedEvenNumberZerosAndSingleOne{false, false, true, true, false, false, false, false, false, true, false, false};
}

int main()
{
  makeAndTestDFAs();
  return 0;
}
