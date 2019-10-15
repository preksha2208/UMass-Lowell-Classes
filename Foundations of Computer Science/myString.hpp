#ifndef MYSTRING_HPP
#define MYSTRING_HPP
#include "myChar.hpp"

// represents a string as a linked list of myChar nodes followed by an emptyString node
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
#endif