package ilm_hash.experiments;

import ilm_hash.experiments.utils.ThreeElementsCollisionProbabilityComputer;

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

	public static void main(String[] args) throws IOException {
		ThreeElementsCollisionProbabilityComputer t = new ThreeElementsCollisionProbabilityComputer(P, M, 0);
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
			dataset.add(d, (double) t.estimate() / ((double) t.getSpaceSize()));
		}
		System.out.println("Done, outputting graph.");

		JFreeChart chart = ChartFactory.createXYLineChart("Probability of collision dependent on d", "d", "P",
				new XYSeriesCollection(dataset), PlotOrientation.VERTICAL, true, true, false);

		ChartUtilities.saveChartAsPNG(new File(fname), chart, t.getPrime() + 100, 768);
	}
}
