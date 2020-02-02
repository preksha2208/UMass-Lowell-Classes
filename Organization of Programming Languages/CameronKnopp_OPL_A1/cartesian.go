// Code taken from http://www.rosettacode.org/wiki/Cartesian_product_of_two_or_more_lists#Go
// Comments added be me
package main

import "fmt"

type pair [2]int

func cart2(a, b []int) []pair { // func takes in two integer arrays and returns an array of pairs
	p := make([]pair, len(a)*len(b)) // creates the array of pairs to hold the final result
	i := 0
	for _, a := range a { // iterate through array a
		for _, b := range b {
			p[i] = pair{a, b} // for each element of array a, make a pair with each element of array b
			i++
		}
	}
	return p // return the final cartesian product
}

func main() {
	fmt.Println(cart2([]int{1, 2}, []int{3, 4}))
	fmt.Println(cart2([]int{3, 4}, []int{1, 2}))
	fmt.Println(cart2([]int{1, 2}, nil))
	fmt.Println(cart2(nil, []int{1, 2}))
}
