from lib import lbin

def lemma_inverse(p, a, S, m):
	lbinP = lbin(S, a, 0, p, m)
	lbinM = lbin(S, p - a, 0, p, m)
	return (lbinP, lbinM)
	
def lemma_add(p, a, b, S, m):
	lbin0 = lbin(S, a, 0, p, m)
	lbinB = lbin(S, a, b, p, m)
	return (lbin0, lbinB)
	
if __name__ == "__main__":
	p = 15485863
	a = 4
	S = [0, 1, 2, 3]

	print lemma_inverse(p, a, S, len(S))
	print lemma_add(p, a, p - 7, S, len(S))

