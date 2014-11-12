package ilm_hash.tables;

public class ChainingTableInfo implements TableInfo {

	public ChainingTableInfo(int tableSize) {
		this.tableLength = tableSize;
	}

	@Override
	public int getNumberOfElements() {
		return size;
	}

	@Override
	public int getStorageLength() {
		return tableLength;
	}

	@Override
	public int getLongestChain() {
		return maxChain;
	}

	@Override
	public double getLoadFactor() {
		return (double) size / (double) tableLength;
	}

	public void addChain(int chainLength) {
		size += chainLength;

		if (maxChain < chainLength) {
			maxChain = chainLength;
		}
	}

	@Override
	public String toString() {
		return String.format("Table length: %d\nElement no: %d\nLongest chain: %d", tableLength, size, maxChain);
	}

	int size;

	int tableLength;

	int maxChain = -1;

}
