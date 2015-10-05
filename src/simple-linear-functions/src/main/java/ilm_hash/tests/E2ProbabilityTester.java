package ilm_hash.tests;

import ilm_hash.utils.RandomGenerator;

import java.math.BigInteger;
import java.util.HashSet;
import java.util.Set;

public class E2ProbabilityTester {

	public static void main(String[] args) {
		testH2insideH1();
	}

	public static void testH2insideH1() {
		// Choose m
		int m = 1 << 16;
		final BigInteger p = new BigInteger("18446744073709551557");

		// Choose gamma
		double gamma = 8;

		// Compute delta, f.
		int delta = (int) Math.ceil(gamma * Math.log(m) / Math.log(2));
		int f = delta * m;
		BigInteger fBig = BigInteger.valueOf(f);

		// Expected value
		int expectedValue = 100;
		int iterationCount = expectedValue * m;

		// Number of successes.
		int containedNo = 0;

		for (int j = 0; j < iterationCount; ++j) {
			// Choose a.
			BigInteger a = RandomGenerator.generate(p);

			Set<Integer> results = new HashSet<Integer>();
			for (int x = 0; x < m; ++x) {
				results.add(a.multiply(BigInteger.valueOf(x)).mod(fBig).intValue());
			}

			boolean contained = true;
			for (int i = 0; i < delta; ++i) {
				if (!results.contains(i * m)) {
					contained = false;
					break;
				}
			}

			if (contained) {
				++containedNo;
			}
		}

		System.out.println(
			String.format("Contained/all: %d/%d, expected value: %d.", containedNo, iterationCount, expectedValue)
		);
	}

}
