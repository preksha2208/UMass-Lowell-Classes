README file for Cameron Knopp OPL Assignment 1 (Assignment done solo)

Languages Used: Ada, Haskell, Go, Python, Prolog
Note: I took solutions from Rosetta code and StackOverflow for all languages except for Ada (which is the language I wrote the code for on my own).

Q1: What was easy?
A: I found Go and Python the easiest. Python has a built-in cartesian product function, so calculating the cartesian product
    entailed simply calling that function. I found that Go has some definite similarities to C++ and Python, so I was able to
    understand it's syntax fairly easily. 

Q2: What was hard?
A: I definitely found Ada to be the most difficult. Compared to Prolog, Ada has more syntactical similarities to what I am used to,
    but I got stuck on several compilation issues for a while with Ada and I had trouble finding
    resources online to remedy the problem because theres not as many resources for Ada as there are for a language such as Python. 
    These were the issues I encountered:
        - I didn't realize that I needed to put my function definition inside of my procedure
        - I didn't realize that my array started at 1 rather than 0, and so I spent a lot of time trying to figure out why I was getting an index out of bounds error
        - Had errors when trying to call Put() on an Integer, until I found out that you need to do Put(Integer'Image(x))
        - Kept trying to make the function work on arrays of varying sizes, but eventually realized I could only do it on arrays of a fixed size
    Overall though I didn't think that it was insanely difficult to complete, since the actual problem we had to solve was not very complex. 
    I also found it difficult to understand the syntax of Haskell, but I got the general idea of it by looking at some explanations of the fundamentals of the language.
    Some of the Prolog solutions were difficult to understand, but the one that I used was pretty intuitive since it used built-in functions to solve the problem.
    I don't like how in prolog you have to manually type in the inputs after running the code. I think that slows down the process a bit.

Q3: Are there noticeable differences in speed?
A: Nothing that I noticed.

Q4: What do you like/dislike?
A: I liked seeing the similarities between the languages and working in something other than my usual C++ or Python.
    

Q5: Did you use polymorphism in one or more of your programs? If YES, specify which one(s), 
    and comment on whether it was easy or hard to make use of polymorphism in the respective language(s)
A: Yes, I use it in prolog, which was easy because the language naturally works this way.
    Prolog makes use of polymorphism so that lists of any size can be passed into the product function
    that I created and still give a correct answer.