p = 24847
m = 23 # int(p**0.5)

LL = []
lastL = []
for aa in range(p / 2):
	a = aa + p / 2
	if a % 2 == 1:
		continue
	
	print "----"
	print "m:", m
	print "a:", a
	print "s:", (-p) % a % m
	print "-:", (p) % a
	print "+:", (-p) % a
	
	sm = m # int(m ** 0.5)
	
	L = []
	for j in range(m * sm * 2):
		i = 1 + j
		print "\t".join(map(lambda x: "%6d" % x, [i, (i * a) % p, (i * a) % p % m, i * a / p]))
		L.append(i * a / p)

	if lastL != L:
		# LL.append(L)
		lastL = L
		print L

# print "\n".join(["\t".join(map(lambda x: "%6d" % x, L)) for L in LL])
