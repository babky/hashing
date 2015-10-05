def lbin(S, a, b, p, m):
	T = {}
	for x in S:
		y = (a * x + b) % p % m
		# print y
		if y in T:
			T[y] += 1
		else:
			T[y] = 1
			
	m = 0
	for y in T:
		if m < T[y]:
			m = T[y]
			
	return m
