def collide(elements, f):
	y = f(elements[0])
	for x in elements[1:]:
		if y != f(x):
			return False

	return True

class ModularFunction(object):

	def __init__(self, a, b, p, m):
		self._a = a
		self._b = b
		self._m = m
		self._p = p

	def __call__(self, x):
		return (self._a * x + self._b) % self._p % self._m

	def __str__(self):
		return "Modular Function: ({0} * x + {1}) % {2} % {3}".format(self._a, self._b, self._p, self._m)

def modular_function_generator(p, m):
	for a in range(1, p):
		for b in range(p):
			yield ModularFunction(a, b, p, m)

if __name__ == "__main__":
	function_count = 0
	elements = [0, 1, 3965]
	a = 0

	for f in modular_function_generator(2**12 - 3, 256):
		if a < f._a - 9:
			a = f._a
			print("Current a: {0}".format(a))

		if collide(elements, f):
			function_count += 1

	print(function_count)

