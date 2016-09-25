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

		
m = 41
p = 24767

histogram = {}
for c in range(p):
	if c % 2 == 0:
		continue
		
	a = (p + c) / 2

	
	
	k = (modinv(c, a) * a * nu) % a
	if k not in histogram:
		histogram[k] = 0
	histogram[k] += 1