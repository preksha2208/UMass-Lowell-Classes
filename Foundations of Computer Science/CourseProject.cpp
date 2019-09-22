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

 private:
  myChar c;
};

template <class State>
class DFA{
 public:
  DFA(std::string name, bool(*Q)(State), std::list<myChar> alphabet, State q0, State(*transitionFunc)(State,Character), bool(*F)(State)) :
    name(name),

 private:
  std::string name;
  bool(*Q)(State);        // possible states for this DFA
  std::list<myChar> alphabet;
  State q0;                             // start state
  State(*transitionFunc)(State, myChar);
  bool(*F)(State);                        // accept states
};


/*
myString lexi(std::list<myString> alphabet){

}
*/

int main() {

  return 0;
}
