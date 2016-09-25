a = 450
p = 24867
c = (-p) % a
m = 17
l = m

k = p / a

print p / (k + 1), p / k
print p + c, a * (k + 1)
print c, a * (k + 1) - p

print "a:", a
print "k:", k
print "c:", c

for i in range(l):
	print i * c % a, i * c % a % m, i * c / a


