-- Code taken from http://www.rosettacode.org/wiki/Cartesian_product_of_two_or_more_lists#Go
-- Comments added by me


cartProd :: [a] -> [b] -> [(a, b)]  -- take in sets with a and b, return set of (a,b) pairs
cartProd xs ys = 
  [ (x, y)
  | x <- xs 
  , y <- ys ]       -- generate set of every combination of xs and ys

main :: IO ()
main =
  mapM_ print $
  uncurry cartProd <$>
  [([1, 2], [3, 4]), ([3, 4], [1, 2]), ([1, 2], []), ([], [1, 2])]  -- call cartProd w/ example lists and print results