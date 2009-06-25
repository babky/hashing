#ifndef FUNCTION_H
#define FUNCTION_H

namespace Hash {

	template<typename T>
	class Function {
	public:
		Function(void) {
		}

		size_t hash(const T & a, int length) {
			return a % length;
		}

	private:
		Function(const Function &);
		Function & operator =(const Function &);
	};

}

#endif /* FUNCTION_H */