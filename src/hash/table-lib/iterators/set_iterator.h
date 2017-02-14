#ifndef SET_ITERATOR_H
#define SET_ITERATOR_H

#include <ostream>
#include <vector>

namespace Hash { namespace Iterators {

typedef std::vector<std::size_t> ElementVector;

/**
 * Checks if the elements contains element.
 *
 * @param element Element to be sought in elements.
 * @param elements The vector to be sought.
 * @return The search result.
 */
bool contains(const ElementVector & elements, std::size_t element);

/**
 * The lexicographically first set confirming to the given parameters.
 */
ElementVector first(std::size_t count, const ElementVector & fixedElements, const ElementVector & disabledElements);

/**
 * Next set in lexicographic order confirming to the given parameters.
 *
 * @return False iff there is no such set.
 */
bool next(ElementVector & v, std::size_t max, const ElementVector & fixedElements, const ElementVector & disabledElements);

} }

namespace std {

/**
 * Prints the vector to the given output stream.
 *
 * @param out The output stream with the contents of the vector.
 * @param v The printed vector.
 * @return The output stream, after printing.
 */
std::ostream & operator <<(std::ostream & out, const Hash::Iterators::ElementVector & v);

}

#endif
