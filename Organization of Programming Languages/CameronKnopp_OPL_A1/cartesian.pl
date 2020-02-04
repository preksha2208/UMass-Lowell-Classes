% Code taken from https://stackoverflow.com/a/49503900

:- use_module(library(chr)).

% definition of cartesian product function
% uses findall predicate and member predicates to find all (X,Y) where X is a member of set A and Y is a member of set B (aka cartesian product)
product(A,B,C) :-
    findall([X,Y],(member(X,A),member(Y,B)),C).

% example input: product([1,2,3], [a,b], X).
% example input: product([a], [a,b], X).
% example input: product([1,2,3], [1,2,3], X).
% example input: product([], [], X).