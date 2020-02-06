with Ada.Text_IO; use Ada.Text_IO;

procedure cartesian is
    type List is array (Integer range <>) of Integer;  -- this will make the function cleaner looking
    type retArray is array (Integer range <>, Integer range <>) of Integer;  -- 2d array for returned cartesian product

    function cartProd (L1 : List; L2: List) return retArray is
        L3 : retArray (1..50, 1..50);  -- this will be where the cartesian product is stored
        I : Integer := 0;  -- L3 index

    begin  -- start of body of cartesian product function

        for x in 1..L1'Length loop
            for y in 1..L2'Length loop
                L3(I) := (L1(x), L2(y));  -- store element combination in L3
                I := I+1;   -- increment index in L3 for next loop
            end loop;  -- end inner loop
        end loop;  -- end outer loop

        return L3;  -- return cartesian product
    end cartProd;

ex1 : List := (1,2,3);
ex2 : List := (4,5,6);
answer1 : retArray(1..50, 1..50);

ex3 : List := (1);
ex4 : List := (1,2,3,4,5);
answer2 : retArray(1..50, 1..50);


begin  -- start of body of main function
    answer1 := cartProd(ex1, ex2);
    answer2 := cartProd(ex3, ex4);

end cartesian;


