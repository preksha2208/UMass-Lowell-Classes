#ifndef EMPTYSTRING_HPP
#define EMPTYSTRING_HPP
#include "myString.hpp"

// epsilon
class emptyString : public myString
{
public:
  emptyString() { this->c = myChar('E'); }  // can probably delete this
  bool isEmpty() { return true; }   // this is how it is identified
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

#endif