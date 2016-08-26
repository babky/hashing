if __name__ == "__main__":
	p = 1873
	a = 300
	ll = 1000
        m = 20
	
	lm = range(ll)
	lmp = map(lambda x: a * x % p, lm)
	lmpm = map(lambda x: x % m, lmp)

	print lm
	print lmp
	print lmpm


