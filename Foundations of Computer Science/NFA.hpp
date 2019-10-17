#ifndef NFA_HPP
#define NFA_HPP

template <class State>
class NFA
{
public:
  NFA<State>(std::string name, std::function<bool(State)> Q,
             std::vector<myChar> alphabet, State q0,
             std::function<State(State, myChar)> transFunc,
             std::function<bool(State)> F)
      : name(name), Q(Q), alphabet(alphabet), q0(q0), transFunc(transFunc),
        F(F) {}
  NFA<State>(const DFA<State> &inputDFA)  // converts DFA to NFA
  {
    this->name = inputDFA.name;
    this->Q = inputDFA.Q;
    this->alphabet = inputDFA.alphabet;
    this->q0 = inputDFA.q0;
    this->transFunc = inputDFA.transFunc;
    this->F = inputDFA.F;
  }

  std::string name;
  std::function<bool(State)> Q; // list of possible states for this dfa
  std::vector<myChar> alphabet;
  State q0;                                      // start state
  std::function<State(State, myChar)> transFunc; // transition function
  std::function<State(State)> epsilonTrans;  // returns what the state transitions to on epsilon
  std::function<bool(State)> F;                  // accept states
};

#endif