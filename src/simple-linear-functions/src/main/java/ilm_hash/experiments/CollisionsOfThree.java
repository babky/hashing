package ilm_hash.experiments;

import java.io.File;
import java.io.IOException;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class CollisionsOfThree {

	static final int P = 9851;

	static final int M = 64;

	static class Test {

		public Test(int p, int m, int d) {
			this.m = m;
			this.p = p;
			this.d = d;
		}

		public long perform() {
			long res = 0;
			for (int b = 0; b < p; ++b) {
				int a = 0;
				// Case 1: l1 == 0. a + b < p. m | a.
				while (a + b < p) {
					if (((a + b) % m == b % m) && (((a * d + b) % p) % m == b % m)) {
						++res;
					}
					
					a += m;
				}
				
				// Case 2: l1 == 1. a + b >= p. m | p - a.
				a = p - (p % m);
				while (a + b >= p) {
					if (((a + b) % m == b % m) && (((a * d + b) % p) % m == b % m)) {
						++res;
					}
					
					a -= m;
				}
			}

			return res;
		}

		public long getSpaceSize() {
			return p * p;
		}

		public int getTableSize() {
			return m;
		}

		public void setTableSize(int m) {
			this.m = m;
		}

		public int getPrime() {
			return p;
		}

		public void setPrime(int p) {
			this.p = p;
		}

		public int getMaxElement() {
			return d;
		}

		public void setMaxElement(int d) {
			this.d = d;
		}

		int m;

		int p;

		int d;

	}

	public static void main(String[] args) throws IOException {
		Test t = new Test(P, M, 0);
		String fname = String.format("coll-3-%d-%d.png", t.getPrime(), t.getTableSize());
		String fname_small = String.format("coll-3-%d-%d.png", t.getPrime(), t.getTableSize());

		XYSeries dataset = new XYSeries("Probabilty");
		dataset.setMaximumItemCount(t.getPrime());

		int percent = -1;
		for (int d = 0; d < t.getPrime(); ++d) {
			if (d * 100 / t.getPrime() > percent) {
				percent = d * 100 / t.getPrime();
				System.out.println(String.format("%2d %%", percent));
			}
			
			if (d == (t.getPrime() / t.getTableSize())) {
				System.out.println("Outputting small graph.");
				JFreeChart chart = ChartFactory.createXYLineChart("Probability of collision dependent on d", "d", "P",
						new XYSeriesCollection(dataset), PlotOrientation.VERTICAL, true, true, false);

				ChartUtilities.saveChartAsPNG(new File(fname_small), chart, 1024, 768);
			}

			t.setMaxElement(d);
			dataset.add(d, (double) t.perform() / ((double) t.getSpaceSize()));
		}
		System.out.println("Done, outputting graph.");

		JFreeChart chart = ChartFactory.createXYLineChart("Probability of collision dependent on d", "d", "P",
				new XYSeriesCollection(dataset), PlotOrientation.VERTICAL, true, true, false);

		ChartUtilities.saveChartAsPNG(new File(fname), chart, t.getPrime() + 100, 768);
	}
}
