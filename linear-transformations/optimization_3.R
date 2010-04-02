# Linear transformation systems - c epsilon optimization
# Modified proof of the second Lemma.
# 29. 9. 2009
# Martin Babka

# It needs to be manually checked if the results of the optimization are valid 
# constants that meet all the conditions.

# Computes the best possible constant for the given parameters.
compute_constant = function(k, e, l) {
	# 2^l / (k 2^l - 2) + (1 - 1/k) ^ exponent <= e
	# exponent = log c_e + log log (1 - 1/k) - l
	
	alpha = 1 - 1 / k;
	l_base = 2 ^ l;
	first_prob = l_base / (l_base * k - 2);
	exp_base = alpha ^ (log2(log2(1 / alpha)) - l);
	RHS = (e - first_prob) / exp_base;
	log_c = log2(RHS) / log2(alpha);

	return (2 ^ log_c);
}

scheme_1_mult_constant = function(k, e, l) {
	# 4 c_e (4 + I / (1 - e))

	c_e = compute_constant(k, e, l);
	I_e = 4.8 / (1 - e);

	return (4 * c_e * (4 + I_e));
}

scheme_2_mult_constant = function(k, e, l) {
	# 4 c_e / (1 - e)

	c_e = compute_constant(k, e, l);

	return (4 * c_e / (1 - e));
}

scheme_3_mult_constant = function(a, p, e, k, l) {
	c_e = compute_constant(k, e, l);
	R = (1 - e) * p;
	j = 1;
	step = 0.01;

	L = (12 * j) ^ (-log2(a) - log2(j) - log2(log2(j * 12)));
	while ((L < R) && (j > 0)) {
		j = j - step;
		L = (12 * j) ^ (-log2(a) - log2(j) - log2(log2(j * 12)));		
	}

	j = j + step;
	return (4 * c_e * a * j);
}

compute_integral = function() {
	s = 0;
	h = 10;

	for (x in 0:(2048-16) * h) {
		r = 16 + x / h;
		d = r / log2(r);
		s = s + 1 / h * d ^ (1 - log2(log2(d)));
	}

	return (s + (2048 ^ (-0.3)) / 0.3);
}

print (compute_integral())

c_min = compute_constant(2, 2/3, 3);
scheme_1_min = scheme_1_mult_constant(2, 2/3, 3);
scheme_2_min = scheme_2_mult_constant(2, 2/3, 3);
scheme_3_min = scheme_3_mult_constant(1.5, 0.5, 2/3, 2, 3);

k_min = 2;
k_max = 4;
k_step = 0.01;

l_min = 1.1;
l_max = 3;
l_step = 0.01;

e_min = 0.95;
e_max = 0.99;
e_step = 0.01;

k = k_min;
while (k <= k_max) {
	l = l_min;

	while (l <= l_max) {
		e = e_min;

		while (e <= e_max) {
			# print(c(k, e, l));

			c = compute_constant(k, e, l);

			if (c < c_min) {
				c_min_vec = c(k, e, l, c);
				c_min = c;
				# print(c_min_vec);
			}

			scheme_1 = scheme_1_mult_constant(k, e, l);
			if (scheme_1 < scheme_1_min) {
				scheme_1_min = scheme_1;
				scheme_1_min_vec = c(k, e, l, scheme_1);
				# print(scheme_1_min_vec);
			}

			scheme_2 = scheme_2_mult_constant(k, e, l);
			if (scheme_2 < scheme_2_min) {
				scheme_2_min = scheme_2;
				scheme_2_min_vec = c(k, e, l, scheme_2, c);
				# print(scheme_2_min_vec);
			}

			scheme_3 = scheme_3_mult_constant(1.0, 0.5, e, k, l);
			if (scheme_3 < scheme_3_min) {
				scheme_3_min = scheme_3;
				scheme_3_min_vec = c(k, e, l, scheme_3, c);
				# print(scheme_2_min_vec);
			}


			e = e + e_step;
		}

		l = l + l_step;
	}

	k = k + k_step;
}

print(c_min_vec);
print(scheme_1_min_vec);
print(scheme_2_min_vec);

# Additive constant
c_e = scheme_2_min_vec[5];
e = scheme_2_min_vec[2];
print (2 * c_e * ((3.36 - 4) / (1 - e) + 4));

print(scheme_3_min_vec);


