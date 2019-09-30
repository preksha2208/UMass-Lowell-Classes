// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

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

class myString
{
public:
  myString(char c, myString *nextString) : nextString(nextString)
  {
    this->c = myChar(c);
  }
  myString() {}
  virtual void print() { c.print(); }
  virtual myString *next() { return nextString; }
  virtual void test() { std::cout << "myString function" << std::endl; }
  virtual char charValue() { return c.getVal(); }
  virtual myChar charObject() { return c; }

private:
  myChar c;
  myString *nextString;
};

class oneString : public myString
{
public:
  oneString(char c, myString *nextString) : nextString(nextString)
  {
    this->c = myChar(c);
  }
  oneString() {}
  myString *next() { return nextString; }
  bool isEmpty() { return false; }
  void print() { c.print(); }
  void test() { std::cout << "oneString function" << std::endl; }
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
  void print() { c.print(); }
  void test() { std::cout << "emptyString function" << std::endl; }
  char charValue() { return c.getVal(); }
  myChar charObject() { return c; }

private:
  myChar c;
};

template <class State>
class DFA
{
public:
  DFA<State>(std::string name, bool (*Q)(State), std::list<myChar> alphabet,
             State q0, State (*transFunc)(State, myChar), bool (*F)(State))
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        F(F) {}

  void printName() { std::cout << name << std::endl; }
  void printAlphabet()
  {
    for (auto a : alphabet)
      a.print();
  }

  bool accepts(myString &inputString)
  { // does DFA accept inputString?
    State qi = this->q0;
    myString *temp = &inputString;

    // step through DFA with the input string
    while (temp->next()->charValue() != 'E')
    {
      qi = (*transFunc)(qi, temp->charObject());
      temp = temp->next();
    }

    return (*F)(qi); // checks whether arrived-at state is an accept state
  }

  void trace(myString &inputString)
  { // does DFA accept inputString?
    State qi = this->q0;
    myString *temp = &inputString;

    // step through DFA with the input string
    while (temp->charValue() != 'E')
    {
      std::cout << qi;
      qi = (*transFunc)(qi, temp->charObject());
      temp = temp->next();
    }
  }
  /*
  myString* acceptedString() {
    // need to check that accept states function is not empty by passing it
  possible

  }
*/
  bool acceptStates(myChar b) { return (*F)(b); } // used for testing
  State transitionFunction(State a, myChar b)
  {
    return (*transFunc)(a, b);
  } // used for testing

private:
  std::string name;
  bool (*Q)(State); // list of possible states for this dfa
  std::list<myChar> alphabet;
  State q0;                          // start state
  State (*transFunc)(State, myChar); // DFA transition function
  bool (*F)(State);                  // accept states
};

/*
myString lexi(std::list<myString> alphabet){

}
*/

// makes a DFA that only excepts a string of just one of the inputted Char
/*
DFA<myChar> oneCharDFA(myChar inputChar) {
  auto tFunc = [=](myChar a, myChar b) -> myChar {
    if (a.getVal() == 'A' && (b.getVal() == inputChar.getVal()))
      return myChar('B');
    else
      return myChar('C');
  };

  return DFA<myChar>("onlyAccepts" + std::string(1, inputChar.getVal()),
         [](myChar a) -> bool {
          return (a == myChar('A') || a == myChar('B'));
         },
         std::list<myChar>{inputChar},
         myChar('A'),
         &tFunc,
         [](myChar a) -> bool {
         return (a == myChar('B'));
         }
);
}
*/

