package ilm_hash.functions;

import java.math.BigInteger;

import ilm_hash.tables.HashTable;
import ilm_hash.utils.RandomGenerator;

public class SimpleLinearFunction implements Function {

	public SimpleLinearFunction(BigInteger a, BigInteger b, BigInteger p, int tableLength) {
		this.a = a;
		this.b = b;
		this.p = p;
		this.r = tableLength;
	}

	public SimpleLinearFunction(BigInteger a, BigInteger b, BigInteger p, HashTable t) {
		this.a = a;
		this.b = b;
		this.p = p;
		this.r = t.getStorageLength();
		t.setFunction(this);
	}

	public SimpleLinearFunction(BigInteger p, HashTable t) {
		this.r = t.getStorageLength();
		this.p = p;
		this.a = RandomGenerator.generate(p);
		this.b = RandomGenerator.generate(p);
		t.setFunction(this);
	}

	public int hash(BigInteger value) {
		return value.multiply(a).add(b).mod(p).mod(BigInteger.valueOf(r)).intValue();
	}

	public int getRange() {
		return r;
	}

	public void setRange(int range) {
		this.r = range;
	}

	final BigInteger a;
	final BigInteger b;
	final BigInteger p;
	int r;

}
