package ilm_hash.functions;

import java.math.BigInteger;

public interface Function {

	public int hash(BigInteger value);

	public int getRange();

	public void setRange(int range);

}
