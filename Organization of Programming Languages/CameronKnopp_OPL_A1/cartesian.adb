with Ada.Text_IO; use Ada.Text_IO;

procedure cartesian is
    type List is array (1..3) of Integer;  -- for inputted lists
    type pair is array(1..2) of Integer; 
    type retArray is array (1..9) of pair;  -- array of pairs for final cartesian product

    function cartProd (L1 : List; L2: List) return retArray is
        L3 : retArray;  -- this will be where the cartesian product is stored
        I : Integer := 1;  -- L3 index

    begin  -- start of body of cartesian product function
        for x in 1..3 loop
            for y in 1..3 loop
                L3(I) := (L1(x), L2(y));  -- store element combination in L3
                I := I+1;   -- increment index in L3 for next loop
            end loop;  -- end inner loop
        end loop;  -- end outer loop

        return L3;  -- return cartesian product
    end cartProd;

ex1 : List := (1,2,3);
ex2 : List := (4,5,6);
answer1 : retArray;

ex3 : List := (1,5,7);
ex4 : List := (1,2,3);
answer2 : retArray;

ex5 : List := (10,12,13);
ex6 : List := (9,22,35);
answer3 : retArray;

ex7 : List := (5,2,1);
ex8 : List := (5,2,1);
answer4 : retArray;


begin  -- start of body of main function
    answer1 := cartProd(ex1, ex2);  -- call the cartesian product function and store the returned value
    answer2 := cartProd(ex3, ex4);
    answer3 := cartProd(ex5, ex6);
    answer4 := cartProd(ex7, ex8);

    -- Print out each cart product by iterating through it and at each iteration printing indices 1 and 2

    Put("(1,2,3) X (4,5,6):     {");
    for x in 1..9 loop  -- iterate through the 9 pairs in the cartesian product
        Put(" (");
        Put(Integer'Image(answer1(x)(1)));  -- need to use Integer'Image in order to print an integer with Put
        Put(",");
        Put(Integer'Image(answer1(x)(2)));
        Put(") ");
    end loop;
    Put("}");
    New_Line(1);

    Put("(1,5,7) X (1,2,3):     {");
    for x in 1..9 loop
        Put(" (");
        Put(Integer'Image(answer2(x)(1)));
        Put(",");
        Put(Integer'Image(answer2(x)(2)));
        Put(") ");
    end loop;
    Put("}");
    New_Line(1);

    Put("(10,12,13) X (9,22,35):    {");
    for x in 1..9 loop
        Put(" (");
        Put(Integer'Image(answer3(x)(1)));
        Put(",");
        Put(Integer'Image(answer3(x)(2)));
        Put(") ");
    end loop;
    Put("}");
    New_Line(1);

    Put("(5,2,1) X (5,2,1):    {");
    for x in 1..9 loop
        Put(" (");
        Put(Integer'Image(answer4(x)(1)));
        Put(",");
        Put(Integer'Image(answer4(x)(2)));
        Put(") ");
    end loop;
    Put("}");
    New_Line(1);

end cartesian;  -- end of main function


