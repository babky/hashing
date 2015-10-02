#ifndef TWT_TEST_IMPL_H
#define TWT_TEST_IMPL_H

#include "test.h"
#include "utils/smart_pointer.h"
#include "utils/static_random_generator.h"
#ifdef BOOST_MSVC
	#pragma warning(disable: 4512 4127 4100)
#endif
#include <boost/integer_traits.hpp>
#include <boost/integer.hpp>
#ifdef BOOST_MSVC
	#pragma warning(default: 4512 4127 4100)
#endif

namespace Hash { namespace Tests {

	template<typename ValueType, class TableType, class GeneratorType>
	class TestImpl : public Test {
	public:
		TestImpl(size_t aThreads, size_t aTestLength, size_t aTableLength, bool aAcceptSeed):
		  threads(aThreads),
		  testLength(aTestLength),
		  tableLength(aTableLength),
		  table(aTableLength),
		  acceptSeed(aAcceptSeed) {
		}

		TableType & getTable(void) {
			return table;
		}

		const TableType & getTable(void) const {
			return const_cast<TestImpl *> (this)->getTable();
		}

		virtual void run(void) {
			table.clear();

			size_t partLength = testLength / threads;

			boost::thread ** testThreads = new boost::thread*[threads];
			size_t currentLength, from = 0;
			for (size_t t = 0; t < threads; ++t) {
				currentLength = t == 0 ? testLength - (threads - 1) * partLength : partLength;
				testThreads[t] = new boost::thread(
						TestPart(
							from,
							currentLength,
							Hash::Utils::StaticRandomGenerator<size_t>::getGenerator().generate(),
							t,
							this
						)
					);

				from += currentLength;
			}

			for (size_t t = 0; t < threads; ++t) {
				testThreads[t]->join();
			}

			for (size_t t = 0; t < threads; ++t) {
				delete testThreads[t];
				testThreads[t] = 0;
			}

			delete [] testThreads;
		}

		virtual void computeStatistics(Hash::Utils::StorageStatistics & stats) {
			table.computeStatistics(stats);
		}

		bool getAcceptSeed(void) const {
			return acceptSeed;
		}

	private:
		class TestPart {
		public:
			TestPart(size_t aFrom, size_t aPartLength, size_t seed, size_t aThreadNo, TestImpl<ValueType, TableType, GeneratorType> * aTest):
			  from(aFrom),
			  partLength(aPartLength),
			  generator(new GeneratorType(boost::integer_traits<ValueType>::const_min, boost::integer_traits<ValueType>::const_max, aTest->getAcceptSeed())),
			  threadNo(aThreadNo),
			  test(aTest) {
				generator->setPartLength(partLength);
				generator->setTableLength(test->getTable().getTableSize());
				generator->setSeed(seed);
				generator->setFrom(from);
			}

			void operator()(void) {
				ValueType e;
				generator->initialize();
				ValueType universumMax = test->getTable().getFunction().getUniversumMax();

				for (size_t i = 0; i < partLength; ++i) {
					do {
						e = generator->generate();
					} while (e >= universumMax);

					test->getTable().insert(e);
				}
			}

		private:
			size_t from;
			size_t partLength;
			Hash::Utils::SmartPointer<GeneratorType> generator;
			size_t threadNo;
			TestImpl<ValueType, TableType, GeneratorType> * test;
		};

		TableType table;
		size_t threads;
		size_t testLength;
		size_t tableLength;
		bool acceptSeed;
	};

} }

#endif /* TWT_TEST_IMPL_H */
