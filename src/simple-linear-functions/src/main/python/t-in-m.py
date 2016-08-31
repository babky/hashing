import math
import sys

p = 14461
m = 640

def h(x, a, b):
	return (x * a + b) % p % m

cnt = 0
same_cnt = 0
for b in range(p):
	for a in range(p):
		sets = {}
		s = set()
		for x in range(m):
			y = h(x, a, b)
			if y in sets:
				sets[y] += [x,]
			else:
				sets[y] = [x,]
				
			if len(sets[y]) > len(s):
				s = sets[y]

		t = len(s)
		tt = int(math.sqrt(t))
		# Try to find the prescribed arithmetic progression.
		l = 0
		ok = False
		for d in range(m // tt):
			for e in range(d):
				collide = True
				y = h(e, a, b)
				for i in range(tt):
					if h(i * d + e, a, b) != y:
						collide = False

				if collide:
					ok = True
					
		if not ok:
			print('FAIL')
			print(a, b)
			for x in s:
				print("x: {0:7d}\th(x): {1:7d}\t leaps: {2}\t({3}, {4})".format(x, h(x, a, b), (a * x + b) // p, a, b))
			exit()
