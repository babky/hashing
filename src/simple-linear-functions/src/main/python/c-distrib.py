cd = {}
p = 8508589
m = int(p ** (1.0 / 2.0))

for a in range(p):
	if a <= 0:
		continue

	c = (-p) % a
	if c in cd:
		cd[c] += 1
	else:
		cd[c] = 1

s = 0
for c in cd:
	# print c, cd[c]
	
	 if c <= p/m:
		s += cd[c]

print m
print s 