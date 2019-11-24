#ifndef MYCHAR_HPP
#define MYCHAR_HPP
#include <iostream>

// custom char class
class myChar
{
public:
  explicit myChar(char c) { this->c = c; } // initialize char variable
  myChar() {}
  myChar(const myChar &charB) { c = charB.c; }
  void print() { std::cout << c; }
  char getVal() const { return c; }
  myChar &operator=(const myChar &charB)
  {
    c = charB.c;
    return *this;
  }
  bool operator<(const myChar &rhs) const
  {
    return this->c < rhs.getVal();
  }
  bool operator==(const myChar &b) const
  {
    return (this->c == b.getVal());
  }
  friend bool operator!=(const myChar &a, const myChar &b)
  {
    return (a.c != b.c);
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