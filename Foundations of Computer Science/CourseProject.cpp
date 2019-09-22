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
  emptyString() {this->c = myChar('E');}
  bool isEmpty() {return true;}
  myString* next() {return NULL;}
  void print() {c.print();}
  void test() {std::cout << "emptyString function" << std::endl;}

 private:
  myChar c;
};

/*
class DFA{
 public:
  DFA(std::string name)

 private:
  std::string name;
  // need function object to represent states
  std::list<myChar> alphabet;
  // need function object to represent transition function
  // need start state
  // need accept states
};
*/

/*
myString lexi(std::list<myString> alphabet){

}
*/

int main() {

  return 0;
}
