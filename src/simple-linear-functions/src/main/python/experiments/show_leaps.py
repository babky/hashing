#p = 15485863
p = 2 ** 100 - 15
m = 2 ** 15
# m = 64
l = int(m ** 0.25)
MAX_STEP = m / l
print "length", l
print "max steps", MAX_STEP
print "m", m

import random
SUM = 0
CNT = 500

def mod_inv(c, a):
	return pow(c, a - 2, a)

LEAP_SET = set()
for s in range(MAX_STEP):
	a = s * p / (MAX_STEP - 1) / 2 + p / 2 + 1
	# NEW_SET = []
	ai = pow(a, p - 2, p)
	for i in range(l):

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
