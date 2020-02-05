with Ada.Text_IO; use Ada.Text_IO;

procedure Main is

type List is array (Integer range <>) of Integer;  -- this will make the function cleaner looking
type retArray is array (Integer range <>, Integer range <>) of Integer;  -- 2d array for returned cartesian product

    function cartProd (L1 : List; L2: List) return retArray is
        L3 : retArray;  -- this will be where the cartesian product is stored
        I : Integer := 0;  -- L3 index

    begin

        for element1 in L1 loop
            for element2 in L2 loop
                L3(I) := (element1, element2);  -- store element combination in L3
                I := I+1;   -- increment index in L3 for next loop
            end loop;  -- end inner for loop
        end loop;  -- end outer for loop

        return L3;  -- return cartesian product
    end cartProd;

begin

end Main;


