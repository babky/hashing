package ilm_hash.experiments;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;

public class LargeD {

	static final int P = 14499713;

	static final int M = 65536;

	static class Test {

		public Test(int p, int m, int d) {
			this.m = m;
			this.p = p;
			this.d = d;
		}

		public int perform() {
			final int pm = p / m;
			int res = 0;
			int a, l;
			for (int k = 0; k < pm; ++k) {
				a = k * m;
				l = (int) (((long) a * d) / p);
				if (l % m == 0) {
					++res;
				}
			}

			return res;
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
		FileOutputStream fos = new FileOutputStream(String.format("large-d-%d-%d.out", t.getPrime(), t.getTableSize()));
		PrintWriter pw = new PrintWriter(fos);

		for (int d = 0; d < t.getPrime(); ++d) {
			t.setMaxElement(d);
			pw.format("%d\t%d\n", d, t.perform());
		}

		pw.close();
		fos.close();
	}

}
