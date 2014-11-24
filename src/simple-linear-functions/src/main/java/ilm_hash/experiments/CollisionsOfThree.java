package ilm_hash.experiments;

import ilm_hash.experiments.utils.ThreeElementsCollisionProbabilityComputer;

import java.io.File;
import java.io.IOException;

import org.apache.commons.cli.BasicParser;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

public class CollisionsOfThree {

	static final int DEFAULT_PRIME = 9851;

	static final int DEFAULT_TABLE_SIZE = 64;

	public static void main(String[] args) throws IOException, ParseException {
		int p;
		int m;

		CommandLineParser parser = new BasicParser();
		Options options = new Options();
		options.addOption("m", "table-size", true, "The table size.");
		options.addOption("p", "prime", true, "Prime.");
		CommandLine cmd = parser.parse(options, args);
		p = Integer.parseInt(cmd.getOptionValue("p", Integer.toString(DEFAULT_PRIME)));
		m = Integer.parseInt(cmd.getOptionValue("m", Integer.toString(DEFAULT_TABLE_SIZE)));

		ThreeElementsCollisionProbabilityComputer t = new ThreeElementsCollisionProbabilityComputer(p, m, 0);
		String fname_full = String.format("coll-3-%d-%d.png", t.getPrime(), t.getTableSize());
		String fname_scaled = String.format("coll-3-%d-%d-scaled.png", t.getPrime(), t.getTableSize());
		String fname_small = String.format("coll-3-%d-%d-%d.png", t.getPrime(), t.getTableSize(),
				t.getPrime() / t.getTableSize());

		XYSeries dataset = new XYSeries("Probabilty");
		dataset.setMaximumItemCount(t.getPrime());

		int percent = -1;
		for (int d = 1; d < t.getPrime(); ++d) {
			if (d * 100 / t.getPrime() > percent) {
				percent = d * 100 / t.getPrime();
				System.err.println(String.format("%2d %%", percent));
			}

			if (d == (t.getPrime() / t.getTableSize())) {
				System.err.println("Outputting small graph.");
				JFreeChart chart = ChartFactory.createXYLineChart("Probability of collision dependent on d", "d", "P",
						new XYSeriesCollection(dataset), PlotOrientation.VERTICAL, true, true, false);

				ChartUtilities.saveChartAsPNG(new File(fname_small), chart, 1024, 768);
			}

			t.setMaxElement(d);
			dataset.add(d, (double) t.estimate() / ((double) t.getSpaceSize()));
		}
		System.err.println("Done, outputting graph.");

		JFreeChart chart = ChartFactory.createXYLineChart("Probability of collision dependent on d", "d", "P",
				new XYSeriesCollection(dataset), PlotOrientation.VERTICAL, true, true, false);

		ChartUtilities.saveChartAsPNG(new File(fname_full), chart, t.getPrime() + 100, 768);
		ChartUtilities.saveChartAsPNG(new File(fname_scaled), chart, 1024, 768);
	}
}
