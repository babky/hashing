#ifndef POLICIES_H
#define POLICIES_H

#include <stdexcept>

namespace Hash { namespace Storages {

	class InvalidProbingCoefficientException : public std::exception {
	public:
		const char * what(void) const {
			return "Invalid probing coefficients.";
		}
	};

	template<typename Hash>
	class ProbingPolicy {
	public:
		virtual ~ProbingPolicy(void) {
		}

		virtual Hash probe(Hash hash, size_t i) const = 0;
	};

	template<typename Hash>
	class LinearProbingPolicy : public ProbingPolicy<Hash> {
	public:
		explicit LinearProbingPolicy(size_t aCoefficient = 1):
		  coefficient(aCoefficient)
		{
			if (aCoefficient == 0) {
				throw InvalidProbingCoefficientException();
			}
		}

		Hash probe(Hash hash, size_t i) const {
			return hash + i * coefficient;
		}

		void swap(LinearProbingPolicy & b) {
			using std::swap;

			size_t & aCoef = const_cast<size_t &> (coefficient);
			size_t & bCoef = const_cast<size_t &> (b.coefficient);

			swap(aCoef, bCoef);
		}

		friend void swap(LinearProbingPolicy & a, LinearProbingPolicy & b) {
			a.swap(b);
		}

	private:
		LinearProbingPolicy & operator=(const LinearProbingPolicy &);

		const size_t coefficient;
	};

	template<typename Hash>
	class QuadraticProbingPolicy : public ProbingPolicy<Hash> {
	public:
		explicit QuadraticProbingPolicy(size_t aQuadraticCoefficient = 1, size_t aLinearCoefficient = 1):
		  quadCoef(aQuadraticCoefficient),
		  linCoef(aLinearCoefficient) 
		{
			if (aLinearCoefficient == 0) {
				throw InvalidProbingCoefficientException();
			}
		}

		Hash probe(Hash hash, size_t i) const {
			return hash + i * (i * quadCoef + linCoef);
		}

		void swap(QuadraticProbingPolicy & b) {
			using std::swap;

			size_t & aQCoef = const_cast<size_t &> (quadCoef);
			size_t & bQCoef = const_cast<size_t &> (b.quadCoef);
			swap(aQCoef, bQCoef);

			size_t & aLCoef = const_cast<size_t &> (linCoef);
			size_t & bLCoef = const_cast<size_t &> (b.linCoef);
			swap(aLCoef, bLCoef);
		}

		friend void swap(QuadraticProbingPolicy & a, QuadraticProbingPolicy & b) {
			a.swap(b);
		}

	private:
		QuadraticProbingPolicy & operator=(const QuadraticProbingPolicy &);

		const size_t quadCoef;
		const size_t linCoef;
	};

} }

#endif /* POLICIES_H */