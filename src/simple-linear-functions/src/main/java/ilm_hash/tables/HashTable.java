package ilm_hash.tables;

import java.math.BigInteger;

import ilm_hash.functions.Function;

public interface HashTable {

	public void insert(BigInteger x) throws Exception;

	public int size();

	public int getStorageLength();

	public TableInfo computeTableInfo();

	public void setFunction(Function f) throws UnsupportedOperationException;

}
