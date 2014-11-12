package ilm_hash.tables;

public interface TableInfo {

	public int getNumberOfElements();

	public int getStorageLength();

	public int getLongestChain();

	public double getLoadFactor();

}
