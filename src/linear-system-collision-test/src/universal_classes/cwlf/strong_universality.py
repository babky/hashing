import sys

print 'N: '
N = int(sys.stdin.readline())
print 'm: '
m = int(sys.stdin.readline())
xVec = [1, 2, 3, 9, 15, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 44]

M = [(a, b) for b in range(N) for a in range(N)]
for x in xVec:
    M = [(a, b) for (a, b) in M if ((a * x + b) % N) % m == 1]
    print "For %d we have %d solutions" % (x, len(M))
    #print "They're: ", M
    print
