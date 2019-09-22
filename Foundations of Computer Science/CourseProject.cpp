// Copyright Cameron Knopp 2019
// This is my course project for Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

#include <iostream>
#include <string>
#include <list>
#include <iterator>

class myChar {
 public:
  myChar(char c) {this->c = c;}  // initialize char variable
  myChar() {}
  myChar(const myChar& charB) {c = charB.c;}
  void print() {std::cout << c;}
  char getVal() {return c;};
  myChar& operator=(const myChar& charB) {
  c = charB.c;
  return *this;
  }

 private:
  char c;
};

class myString {
 public:
  myString(char c, myString* nextString) : nextString(nextString) {this->c = myChar(c);}
  myString() {}
  virtual void print() {c.print();}
  virtual myString* next() {return nextString;}
  virtual void test() {std::cout << "myString function" << std::endl;}
  virtual char charValue() {return c.getVal();}

 private:
  myChar c;
  myString* nextString;
};

class oneString : public myString {
 public:
  oneString(char c, myString* nextString) : nextString(nextString) {this->c = myChar(c);}
  oneString() {}
  myString* next() {return nextString;}
  bool isEmpty() {return false;}
  void print() {c.print();}
  void test() {std::cout << "oneString function" << std::endl;}
  char charValue() {return c.getVal();}

 private:
  myChar c;
  myString* nextString;
};

class emptyString : public myString {
 public:
  emptyString() {this->c = myChar('E');}          // need to change the E to an actual epsilon somehow
  bool isEmpty() {return true;}
  myString* next() {return NULL;}
  void print() {c.print();}
  void test() {std::cout << "emptyString function" << std::endl;}
  char charValue() {return c.getVal();}

 private:
  myChar c;
};

template <class State>
class DFA {
 public:
  DFA(std::string name, bool(*Q)(State), std::list<myChar> alphabet, State q0, State(*transFunc)(State, myChar), bool(*F)(State)) :
    name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc), F(F) {}

  void printAlphabet() {
    for (auto const& a : alphabet)
      a->print();
  }

  bool accepts(myString inputString) {      // returns whether the DFA accepts the input string
    State qi = this->q0;
    myString* temp = &inputString;

    while(temp->charValue() != 'E') {          // steps through DFA with each letter of inputted string
     (*transFunc)(qi, temp->charValue());
     temp = temp->next();
    }

    return (*F)(qi);            // checks whether arrived-at state is an accept state
  }

 private:
  std::string name;
  bool(*Q)(State);        // possible states for this DFA
  std::list<myChar> alphabet;
  State q0;                             // start state
  State(*transFunc)(State, myChar);        // DFA transition function
  bool(*F)(State);                        // accept states
};


/*
myString lexi(std::list<myString> alphabet){

}
*/

int main() {

  return 0;
}
