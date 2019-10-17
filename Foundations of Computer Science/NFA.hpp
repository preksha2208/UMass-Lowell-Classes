#ifndef NFA_HPP
#define NFA_HPP

template <class State>
class NFA
{
public:
  NFA<State>(std::string name, std::function<bool(State)> Q,
             std::vector<myChar> alphabet, State q0,
             std::function<std::vector<State>(State, myChar)> transFunc,
             std::function<std::vector<State>(State)> epsilonTrans, std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        epsilonTrans(epsilonTrans), F(F) {}
  NFA<State>(const DFA<State> &inputDFA) // converts DFA to NFA
  {
    this->name = inputDFA.name;
    this->Q = inputDFA.Q;
    this->alphabet = inputDFA.alphabet;
    this->q0 = inputDFA.q0;
    // this->transFunc = inputDFA.transFunc;  // need to fix this
    this->F = inputDFA.F;
  }

  std::string name;
  std::function<bool(State &)> Q; // list of possible states for this dfa
  std::vector<myChar> alphabet;
  State q0;                                                       // start state
  std::function<std::vector<State>(State &, myChar &)> transFunc; // transition function
  std::function<std::vector<State>(State &)> epsilonTrans;        // returns what the state transitions to on epsilon
  std::function<bool(State &)> F;                                 // accept states

  bool accepts(myString &inputString) // does NFA accept inputString?
  {
    State qi = this->q0;
    myString *temp = &inputString;

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      qi = transFunc(qi, temp->charObject());
      temp = temp->next();
    }

    return F(qi); // checks whether arrived-at state is an accept state
  }

private:
};

#endif