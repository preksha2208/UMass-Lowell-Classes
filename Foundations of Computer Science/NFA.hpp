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
    this->transFunc = [=](State a, myChar b) -> std::vector<State> {
      return std::vector<State>{inputDFA.transFunc(a, b)};
    };
    this->epsilonTrans = [](State) -> std::vector<State> { return std::vector<State>{}; }; // epsilon transitions don't exist in DFAs
    this->F = inputDFA.F;
  }

  std::string name;
  std::function<bool(State &)> Q; // list of possible states for this dfa
  std::vector<myChar> alphabet;
  State q0;                                                     // start state
  std::function<std::vector<State>(State, myChar &)> transFunc; // transition function
  std::function<std::vector<State>(State)> epsilonTrans;        // returns what the state transitions to on epsilon
  std::function<bool(State &)> F;                               // accept states

  bool accepts(myString &inputString) // does NFA accept inputString?
  {
    State qi = this->q0;
    std::vector<State> currentStates;  // keeps track of current states
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;

    if (temp->isEmpty() != true) // get first set of states from first transition
    {
      newStates = transFunc(qi, temp->charObject()); // newStates is the set of states obtained from this transition
      currentStates.insert(currentStates.end(), newStates.begin(), newStates.end());
    }
    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc

      for (auto x : currentStates)
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }
      currentStates.clear();
      currentStates = newStates;

      temp = temp->next(); // move to next character in the string
    }

    for (auto x : currentStates)
    {
      if (F(x))  // check whether any of the set of current states is an accept state
        return true;  // return true if an accept state has been reached
    }
    return false;   // otherwise return false. The NFA does not accept this input string
  }

private:
};

#endif