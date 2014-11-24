package ilm_hash.experiments.utils;

public class ThreeElementsCollisionProbabilityComputer {

	public ThreeElementsCollisionProbabilityComputer(int p, int m, int d) {
		this.m = m;
		this.p = p;
		this.d = d;
	}

	public long estimate() {
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