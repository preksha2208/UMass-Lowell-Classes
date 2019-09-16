// Copyright Cameron Knopp 2019
// This is my course project for Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

/*
TASK NOTES:
Task 1: I will be using a std::list to represent an alphabet and I will be using an object to represent a character
Task 2: A string will be represented by a linked list of string objects, with there being an empty string object, a string with 1 character
Task 3: Still need to complete. Unsure of what exactly it is I am supposed to do, so I am skipping forward to task 4.
  - I think my issue is likely that I just don't fully understand how the character and string classes are going to coursework
  - What are some examples of what the string objects will be holding? Will it be binary, will it be full words, will it be symbols?
Task 4: Unsure of what is meant by representing the set of states as a membership function
  - Confused as to how the data could be held by a function, or does he mean that
  - Need to think about what exactly the DFA class will be capable of doing
*/

#include <iostream>
#include <string>
#include <list>
#include <iterator>

class myChar {
 public:
  myChar(char c) {this->c = c;}
  myChar() {}
  myChar(const myChar& charB) {c = charB.c;}
  char c;
  void print() {std::cout << c;}
  myChar& operator=(const myChar& charB) {
  c = charB.c;
  return *this;
  }
};

class myString {
  virtual void print() = 0;
};

class oneString : public myString {
 public:
  oneString(myChar c, myString nString) {thisString = &c; nextString = &nString;}
  bool isEmpty() {return false;}

 private:
  myChar* thisString;      // CHARACTER OBJECT FOR THIS STRING ELEMENT
  myString* nextString;  // POINTER TO NEXT CHARACTER OF STRING
};

class emptyString : public myString {
 public:
  emptyString() {epsilon = new myChar('E');}
  bool isEmpty() {return true;}
  ~emptyString() {delete epsilon;}

 private:
  myChar* epsilon;
  myString* nextString;
};

/*
class DFA{
 public:
  DFA(std::string name)

 private:
  std::string name;
  // need function object to represent states
  std::list<myChar> alphabet;
  // need function object to represent transition function
  // need start state
  // need accept states



};
*/

/*
myString lexi(std::list<myString> alphabet){

}
*/

int main() {


  return 0;
}
