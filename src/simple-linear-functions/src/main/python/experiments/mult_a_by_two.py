import random
from fractions import gcd

p = 15485863
# p = 2 ** 100 - 15
m = 2 ** 4
# m = 64
l = int(m ** 0.5)
MAX_STEP = m / l
print "length", l
print "max steps", MAX_STEP
print "m", m

SUM = 0
CNT = 500

def egcd(a, b):
    if a == 0:
        return (b, 0, 1)
    else:
        g, y, x = egcd(b % a, a)
        return (g, x - (b // a) * y, y)

def modinv(a, m):
    g, x, y = egcd(a, m)
    if g != 1:
        raise Exception('modular inverse does not exist')
    else:
        return x % m

SET = set()
for s in range(2): # range(MAX_STEP):
	a = s * p / (MAX_STEP - 1) / 2 + p / 2 + 1
	NEW_SET = []
	c = 2 * a - p
	leaps = (m * modinv(c, a)) % a
	print "leaps", leaps, c, a
	
	for i in range(l):
		x0 = 2 + leaps * i
		for xd in [0]:
			x = (x0 + xd) % p
			print x
			NEW_SET.append(x)
			if x in SET:
				continue
			SET.add(x)
	
	print "a", a
	print "step in a", (-p) % a
	T = {}
	print "x", "leaps", "in Z_a", "is first", "in Z_p", "y"
	for x in NEW_SET:
		y = (a * x) % p % m
		print x, a * x / p, (a * x / p * (-p)) % a, a * (x - 1) / p < a * x / p, a * x % p, y
		assert y < m
		if y not in T:
			T[y] = 0
		T[y] += 1
	
	M = 0
	for y in T:
		if M < T[y]:
			M = T[y]
	print "result for a:", a, M
	
print len(SET), m
assert len(SET) <= m

quit()

for j in range(CNT):
	a = random.randint(0, p - 1)
	b = random.randint(0, p - 1)
	T = {}

	for x in SET:
		y = (a * x) % p % m
		assert y < m
		if y not in T:
			T[y] = 0
		T[y] += 1
	
	M = 0
	for y in T:
		if M < T[y]:
			M = T[y]
	SUM += M

print l	
print float(SUM) / CNT
