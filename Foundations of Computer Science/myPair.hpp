#ifndef MYPAIR_HPP
#define MYPAIR_HPP

template<class State1, class State2>
class myPair
{
public:
  myPair<State1, State2>(State1 data1, State2 data2) : first(data1), second(data2) {}

  State1 first;
  State2 second;
  friend myPair<State1, State2> make_MyPair(State1 data1, State2 data2)
  {
    return myPair<State1, State2>(data1, data2);
  }
  friend std::ostream &operator<<(std::ostream &os, const myPair &p)
  {
    os << p.first << p.second;
    return os;
  }
  friend bool operator==(const myPair &a, const myPair &b)
  {
    return (a.first == b.first && a.second == b.second);
  }
};
#endif