package ilm_hash.tables;

import java.math.BigInteger;

import ilm_hash.storages.Storage;
import ilm_hash.functions.Function;

public class HashTableDefaultImpl implements HashTable {

	public HashTableDefaultImpl(Function f, Storage s) {
		this.f = f;
		this.s = s;
		this.size = 0;
	}

	Function f;

	final Storage s;

	@Override
	public void insert(BigInteger x) throws Exception {
		s.insert(f.hash(x), x);
		++size;
	}

	@Override
	public int size() {
		return size;
	}

	@Override
	public int getStorageLength() {
		return s.getStorageLength();
	}

	@Override
	public TableInfo computeTableInfo() {
		return s.computeTableInfo();
	}

	private int size;

	@Override
	public void setFunction(Function f) throws UnsupportedOperationException {
		if (this.size() != 0) {
			throw new UnsupportedOperationException("Can not change function when there are hashed elements.");
		}

		this.f = f;
	}

}
