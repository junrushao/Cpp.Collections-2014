
/** @file */

#ifndef __UTILITY_H
#define __UTILITY_H

#include <sstream>
#include <string>

template <class T>
inline std::string toString(const T &a) {
	std::ostringstream os;
	os << a;
	return os.str();
}

#endif /* __UTILITY_H */
