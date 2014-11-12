p = 97
a = 13
i = 0
for x in range(p):
	if x * a % p < a:
		print x * a % p
		print (-i * p) % a
		i += 1
