#ifndef LINEAR_PROBING_TABLE_IMPLEMENTATION_H
#define LINEAR_PROBING_TABLE_IMPLEMENTATION_H

#include <algorithm>

namespace Hash { namespace Miscellaneous {

	template<typename T, class HashFunction>
	class Table {
	public:
		explicit Table(size_t tableSize = 16) {
			storage = new StorageItem[tableSize];
			this->tableSize = tableSize;
			elementCount = 0;
			f.setTableSize(tableSize);
			f.reset();
		}

		size_t size() const {
			return elementCount;
		}

		size_t getTableSize() const {
			return tableSize;
		}

		void insert(const T & e) {
			++elementCount;

			if (2 * elementCount > tableSize) {
				rehash(2 * tableSize);
			}

			for (size_t i = f(x); storage[i].empty == false; i = (i + 1) % tableSize) {
			}

			storage[i].empty = false;
			storage[i].item = e;
		}

		void rehash(size_t newTableSize) {
			Table t(newTableSize);
			for (size_t i = 0; i < tableSize; ++i) {
				if (storage[i].empty == false) {
					t.insert(storage[i].item);
				}
			}

			swap(t);
		}

		void swap(Table & t) {
			using namespace std;

			swap(storage, t.storage);
			swap(tableSize, t.tableSize);
			swap(elementCount, t.elementCount);
			swap(f, t.f);
		}

		bool find(const T & e) {
			for (size_t i = f(x); storage[i].empty; i = (i + 1) % tableSize) {
				if (storage[i].item == e) {
					return true;
				}
			}

			return false;
		}

	private:
		struct StorageItem {
			bool empty;
			T item;
		};

		StorageItem * storage;
		size_t tableSize;
		size_t elementCount;
		HashFunction f;
	};

} }

#endif /* LINEAR_PROBING_TABLE_IMPLEMENTATION_H */
