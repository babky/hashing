package ilm_hash.tests;

import java.math.BigInteger;

import ilm_hash.storages.LinearChainingStorage;
import ilm_hash.tables.HashTable;
import ilm_hash.tables.HashTableDefaultImpl;
import ilm_hash.tables.TableInfo;
import ilm_hash.functions.Function;
import ilm_hash.functions.SimpleLinearFunction;
import ilm_hash.generators.LinearGenerator;

public class CwlfTest {

	public static void main(String[] args) throws Exception {

		final int TEST_LENGTH = 1 << 16;
		final BigInteger P = new BigInteger("18446744073709551557");
		final int LOG_TEST_LENGTH = (int) Math.ceil(Math.log(TEST_LENGTH));

		BigInteger x;

		int badCount = 0;
		for (int j = 0; j < TEST_LENGTH; ++j) {
			LinearChainingStorage s = new LinearChainingStorage(TEST_LENGTH);
			HashTable ht = new HashTableDefaultImpl(null, s);
			Function f = new SimpleLinearFunction(P, ht);
			ht.setFunction(f);
			LinearGenerator g = new LinearGenerator();
			g.setSeed(TEST_LENGTH);

			for (int i = 0; i < TEST_LENGTH; ++i) {
				x = g.iterator().next();
				// System.out.println("h(" + x + ") = " + f.hash(x));
				ht.insert(x);
			}

			TableInfo ti = ht.computeTableInfo();
			if (ti.getLongestChain() > LOG_TEST_LENGTH) {
				// System.out.println(ti);
				++badCount;
				System.out.println(String.format("%d Fail.", j));
			} else {
				System.out.println(String.format("%d OK.", j));
			}
		}

		System.out.println(String.format("Bad count - %d/%d.", badCount, TEST_LENGTH));
	}

}
