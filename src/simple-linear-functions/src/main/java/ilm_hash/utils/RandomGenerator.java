package ilm_hash.utils;

import java.math.BigInteger;
import java.util.Random;

public final class RandomGenerator {

	public static final Random RANDOM = new Random();

	public static BigInteger generate(BigInteger p) {
		BigInteger a;
		for (a = p; a.compareTo(p) >= 0; a = new BigInteger(p.bitLength(), RANDOM)) {
		}

		return a;
	}

}
