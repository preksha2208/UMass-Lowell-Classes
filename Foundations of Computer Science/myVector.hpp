#ifndef MYVECTOR_HPP
#define MYVECTOR_HPP

#include <initializer_list>
#include <iostream>
#include <vector>

// custom vector class with an overloaded << operator. Needed for accepts function and traces
template <class State>
class myVector
{
public:
  typedef typename std::vector<State>::iterator iter;
  std::vector<State> v;

  myVector<State>() // default constructor
  {
    v = std::vector<State>();
  }
  myVector<State>(std::initializer_list<State> init)  // init list constructor i.e. {1,2,3}
  {
    v = std::vector<State>(init);
  }
  myVector<State>(const myVector &v2)  // copy constructor
  {
    v = v2.v;
  } 

  int size()
  {
    return v.size();
  }

  void clear()
  {
    v.clear();
  }

  iter begin()
  {
    return v.begin();
  }
  iter end()
  {
    return v.end();
  }
  iter insert(const iter aBegin, const iter bBegin, const iter bEnd)
  {
    return v.insert(aBegin, bBegin, bEnd);
  }

  friend bool operator==(const myVector &a, const myVector &b)
  {
    return a == b; // use std::vector overloaded == operator
  }

  const State &operator[](int index)
  {
    return v[index];
  }

  friend std::ostream &operator<<(std::ostream &os, const myVector &p)
  {
    os << "|"; // signifies start of vector when printing
    for (auto x : p.v)
      os << x;
    os << "|"; // signfiies end of vector when printing
    return os;
  }
};

#endif