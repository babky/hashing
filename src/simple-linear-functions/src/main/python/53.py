p = 22637
m = 128
l = 4
prob = 0

for a in range(p):
	sa = map(lambda x: x * a % p % m, range(m))
	mc = 0
	px = -1
	for x in sorted(sa):
		if px != x:
			c = 0
			if mc < c:
				mc = c
		else:
			c += 1

		px = x

	if mc < c:
		mc = c
	
	if mc >= l:
		# print a, sa
		prob += 1


print prob
