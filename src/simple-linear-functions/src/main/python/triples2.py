import math

m = 17
d = 36
S = [0, 1, d]
k = len(S)

def bound(p, d, m):
	return 1 + p * (p - 1) / 2 * (d + m) / d / m / m

print("Input set: {0}".format(S))
for p in [2161]:
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
	print("\tUpper bound:  {0:d}".format(bound(p, d, m)))
	print("\tActual count: {0:d}".format(cnt))
	print("")

