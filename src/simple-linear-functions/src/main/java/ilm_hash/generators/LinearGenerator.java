package ilm_hash.generators;

import java.math.BigInteger;
import java.util.Iterator;

public class LinearGenerator implements Generator {

	public int getSeed() {
		return seed;
	}

	public void setSeed(int seed) {
		this.seed = seed;
	}

	@Override
	public Iterator<BigInteger> iterator() {
		return new Iterator<BigInteger>() {

			@Override
			public void remove() {
				throw new UnsupportedOperationException();
			}

			@Override
			public BigInteger next() {
				return BigInteger.valueOf(i++).multiply(BigInteger.valueOf(seed));
			}

			@Override
			public boolean hasNext() {
				return true;
			}
		};
	}

	int i = 0;

	int seed = 1;

}
