import math
import sys

p = 14461
m = 640

def h(x, a, b):
	return (x * a + b) % p % m

cnt = 0
same_cnt = 0
b = 0
for a in range(p):
	sets = {}
	max = 0
	max_set = -1
	for x in range(m):
		y = h(x, a, b)
		if y in sets:
			sets[y] += [x,]
		else:
			sets[y] = [x,]
			
		if len(sets[y]) > max:
			max = len(sets[y])
			max_set = y
			
#			if max_set != 0:
#				print 'FAIL'
#				print max_set
#				print a
#				print sets
#				exit()
		if len(sets[max_set]) - len(sets[0]) > 1:
			print 'FAIL'
			print a
			print sets
			print
			print max_set
			exit()

	