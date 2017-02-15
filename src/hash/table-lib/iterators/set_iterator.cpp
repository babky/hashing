#include "set_iterator.h"
#include <algorithm>
#include <numeric>
#include <utility>

using namespace std;
using namespace Hash::Iterators;

bool Hash::Iterators::contains(const ElementVector & elements, std::size_t element) {
	return find(elements.begin(), elements.end(), element) != elements.end();
}

ElementVector Hash::Iterators::first(std::size_t count, const ElementVector & fixedElements, const ElementVector & disabledElements) {
	ElementVector v(fixedElements);
	v.reserve(count);

	for (std::size_t i = fixedElements.size(), o = 0; i < count; ++i, ++o) {
		while (contains(fixedElements, o) || contains(disabledElements, o)) {
			++o;
		}

		v.push_back(o);
	}

	return v;
}

ostream & std::operator <<(std::ostream & out, const ElementVector & v) {
	for (std::size_t i = 0; i < v.size(); ++i) {
		if (i > 0) {
			out << " ";
		}
		out << v[i];
	}
	return out;
}

size_t compute_max(size_t max, const ElementVector & fixedElements, const ElementVector & disabledElements) {
	for(--max; contains(fixedElements, max) || contains(disabledElements, max); --max) {
	}
	return max;
}

void fix_value(std::size_t & val, const ElementVector & fixedElements, const ElementVector & disabledElements) {
	for (; contains(fixedElements, val) || contains(disabledElements, val); ++val) {
	}
}

bool Hash::Iterators::next(ElementVector & v, std::size_t max, const ElementVector & fixedElements, const ElementVector & disabledElements) {
	std::size_t s = v.size() - 1;

	max = compute_max(max, fixedElements, disabledElements);

	while (v[s] == max) {
		// First two elements may be fixed to 0 and 1 for linear functions.
		if (s == fixedElements.size()) {
			return false;
		}

		--s;
		max = compute_max(max, fixedElements, disabledElements);
	}

	++v[s];
	fix_value(v[s], fixedElements, disabledElements);
	for (++s; s < v.size(); ++s) {
		v[s] = v[s - 1] + 1;
		fix_value(v[s], fixedElements, disabledElements);
	}

	return true;
}
