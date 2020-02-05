with Ada.Text_IO; use Ada.Text_IO;

procedure Main is
    type List is array (Integer range <>) of Integer;  -- this will make the function cleaner looking
    type retArray is array (Integer range <>, Integer range <>) of Integer;  -- 2d array for returned cartesian product

    function cartProd (L1 : List; L2: List) return retArray is
        L3 : retArray;  -- this will be where the cartesian product is stored
        I : Integer := 0;  -- L3 index
        firstLength : Integer := L1'Length;  -- save length of arrays for use in for loop
        secondLength : Integer := L2'Length;

    begin  -- start of body of cartesian product function

        for x in firstLength loop
            for y in secondLength loop
                L3(I) := (L1(x), L2(y));  -- store element combination in L3
                I := I+1;   -- increment index in L3 for next loop
            end loop;  -- end inner loop
        end loop;  -- end outer loop

        return L3;  -- return cartesian product
    end cartProd;

begin  -- start of body of main function

end Main;


