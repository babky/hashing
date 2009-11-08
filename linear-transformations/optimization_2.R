# Linear transformation systems - c epsilon optimization
# 29. 9. 2009
# Martin Babka

# It needs to be manually checked if the results of the optimization are valid 
# constants that meet all the conditions.

# Computes the best possible constant for the given parameters.
compute_constant = function(k, e, l) {
	# exponent = l - log e + log log (1/alpha)
	# alpha ^ (log c + exponent) <= probability_upper_bound

	# temporary values
	e_member_1 = e / ((k - 1) * 2^l);
	e_member_2 = e / (2 * k * 2^l);
	
	# needed values
	alpha = 1 - e_member_2;
	probability_upper_bound = e - e_member_1;
	exponent = -log2(e) + l + log2(log2(1/alpha));
	
	# alpha ^ log c = probability_upper_bound * alpha ^ -exponent
	# log c = log (probability_upper_bound * alpha ^ -exponent) / log(alpha)
	RHS = probability_upper_bound * alpha^(-exponent);
	print(probability_upper_bound);
	print(RHS);
	print(alpha^(-exponent));
	log_c = log2(RHS)/log2(alpha);

	return (2 ^ log_c);
}

scheme_1_mult_constant = function(k, e, l) {
	# 4 c_e (4 + I / (1 - e))

	c_e = compute_constant(k, e, l);
	I_e = 4.8 / (1 - e);

	return (4 * c_e * (4 + I_e));
}

scheme_2_mult_constant = function(k, e, l) {
	# 8 c_e / (1 - e)

	c_e = compute_constant(k, e, l);

	return (8 * c_e / (1 - e));
}

c_min = compute_constant(3, 0.5, 0);
scheme_1_min = scheme_1_mult_constant(3, 0.5, 0);
scheme_2_min = scheme_2_mult_constant(3, 0.5, 0);

k_min = 2;
k_max = 4;
k_step = 0.01;

l_min = 0.5;
l_max = 2;
l_step = 0.01;

e_min = 0.2;
e_max = 0.99;
e_step = 0.01;

k = k_min;
while (k <= k_max) {
	l = l_min;

	while (l <= l_max) {
		e = e_min;

		while (e <= e_max) {
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
				scheme_2_min_vec = c(k, e, l, scheme_2);
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


