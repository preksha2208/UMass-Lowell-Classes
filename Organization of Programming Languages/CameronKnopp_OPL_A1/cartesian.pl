% Code taken from https://stackoverflow.com/questions/40244936/prolog-cartesian-product-calculator?rq=1

% call new with 4 parameters (so we can keep List1 in memory)
product(L1,L2,L3):- product(L1,L2,L3,L1).

% stop when both List1 and List2 are empty
product(_, [], [], _).

% first list is empty, recreate it and work it again with the next element of second list (and shorten memory)
product([], [_|T2], List3, L4):-
    product(L4, T2, List3, L4).

%go through first list and always first element of second list to our answer
product([H1|T1], [H2|T2], [[H1,H2]|T3], List4):-
    product(T1, [H2|T2], T3, List4).