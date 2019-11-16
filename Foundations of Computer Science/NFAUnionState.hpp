#ifndef NFAUNIONSTATE_HPP
#define NFAUNIONSTATE_HPP

template <class X, class Y>
class NFAUnionState
{
public:
    bool isStartState;
    bool isAcceptState;
    bool isFromX;
    bool isFromY;
    X fromX;
    Y fromY;

    NFAUnionState<X, Y>(int i)
    {
        if (i == 0) // create start state object
        {
            isStartState = 1;
            isAcceptState = 0;
        }
        if (i == 1) // create accept state object
        {
            isStartState = 0;
            isAcceptState = 1;
        }
        isFromX = 0;
        isFromY = 0;
    }
    NFAUnionState<X, Y>(X fromX, int i) // create state object for NFA of type X
    {
        // need to initialize X variable
        this->fromX = fromX;
        isStartState = 0;
        isAcceptState = 0;
        isFromX = 1;
        isFromY = 0;
    }
    NFAUnionState<X, Y>(int i, Y fromY) // create state object for NFA of type Y
    {
        // need to initialize Y variable
        this->fromY = fromY;
        isStartState = 0;
        isAcceptState = 0;
        isFromX = 0;
        isFromY = 1;
    }
};

#endif