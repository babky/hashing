package ilm_hash.storages;

import java.math.BigInteger;
import java.util.LinkedList;
import java.util.List;

import ilm_hash.tables.ChainingTableInfo;
import ilm_hash.tables.TableInfo;

public class LinearChainingStorage implements Storage, StorageInfo {

	private final LinkedList<BigInteger>[] storage;

	@SuppressWarnings("unchecked")
	public LinearChainingStorage(int length) {
		storage = new LinkedList[length];
	}

	@Override
	public int getChainLength(int idx) {
		LinkedList<BigInteger> l = storage[idx];
		return l == null ? 0 : l.size();
	}

	@Override
	public int getNumberOfElements() {
		return size;
	}

	@Override
	public int getStorageLength() {
		return storage.length;
	}

	public List<BigInteger> getChain(int idx) {
		return storage[idx];
	}

	@Override
	public void insert(int idx, BigInteger x) throws Exception {
		LinkedList<BigInteger> chain = storage[idx];

		if (chain == null) {
			chain = storage[idx] = new LinkedList<BigInteger>();
		} else if (chain.contains(x)) {
			throw new Exception("Key already stored");
		}

		chain.add(x);
	}

	public TableInfo computeTableInfo() {
		ChainingTableInfo info = new ChainingTableInfo(storage.length);

		for (int i = 0; i < storage.length; ++i) {
			info.addChain(getChainLength(i));
		}

		return info;
	}

	int size = 0;

}
