prob = function (r) {
	return (log2(r) / r) ^ (log2(r / log2(r)) + log2(log2(r / log2(r))))
}

c = function (e, I, k) {
	e4 = 1 - e / (4 * k)
	le4 = log2(e4)
	le = log2(e)
	
	f = (le4 * (log2(k) + log2(-le4)) - log2(e - (e / k))) / (le4 * le)
	return (4 * (4 + I / (1 - e)) * (e ^ (-f)))
}

sum = 0
for (i in 4:15) {
	sum = sum + prob(i)
}
I = sum + 2 / (16 ^ 0.5)

min = 1000000000;
max_e = 1000
max_k = 500

for (j in 1:max_k) {
	k = 1 + j/100
	for (i in 1:(max_e - 1)) {
		e = i / max_e
		r = c(e, I, k)
		if (min > r) {
			min = r
			min_k = k
			min_e = e
		}
	}
}

print (min)
print (min_k)
print (min_e)
