p = 24847
m = 128
k = 17
min = p / (k + 1) + 1
max = p / k

for a in range(p):
	if a < min or a > max:
		continue
	print
	print a
	
	leap = 0
	L = []
	for i in range((k + 1) * m):
		l = i * a / p
		if l == leap:
			continue

		leap = l
		L.append((a * i - l * p) % m)
		print "\t".join(map(lambda x: "%6d" % x, [i, (i * a) % p, (i * a) % p % m, i * a / p, l * (a * (k + 1) - p) % m, l * (a * (k) - p) % m]))
		
		if l == m:
			break

	L.sort()
	
	print L, len(set(L))
	print L  == range(m)[1:]
#		print L
#		print range(m)[1:]
