package ilm_hash.experiments;

import java.math.BigInteger;
import java.util.Comparator;
import java.util.PriorityQueue;

import ilm_hash.experiments.utils.ThreeElementsCollisionProbabilityComputer;
import ilm_hash.functions.Function;
import ilm_hash.functions.SimpleLinearFunction;
import ilm_hash.storages.LinearChainingStorage;
import ilm_hash.tables.HashTable;
import ilm_hash.tables.HashTableDefaultImpl;
import ilm_hash.tables.TableInfo;

public class BadSet {

	static final int P = 9851;

	static final int M = 256;

	static final int RUNS = 128;

	static class Element {

		public Element() {
		}

		public Element(int d, long functionCount) {
			this.x = d;
			this.functionCount = functionCount;
		}

		public long functionCount;

		public int x;

	}

	public static void main(String[] args) throws Exception {
		PriorityQueue<Element> q = new PriorityQueue<>(M, new Comparator<Element>() {

			@Override
			public int compare(Element o1, Element o2) {
				return Long.signum(o1.functionCount - o2.functionCount);
			}

		});

		ThreeElementsCollisionProbabilityComputer c = new ThreeElementsCollisionProbabilityComputer(P, M, 0);
		Element e = new Element();
		for (int d = 2; d < P; ++d) {
			c.setMaxElement(d);
			if (q.size() < M) {
				q.add(new Element(d, c.estimate()));
			} else {
				e.functionCount = c.estimate();
				e.x = d;

				if (q.peek().functionCount < e.functionCount) {
					Element t = q.poll();
					q.add(e);
					e = t;
				}
			}
		}

		int sum = 0;
		for (int i = 0; i < RUNS; ++i) {
			LinearChainingStorage s = new LinearChainingStorage(M);
			HashTable ht = new HashTableDefaultImpl(null, s);
			Function f = new SimpleLinearFunction(new BigInteger(Long.toString(P)), ht);
			ht.setFunction(f);

			for (Element el : q) {
				ht.insert(new BigInteger(Integer.toString(el.x)));
			}

			TableInfo info = ht.computeTableInfo();

			System.out.println(info.toString());
			sum += info.getLongestChain();
		}

		System.out.println(String.format("Average longest chain: %f.\n", sum / (double) RUNS));
	}

}
