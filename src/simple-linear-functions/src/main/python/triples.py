import math

m = 17
for c in range(500):
	d = 10 * (c + 1)
	S = [0, 1, d]
	k = len(S)
	
	print("Input set: {0}".format(S))
	for p in [58379]:
		cnt = 0
		for a in range(p):
			#if a % 100 == 0:
			#	print(a * 100 // p, "%")
			for b in range(p):
				y = (a * S[0] + b) % p % m
				res = True
				for x in S:
					if (a * x + b) % p % m != y:
						res = False
				if res:
					cnt += 1
	
		print("Prime {0}.".format(p))
		print("\tUpper bound:  {0:d}".format((p ** 2 / (d * m) + p ** 2 / m ** 2)))
		print("\tActual count: {0:d}".format(cnt))
		print("")
	
