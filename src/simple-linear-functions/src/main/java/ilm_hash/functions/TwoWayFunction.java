package ilm_hash.functions;

import java.math.BigInteger;

import ilm_hash.storages.StorageInfo;

public class TwoWayFunction<Function1 extends Function, Function2 extends Function> implements Function {

	public TwoWayFunction(Function1 f1, Function2 f2, StorageInfo storageInfo) {
		this.f1 = f1;
		this.f2 = f2;
		this.storageInfo = storageInfo;
	}

	public int hash(BigInteger value) {
		int h1 = f1.hash(value);
		int h2 = f2.hash(value);

		if (storageInfo.getChainLength(h1) <= storageInfo.getChainLength(h2)) {
			return h1;
		} else {
			return h2;
		}
	}

	public int getRange() {
		return f1.getRange();
	}

	public void setRange(int range) {
		f1.setRange(range);
		f2.setRange(range);
	}

	private final StorageInfo storageInfo;
	private final Function1 f1;
	private final Function2 f2;

}
