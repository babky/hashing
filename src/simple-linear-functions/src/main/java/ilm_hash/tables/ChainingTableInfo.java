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
	public int getShortestChain() {
		return minChain;
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

		if (chainLength > 0 && minChain > chainLength) {
			minChain = chainLength;
		}
	}

	@Override
	public String toString() {
		return String.format("Table length: %d\nElement no: %d\nLongest chain: %d\nShortest chain: %d", tableLength,
				size, maxChain, minChain);
	}

	int size;

	int tableLength;

	int maxChain = -1;

	int minChain = Integer.MAX_VALUE;

}
