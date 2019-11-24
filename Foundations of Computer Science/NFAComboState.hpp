#ifndef NFACOMBOSTATE_HPP
#define NFACOMBOSTATE_HPP

template <class X, class Y>
class NFAComboState
{
public:
    bool isStartState;
    bool isAcceptState;
    bool isFromX;
    bool isFromY;
    X fromX;
    Y fromY;

    NFAComboState<X, Y>(int i)
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
    NFAComboState<X, Y>(X fromX, int i) // create state object for NFA of type X
    {
        // need to initialize X variable
        this->fromX = fromX;
        isStartState = 0;
        isAcceptState = 0;
        isFromX = 1;
        isFromY = 0;
    }
    NFAComboState<X, Y>(int i, Y fromY) // create state object for NFA of type Y
    {
        // need to initialize Y variable
        this->fromY = fromY;
        isStartState = 0;
        isAcceptState = 0;
        isFromX = 0;
        isFromY = 1;
    }
    bool operator<(const NFAComboState<X, Y> &rhs) const  // used to sort vector of NFAComboState objects
    {
        if (this->isStartState && (rhs.isFromX || rhs.isFromY || rhs.isAcceptState))
            return true;
        else if ((this->isFromX || this->isFromY || this->isAcceptState) && rhs.isStartState)
            return false;
        else if (this->isFromX && rhs.isFromX)
            return this->fromX < rhs.fromX;
        else if (this->isFromY && rhs.isFromY)
            return this->fromY < rhs.fromY;
        else if (this->isFromX && rhs.isFromY)
            return true;
        else if (this->isFromY && rhs.isFromX)
            return false;
        else if (this->isAcceptState && (rhs.isFromX || rhs.isFromY))
            return false;
        else if ((this->isFromX || this->isFromY) && rhs.isAcceptState)
            return true;
        else
            return false;
    }

    bool operator==(const NFAComboState<X, Y> &rhs) const // used to sort vector of NFAComboState objects
    {
        if (this->isStartState && (rhs.isFromX || rhs.isFromY || rhs.isAcceptState))
            return false;
        else if (this->isStartState && rhs.isStartState)
            return true;
        else if (this->isAcceptState && rhs.isAcceptState)
            return true;
        else if ((this->isFromX || this->isFromY || this->isAcceptState) && rhs.isStartState)
            return false;
        else if (this->isFromX && rhs.isFromX)
            return this->fromX == rhs.fromX;
        else if (this->isFromY && rhs.isFromY)
            return this->fromY == rhs.fromY;
        else if (this->isFromX && rhs.isFromY)
            return false;
        else if (this->isFromY && rhs.isFromX)
            return false;
        else if (this->isAcceptState && (rhs.isFromX || rhs.isFromY))
            return false;
        else if ((this->isFromX || this->isFromY) && rhs.isAcceptState)
            return false;
        else
            return false;
    }

    friend std::ostream &operator<<(std::ostream &os, const NFAComboState &n)
    {
        if (n.isStartState)
            os << "<";
        else if (n.isAcceptState)
            os << ">";
        else if (n.isFromX)
            os << n.fromX;
        else
            os << n.fromY;

        return os;
    }
};

#endif