import Data.Bits (shiftL)

searchBin pred l h
	| l==h = l
	| pred m = searchBin pred l m
	| otherwise = searchBin pred (m+1) h
	where m = (l+h) `div` 2

searchExp pred n0
	| pred n1 = searchBin pred n0 n1
	| otherwise = searchExp pred n1
	where n1 = n0 `shiftL` 1

searchDExp1 pred = let
	l = searchExp (pred . (1 `shiftL`)) 1
	in searchExp pred $ 1 `shiftL` (l-1)

komb n k
	| n<k || k<0 = 0			-- not really needed
	| k>n-k = komb n (n-k)		-- just optimizing
	| otherwise = product [(n-k+1)..n] `div` product [2..k]

fI x = fromIntegral x
log2 = logBase 2

type BoundFunc = Int -> Integer -> (String,Double) -- logn -> k -> (d,name)
boundFind, boundOur :: BoundFunc

boundFind logn k = ("Real", fI $ searchDExp1 (\d -> (d+k) `komb` k >= 1 `shiftL` logn))

boundOur logn' k' = ("Our", logn / (log2 (k / log2 logn)))
	where (logn,k) = (fI logn', fI k')

boundMike logn' k' = ("Mike", logn / (2 * log2 (2* k / log2 logn)))
	where (logn,k) = (fI logn', fI k')

boundHonza logn' k' = ("Honza", logn / 6 / (log2 (4*k) - log2 logn))
	where (logn,k) = (fI logn', fI k')

boundVlada logn' k' = ("Vlada", logn / log2 ( e*(1+(k * log2 (e*k) / logn)))) where
	(logn,k) = (fI logn', fI k')
	e = exp 1

testBounds bnds (logn,k) = "log n = " ++ show logn ++ ", k = " ++ show k ++ "\n"
	++ concatMap (\f -> show (f logn k) ++ "\n") bnds
test = putStr . testBounds [boundFind,boundOur,boundMike,boundHonza,boundVlada]
