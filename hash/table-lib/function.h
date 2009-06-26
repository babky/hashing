#ifndef FUNCTION_H
#define FUNCTION_H

namespace Hash {

	template<typename T>
	class Function {
	public:
		typedef size_t HashType;

		Function(void) {
		}

		size_t hash(const T & a, size_t length) {
			return a % length;
		}

		size_t operator()(const T & a, size_t length) {
			return hash(a, length);
		}

	private:
		Function(const Function &);
		Function & operator =(const Function &);
	};

}

#endif /* FUNCTION_H */