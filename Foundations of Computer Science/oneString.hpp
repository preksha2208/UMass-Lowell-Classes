#ifndef ONESTRING_HPP
#define ONESTRING_HPP
#include "myString.hpp"

// represents a letter in a string linked list
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

#endif