# Code taken from http://www.rosettacode.org/wiki/Cartesian_product_of_two_or_more_lists#Go
# Comments added by me

import itertools
 
def cp(lsts):
    return list(itertools.product(*lsts))  # Call built-in Cartesian product function
    # this is an example of polymorphism because as can be seen below, the code works with different types of list
 
if __name__ == '__main__':  # basically int main()
    from pprint import pprint as pp  # "pretty print"
 
    for lists in [[[1,2],[3,4]], [[3,4],[1,2]], [[], [1, 2]], [[1, 2], []],  # loop through this list of lists
                  ((1776, 1789),  (7, 12), (4, 14, 23), (0, 1)),
                  ((1, 2, 3), (30,), (500, 100)),
                  ((1, 2, 3), (), (500, 100)), (('a','b'),('c','d'))]:
        print(lists, '=>')  # print the lists being inputted into Cartesian product function
        pp(cp(lists), indent=2)  # print cartesian product of the lists