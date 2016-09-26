def generate_1(m):
	S = []
	sm = int(m ** 0.5)
	for JJ in range(sm):
		J = (JJ + 2)
		L = []
		l = 0
		T = range(m * (sm + 1) * 2)
		for x in T:
			if x % J == 0:
				l -= 1
			l += 1
			L.append(l)

		print L

		T = map(lambda x: 1 + x, T)
		cnt = 0
		last_l = 0
		i = 0
		while cnt < sm:
			if last_l != L[i] and L[i] % m == 1:
				cnt += 1
				S.append(T[i])
				last_l = L[i]
			i += 1

	S = set(S)
	
generate_1(64)
