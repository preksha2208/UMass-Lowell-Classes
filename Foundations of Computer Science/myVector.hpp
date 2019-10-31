#ifndef MYVECTOR_HPP
#define MYVECTOR_HPP

#include <initializer_list>
#include <iostream>
#include <vector>


// custom vector class with an overloaded << operator. Needed for accepts function and traces
template <class State> class myVector {
 public:
  typedef typename std::vector<State>::iterator iter;  
  std::vector<State> v;

  myVector<State>(std::initializer_list<State> init) 
  {
    v = std::vector<State>(init);
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
    return a == b;  // use std::vector overloaded == operator
  }

  friend std::ostream &operator<<(std::ostream &os, const myVector &p) 
  {
    os << "| ";
    for (auto x : p.v)
        os << x;
    os << " |";
    return os;
  }


};

#endif