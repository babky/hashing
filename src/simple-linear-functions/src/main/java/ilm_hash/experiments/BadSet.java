package ilm_hash.experiments;

import java.math.BigInteger;
import java.util.Comparator;
import java.util.PriorityQueue;

import org.apache.commons.cli.BasicParser;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;

import ilm_hash.experiments.utils.ThreeElementsCollisionProbabilityComputer;
import ilm_hash.functions.Function;
import ilm_hash.functions.SimpleLinearFunction;
import ilm_hash.storages.LinearChainingStorage;
import ilm_hash.tables.HashTable;
import ilm_hash.tables.HashTableDefaultImpl;
import ilm_hash.tables.TableInfo;

public class BadSet {

	static final int DEFAULT_PRIME = 9851;

	static final int DEFAULT_TABLE_SIZE = 256;

	static final int DEFAULT_RUNS = 128;

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
		BigInteger p;
		int runs;
		int m;
		
		CommandLineParser parser = new BasicParser();
		Options options = new Options();
		options.addOption("r", "runs", true, "The number of runs.");
		options.addOption("m", "table-size", true, "The table size.");
		options.addOption("p", "prime", true, "Prime.");
		CommandLine cmd = parser.parse(options, args);
		runs = Integer.parseInt(cmd.getOptionValue("r", Integer.toString(DEFAULT_RUNS)));
		p = new BigInteger(cmd.getOptionValue("p", Integer.toString(DEFAULT_PRIME)));
		m = Integer.parseInt(cmd.getOptionValue("m", Integer.toString(DEFAULT_TABLE_SIZE)));
		
		PriorityQueue<Element> q = new PriorityQueue<>(m, new Comparator<Element>() {

			@Override
			public int compare(Element o1, Element o2) {
				return Long.signum(o1.functionCount - o2.functionCount);
			}

		});

		ThreeElementsCollisionProbabilityComputer c = new ThreeElementsCollisionProbabilityComputer(p.intValue(), m, 0);
		Element e = new Element();
		for (int d = 2; d < p.intValue(); ++d) {
			c.setMaxElement(d);
			if (q.size() < m) {
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
		for (int i = 0; i < runs; ++i) {
			LinearChainingStorage s = new LinearChainingStorage(m);
			HashTable ht = new HashTableDefaultImpl(null, s);
			Function f = new SimpleLinearFunction(p, ht);
			ht.setFunction(f);

			for (Element el : q) {
				ht.insert(new BigInteger(Integer.toString(el.x)));
			}

			TableInfo info = ht.computeTableInfo();

			System.out.println(info.toString());
			sum += info.getLongestChain();
		}

		System.out.println(String.format("Average longest chain: %f.\n", sum / (double) runs));
	}

}
