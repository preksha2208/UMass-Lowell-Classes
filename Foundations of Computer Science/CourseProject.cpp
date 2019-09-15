// Copyright Cameron Knopp 2019
// This is my course project for Fall 2019 Foundations of Computer Science with Professor Jay McCarthy

/*
TASK NOTES:
Task 1: I will be using a std::list to represent an alphabet and I will be using an object to represent a character
Task 2: I will be using an abstract class to represent a string and then I will make child-classes from that class which will allow for the building up of strings out of different elements
Task 3: Still need to complete. Unsure of what exactly it is I am supposed to do, so I am skipping forward to task 4.
  - I think my issue is likely that I just don't fully understand how the character and string classes are going to coursework
  - What are some examples of what the string objects will be holding? Will it be binary, will it be full words, will it be symbols?
Task 4: Unsure of what is meant by representing the set of states as a membership function
  - Confused as to how the data could be held by a function, or does he mean that
  - Need to think about what exactly the DFA class will be capable of doing
*/

#include <iostream>
#include <string>

class myChar {
 public:
  myChar(char c) {this.c = c;}
  char c;
}

class myString {
 public:
  myString() {}
  std::list<myChar> thisString;
  

 private:

}


class DFA {
 public:


 private:

}

myString lexi(std::list<myString> alphabet) {

}
