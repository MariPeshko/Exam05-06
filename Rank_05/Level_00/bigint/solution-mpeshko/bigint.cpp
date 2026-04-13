#include "bigint.hpp"

// as a friend function, it can access the private members of the class like digits
std::ostream &operator<<(std::ostream &os, const bigint &bi) {
	os << bi.num;
	return os;
}
