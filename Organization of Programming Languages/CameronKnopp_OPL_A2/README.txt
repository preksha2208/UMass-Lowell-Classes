Assignment 2 for Organization of Programming Languages with Professor Charles Wilkes MWF 1-1:50pm

Name: Cameron Knopp (I worked solo on this assignment)

TASK 1: TRANSLATE THE PROVIDED C CODE TO C++
- In order to convert the initial C code to C++ I pretty much just had to change the printf to std::cout and getchar() to std::cin.get() 

TASK 2: EXTEND THE CALCULATOR LANGUAGE
- added in the if and do/check statements as described

TASK 3: IMPLEMENT EXCEPTION-BASED SYNTAX ERROR RECOVERY
- Error recovery implemented using try/catch blocks and follow sets of the provided grammar
- Exception handling in statements, relations, and expressions

TASK 4: OUTPUT AN ABSTRACT SYNTAX TREE
- Input is taken as specified in the Makefile, and an AST is outputted without indentation
- Return type of functions in parse changed from void to std::string so that the final output can be more easily configured

