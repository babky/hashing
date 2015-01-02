package ilm_hash.tests;

import java.math.BigInteger;

import ilm_hash.storages.LinearChainingStorage;
import ilm_hash.tables.HashTable;
import ilm_hash.tables.HashTableDefaultImpl;
import ilm_hash.tables.TableInfo;
import ilm_hash.utils.RandomGenerator;
import ilm_hash.functions.SimpleLinearFunction;
import ilm_hash.generators.LinearGenerator;

public class CwlfTest {

	public static void main(String[] args) throws Exception {

		final int TEST_LENGTH = 64;
		final BigInteger P =
				BigInteger.valueOf(14557);
				// new BigInteger("18446744073709551557");
		final int LOG_TEST_LENGTH = (int) Math.ceil(Math.log(TEST_LENGTH));

		BigInteger x;

		int badCount = 0;
		int percent = -1;
		for (int j = 0; j < TEST_LENGTH; ++j) {
			if (percent < j * 100 / TEST_LENGTH) {
				percent = j * 100 / TEST_LENGTH;
				System.err.println(String.format("%d %%", percent));
			}

			LinearChainingStorage s = new LinearChainingStorage(TEST_LENGTH);
			HashTable ht = new HashTableDefaultImpl(null, s);
			SimpleLinearFunction f = new SimpleLinearFunction(RandomGenerator.generate(P), BigInteger.valueOf(0), P, ht);
			ht.setFunction(f);
			LinearGenerator g = new LinearGenerator();

			for (int i = 0; i < TEST_LENGTH; ++i) {
				x = g.iterator().next();
				ht.insert(x);
			}

			TableInfo ti = ht.computeTableInfo();
			if (ti.getLongestChain() > LOG_TEST_LENGTH) {
				++badCount;
			}

			System.out.print(String.format("%d, %d: ", f.getMultiplier(), ti.getLongestChain()));
			for (int k = 0; k < TEST_LENGTH; ++k) {
				if (s.getChainLength(k) == ti.getLongestChain()) {
					System.out.print(k);
					System.out.print(" ");
					System.out.print(s.getChain(k));
					System.out.print("  ");
				}
			}
			System.out.println();
		}

		System.out
				.println(String.format("No of tests exceeding the logarithmic bound - %d/%d.", badCount, TEST_LENGTH));
	}

}
