// Copyright Cameron Knopp 2019
// Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <iostream>
#include <string>
#include <list>
#include <iterator>
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
  DFA<State>(std::string name,
             bool (*Q)(State),
             std::list<myChar> alphabet,
             State q0,
             State (*transFunc)(State, myChar), bool (*F)(State)) : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc), F(F) {}

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
    while (temp->next()->charValue() != 'E')
    {
      std::cout << qi;
      qi = (*transFunc)(qi, temp->charObject());
      temp = temp->next();
    }
  }
  /*
  myString* acceptedString() {
    // need to check that accept states function is not empty by passing it possible

  }
*/
  bool acceptStates(myChar b)
  {
    return (*F)(b);
  } // used for testing
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

/*
// makes a DFA that only excepts a string of just one of the inputted Char
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
  DFA<myChar> evenLength("EvenLength",          // name
                         [](myChar a) -> bool { // state function
                           return ((a.getVal() == 'A') || (a.getVal() == 'B'));
                         },
                         std::list<myChar>{myChar('0'), myChar('1')}, // alphabet
                         myChar('A'),                                 // start state
                         [](myChar a, myChar b) -> myChar {           // transition function
                           if ((a.getVal() == 'A') &&
                               ((b.getVal() == '0') || (b.getVal() == '1')))
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

  DFA<myChar> acceptsNothing("AcceptsNothing",
                             [](myChar a) -> bool {
                               return false;
                             },
                             std::list<myChar>{},
                             myChar('A'),
                             [](myChar a, myChar b) -> myChar {
                               return a;
                             },
                             [](myChar a) -> bool {
                               return false;
                             });

  DFA<myChar> onlyAcceptsEmptyString("OnlyAcceptsEmptyString",
                                     [](myChar a) -> bool {
                                       return ((a.getVal() == 'A') || (a.getVal() == 'B') ||
                                               (a.getVal() == 'C'));
                                     },
                                     std::list<myChar>{'E'},
                                     myChar('A'),
                                     [](myChar a, myChar b) -> myChar {
                                       if (a.getVal() == 'A' && b.getVal() == 'E')
                                         return myChar('B');
                                       else
                                         return myChar('C');
                                     },
                                     [](myChar a) -> bool {
                                       return (a == myChar('E'));
                                     });
}

int main()
{
  makeAndTestDFAs();

  return 0;
}
