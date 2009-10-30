import sys

print 'N: '
N = int(sys.stdin.readline())
print 'm: '
m = int(sys.stdin.readline())
xVec = [1, 2, 3, 9, 15, 16]

M = [(a, b) for b in range(N) for a in range(N)]
for x in xVec:
    M = [(a, b) for (a, b) in M if ((a * x + b) % N) % m == 0]
    print "For %d we have %d solutions" % (x, len(M))
    # print "They're: ", M
    print
