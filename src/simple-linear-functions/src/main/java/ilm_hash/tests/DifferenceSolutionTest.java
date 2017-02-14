package ilm_hash.tests;

import org.apache.commons.lang3.StringUtils;

public class DifferenceSolutionTest {

	public static void main(String[] args) throws Exception {
		final long P = 104543;
		final long M = 16;

		for (long a = 1; a < P; ++a) {
			long minD1 = -1, minD2 = - 1;
			for (long d = 1; d < M; ++d) {
				if ((a * d % P) % M == 0) {
					if (minD1 == -1) {
						minD1 = d;
					} else if (d % minD1 > 0) {
						throw new Exception("More than one solution!");
					}
				}

				if ((P - (a * d % P)) % M == 0) {
					if (minD2 == -1) {
						minD2 = d;
					} else if (d % minD2 > 0) {
						throw new Exception("More than one solution!");
					}
				}
			}

			String s1 = String.format("%5d (%6d)", minD1, a * minD1 % P);
			if (minD1 == -1) {
				s1 = StringUtils.repeat(' ', 14);
			}
			String s2 = String.format("%5d (%6d)", minD2, a * minD2 % P - P);
			if (minD2 == -1) {
				s2 = StringUtils.repeat(' ', 14);
			}

			if (minD1 > 0 && minD2 > 0) {
				System.out.println(String.format("%5d: %s - %s", a, s1, s2));
			}
		}
	}

}