void makeAndTestDFAs()
{

  DFA<myChar> evenLength(
      "EvenLength",          // name
      [](myChar a) -> bool { // state function
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
        if (a.getVal() == 'B')
          return true;
        else
          return false;
      });

  DFA<myChar> acceptsNothing(
      "AcceptsNothing", [](myChar a) -> bool { return false; },
      std::list<myChar>{}, myChar('A'),
      [](myChar a, myChar b) -> myChar { return a; },
      [](myChar a) -> bool { return false; });

  DFA<myChar> onlyAcceptsEmptyString(
      "OnlyAcceptsEmptyString",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C'));
      },
      std::list<myChar>{myChar('E')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == 'E')
          return myChar('B');
        else
          return myChar('C');
      },
      [](myChar a) -> bool { return (a == myChar('E')); });
  DFA<myChar> evenBinaryNumber(
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

  DFA<myChar> oddBinaryNumber(
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

  DFA<myChar> containsCAM(
      "ContainsCAM",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C') || (a.getVal() == 'D'));
      },
      std::list<myChar>{myChar('C'), myChar('A'), myChar('M')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == 'C')
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

  DFA<myChar> containsLineComment(
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

  DFA<myChar> threeConsecutiveZerosBinary(
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
        else if (a.getVal() == 'D' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'D' && b.getVal() == '1')
          return myChar('D');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('D')); });

  DFA<myChar> threeConsecutiveOnesBinary(
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

  DFA<myChar> oddNumberOfOnesBinary(
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
  DFA<myChar> evenNumberOfOnesBinary(
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

  DFA<myChar> evenNumberOfZerosAndSingleOne(
      "EvenNumberOfZerosAndSingleOne",
      [](myChar a) -> bool {
        return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                (a.getVal() == 'C'));
      },
      std::list<myChar>{myChar('1'), myChar('0')}, myChar('A'),
      [](myChar a, myChar b) -> myChar {
        if (a.getVal() == 'A' && b.getVal() == '0')
          return myChar('B');
        else if (a.getVal() == 'B' && b.getVal() == '0')
          return myChar('A');
        else if (a.getVal() == 'A' && b.getVal() == '1')
          return myChar('C');
        else
          return myChar('A');
      },
      [](myChar a) -> bool { return (a == myChar('C')); });

  oneString OZ = oneString('1', new oneString('0', new emptyString));
  oneString ZO = oneString('0', new oneString('1', new emptyString));
  oneString OZOO = oneString(
      '1', new oneString(
               '0', new oneString('1', new oneString('1', new emptyString))));
  oneString ZZZZ = oneString(
      '0', new oneString(
               '0', new oneString('0', new oneString('0', new emptyString))));
  oneString OOOOOO = oneString(
      '1',
      new oneString(
          '1',
          new oneString(
              '1', new oneString(
                       '1', new oneString(
                                '1', new oneString('1', new emptyString))))));
  oneString mtString = oneString('E', new emptyString);
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

  std::cout << std::boolalpha;
  std::cout << "-----------------------" << std::endl;
  std::cout << "Testing EvenLength DFA" << std::endl;
  std::cout << "1: Does EvenLength accept 10? " << evenLength.accepts(OZ)
            << std::endl;
  std::cout << "Trace for 10: ";
  evenLength.trace(OZ);
  std::cout << std::endl;
  std::cout << "2: Does EvenLength accept 01? " << evenLength.accepts(ZO)
            << std::endl;
  std::cout << "Trace for 01: ";
  evenLength.trace(ZO);
  std::cout << std::endl;
  std::cout << "3: Does EvenLength accept 1011? " << evenLength.accepts(OZOO)
            << std::endl;
  std::cout << "Trace for 1011: ";
  evenLength.trace(OZOO);
  std::cout << std::endl;
  std::cout << "4: Does EvenLength accept 0000? " << evenLength.accepts(ZZZZ)
            << std::endl;
  std::cout << "Trace for 0000: ";
  evenLength.trace(ZZZZ);
  std::cout << std::endl;
  std::cout << "5: Does EvenLength accept 111111? "
            << evenLength.accepts(OOOOOO) << std::endl;
  std::cout << "Trace for 111111: ";
  evenLength.trace(OOOOOO);
  std::cout << std::endl;
  std::cout << "6: Does EvenLength accept the empty string? "
            << evenLength.accepts(mtString) << std::endl;
  std::cout << "Trace for empty string: ";
  evenLength.trace(mtString);
  std::cout << std::endl;
  std::cout << "7: Does EvenLength accept 1? " << evenLength.accepts(O)
            << std::endl;
  std::cout << "Trace for 1: ";
  evenLength.trace(O);
  std::cout << std::endl;
  std::cout << "8: Does EvenLength accept 0? " << evenLength.accepts(Z)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenLength.trace(Z);
  std::cout << std::endl;
  std::cout << "9: Does EvenLength accept 010? " << evenLength.accepts(ZOZ)
            << std::endl;
  std::cout << "Trace for 0: ";
  evenLength.trace(Z);
  std::cout << std::endl;
  std::cout << "10:Does EvenLength accept 00000? " << evenLength.accepts(ZZZZZ)
            << std::endl;
  std::cout << "Trace for 00000: ";
  evenLength.trace(ZZZZZ);
  std::cout << std::endl;
  std::cout << "11: Does EvenLength accept 111? " << evenLength.accepts(OOO)
            << std::endl;
  std::cout << "Trace for 111: ";
  evenLength.trace(OOO);
  std::cout << std::endl;
  std::cout << "12: Does EvenLength accept 01010? " << evenLength.accepts(ZOZOZ)
            << std::endl;
  std::cout << "Trace for 01010: ";
  evenLength.trace(ZOZOZ);
  std::cout << std::endl;
  std::cout << "-----------------------" << std::endl;
}

int main()
{
  makeAndTestDFAs();

  return 0;
}
