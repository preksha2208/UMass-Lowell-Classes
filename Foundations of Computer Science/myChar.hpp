#ifndef MYCHAR_HPP
#define MYCHAR_HPP
#include <iostream>

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
#endif