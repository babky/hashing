package ilm_hash.tables;

public interface TableInfo {

	public int getNumberOfElements();

	public int getStorageLength();

	public int getLongestChain();

	public int getShortestChain();

	public double getLoadFactor();

}
