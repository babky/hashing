package ilm_hash.tests;

import java.math.BigInteger;

import ilm_hash.functions.Function;
import ilm_hash.functions.SimpleLinearFunction;
import ilm_hash.functions.TwoWayFunction;
import ilm_hash.generators.LinearGenerator;
import ilm_hash.storages.LinearChainingStorage;
import ilm_hash.tables.HashTable;
import ilm_hash.tables.HashTableDefaultImpl;
import ilm_hash.tables.TableInfo;

public class TwoChoicesCwlfTest {

	public static void main(String[] args) throws Exception {

		final int TEST_LENGTH = 50000;
		final BigInteger P = new BigInteger("18446744073709551557");
		// final BigInteger P = new BigInteger("612067");
		BigInteger x;

		for (int j = 0; j < 10; ++j) {
			LinearChainingStorage s = new LinearChainingStorage(TEST_LENGTH);
			HashTable ht = new HashTableDefaultImpl(null, s);
			//Function f = new TwoWayFunction<SimpleLinearFunction, SimpleLinearFunction>(new SimpleLinearFunction(P, ht), new SimpleLinearFunction(P, ht), s);
			Function f = new TwoWayFunction<SimpleLinearFunction, SimpleLinearFunction>(
					new SimpleLinearFunction(BigInteger.valueOf(1), BigInteger.valueOf(2), P, ht),
					new SimpleLinearFunction(BigInteger.valueOf(2), BigInteger.valueOf(32), P, ht),
					s
				);
			ht.setFunction(f);
			LinearGenerator g = new LinearGenerator();
			g.setSeed(TEST_LENGTH);

			for (int i = 0; i < TEST_LENGTH; ++i) {
				x = g.iterator().next();
				// System.out.println(x);
				// System.out.println(f.hash(x));

				ht.insert(x);
			}

			TableInfo ti = ht.computeTableInfo();
			//if (ti.getLongestChain() > 10) {
				System.out.println(ti);
			//}
		}

	}


}
