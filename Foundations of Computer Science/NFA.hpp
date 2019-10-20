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
  std::function<std::vector<State>(State, myChar)> transFunc; // transition function
  std::function<std::vector<State>(State)> epsilonTrans;        // returns what the state transitions to on epsilon
  std::function<bool(State &)> F;                               // accept states

  bool accepts(myString &inputString) // does NFA accept inputString?
  {
    std::vector<State> currentStates { this->q0 };  // keeps track of current states
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;
    int i = 1;

    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      std::cout << "iteration #" << i<< std::endl;
      i++;
      std::cout << "currentStates size before loop: " << currentStates.size() << std::endl;
      std::cout << "newStates size before loop: " << newStates.size() << std::endl;
      for (State x : currentStates)  // for each state  in current states
      {
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        std::cout << "tempVector  size after transFunc: " << tempVector.size() << std::endl;
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        std::cout << "newStates size after transFunc: " << newStates.size() << std::endl;
       /* tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        std::cout << "tempVector size after epsilon transFunc: " << tempVector.size() << std::endl;
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        std::cout << "newStates size after epsilon transFunc: " << newStates.size() << std::endl; */
      }

      currentStates.clear();
      currentStates = newStates;
      std::cout << "currentStates size after loop: " << currentStates.size() << std::endl;
      /*for (auto x : currentStates)
        std::cout << x << std::endl; */
      temp = temp->next(); // move to next character in the string
    }

    for (State x : currentStates)
    {
      if (F(x))  // check whether any of the set of current states is an accept state
        return true;  // if in accept state, then that means this NFA accepts the input string
    }
    return false;   // NFA does not accept the input string
  }

  void trace(myString &inputString) // prints trace of visited states
  {
    std::vector<State> currentStates { this->q0 }; 
    std::vector<State> tempVector;
    std::vector<State> newStates;
    myString *temp = &inputString;
    std::cout << "Trace: ";
    // step through NFA with the input string
    while (temp->isEmpty() != true)
    {
      newStates.clear(); // prepare to get new set of states from transFunc
      std::cout << ", ";
      for (auto x : currentStates)
      {
        std::cout << x << " ";  // print out each current state separated by a space
        tempVector = transFunc(x, temp->charObject()); // generate new sets of states from input char w/ each current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
        tempVector = epsilonTrans(x); // check whether there are epsilon transitions for current state
        newStates.insert(newStates.end(), tempVector.begin(), tempVector.end());
      }

      currentStates.clear();
      currentStates = newStates;
      temp = temp->next(); // move to next character in the string
    }
  }


private:
};

#endif