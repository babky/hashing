#ifndef CWLF_EXPONENTIAL_SYSTEM_H
#define CWLF_EXPONENTIAL_SYSTEM_H

namespace Hash { namespace Systems {

	template<typename T, class Storage>
	class CWLFExponentialSystem: public Hash::Systems::UniversalFunction<T, Storage> {
	public:
		explicit CWLFExponentialSystem(size_t aTableSize = StorageParams::INITIAL_STORAGE_SIZE, size_t aUniversumMax =
				Hash::Math::Prime<T>::GREATEST_PRIME) :
				universumMax(aUniversumMax), tableSize(aTableSize) {
			reset();
		}

		void setUniversumMax(T aUniversumMax) {
			universumMax = aUniversumMax;
		}

		T getUniversumMax(void) const {
			return universumMax;
		}

		void setTableSize(size_t size) {
			this->tableSize = size;
		}

		size_t getTableSize(void) const {
			return this->tableSize;
		}

		void reset(void) {
			Hash::Utils::IntegralGeneratorWrapper<T> g = Hash::Utils::IntegralGeneratorWrapper<T>(0,
					this->getUniversumMax());

			a = g.generate();
			b = g.generate();
			i = g.generate() % 5 + 1; // TODO: Constant, logarithm, whatever.
		}

		size_t hash(const T & x) {
			T y = Hash::Math::UnsignedDoubleWord<T>::exponentiate(x, i, universumMax);
			return Hash::Math::UnsignedDoubleWord<T>::linear(a, y, b, universumMax) % tableSize;
		}

		size_t operator()(const T & a) {
			return hash(a);
		}

		void swap(CWLFExponentialSystem & function) {
			std::swap(universumMax, function.universumMax);
			std::swap(a, function.a);
			std::swap(b, function.b);
			std::swap(i, function.i);
			std::swap(tableSize, function.tableSize);
		}

	private:
		T universumMax;
		T a, b, i;
		size_t tableSize;
	};

} }

namespace std {

	template<typename T, class Storage>
	void swap(Hash::Systems::CWLFExponentialSystem<T, Storage> & a, Hash::Systems::CWLFExponentialSystem<T, Storage> & b) {
		a.swap(b);
	}

}

#endif /* CWLF_EXPONENTIAL_SYSTEM_H */
