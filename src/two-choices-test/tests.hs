import Data.Word(Word)

import Data.Array.ST
import Control.Monad.ST
import Control.Monad
import Control.Exception(assert)

import Data.Array.IArray
import Data.List

import System.Random

import Debug.Trace


-- | Supplying the obvious (but incomplete ATM) instance
instance Random Word where
	random g = let (a,g2) = random g
		in ( fromIntegral (a::Int), g2 )
	randomR = undefined -- for now

-- Creates a random list of given length
randomList 0 g = ([],g)
randomList n g = (a:as,gn) where
	(a,g2) = random g
	(as,gn) = randomList (n-1) g2

-- table length -> chosen chains' lengths -> chosen index
type DecideFun = Word -> [Word] -> Word
decShort :: DecideFun

-- Chooses the first shortest chain
decShort _ (l:ls) = fst $ foldl f (0,l) $ zip [1..] ls where
	f p1@(i1,l1) p2@(i2,l2) = if l1<=l2 then p1 else p2

-- | A set of hash functions: table length, the functions
data HashFuns a = HashFuns !Word ![a->Word]
instance Show (HashFuns a) where
	showsPrec p (HashFuns n l) = showsPrec p (length l) . (' ':) . showsPrec p n
getHashes (HashFuns _ funs) key = map ($key) funs

-- | Generates a set of d standard hash functions on table of length 2^logm
--  the hash functions are of type: h x = a*x >> sh
stdHashFuns:: RandomGen g => Word -> Word -> g -> (HashFuns Word, g)
stdHashFuns d logm g = let (rs,g') = randomList d g -- getting d randoms
	in ( hashx $ map (\x -> x*2+1) rs, g' ) where -- mapping them to odd numbers
	hashx :: [Word] -> HashFuns Word -- list of multipliers -> list of functions
	wlen = fromIntegral (maxBound::Word) + 1 :: Integer -- to prevent overflowing
	denom = fromIntegral $ wlen `div` fromIntegral (2^logm) -- the divisor
	hashx muls = assert -- checking the basic conditions
		( denom/=0 && all odd muls )
		$ HashFuns (2^logm) $ map (\mul val -> mul*val `div` denom) muls




-- | Takes a decision function, a list of keys, a set of hash functions
--	and returns lengths of the buckets (uniqueness of keys isn't tested)
getTableLengths :: DecideFun -> [tk] -> HashFuns tk -> [Word]
getTableLengths decide keys hFuns@(HashFuns m _) = elems $ runSTUArray $ do
	let hashes = map (getHashes hFuns) keys -- list of hashes for every key
	a <- mkArray (0,m-1) 0
	forM_ hashes $ \hs -> do -- take a list of indices
		is <- mapM (readArray a) hs -- read lengths of the corresponding chains
		let i= hs !! fromIntegral (decide m is) -- choose a bucket
		modArray a i (+1) -- increment the chosen index
	return a
	where
		mkArray:: (Word,Word) -> Word -> ST s (STUArray s Word Word)
		mkArray= newArray
		--modArray :: (MArray a e m, Ix i) => a i e -> i -> (e -> e) -> m ()
		modArray a i f = readArray a i >>= writeArray a i . f




-- TESTING STUFF


showStats:: Word -> Word -> Word -> [Word] -> IO ()
showStats logn logm limit lens = let
	groupCounts = map (\x -> (head x,fromIntegral $ length x))
		. reverse . group . sort
	hist = groupCounts lens
	countAbove x = foldr f (0::Integer) where
		f (size,count) sum = if size<=x then sum
			else sum + fromIntegral ((size-x)*count)
	in do
	print hist
	putStrLn $ show ( fromIntegral (countAbove limit hist) / 2^logn ::Double )
		++ " part of keys over limit " ++ show limit

main = let
	hfunGen = stdHashFuns 2 logm
	logn = 20
	logm = logn - 4
	limit = round $ 1.5 * fromIntegral (2^(logn-logm))
	in do
		(hfuns,rgen) <- liftM hfunGen $ getStdGen
			-- ^ generating the functions and taking the remaining randomness
		let keys = fst $ randomList (2^logn) rgen
		let chainLens = getTableLengths decShort keys hfuns
		showStats logn logm limit chainLens
