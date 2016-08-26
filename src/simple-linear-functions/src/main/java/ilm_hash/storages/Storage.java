package ilm_hash.storages;

import java.math.BigInteger;

import ilm_hash.tables.TableInfo;

public interface Storage {

	public void insert(int idx, BigInteger x) throws Exception;

	public int getStorageLength();

	public TableInfo computeTableInfo();

}
