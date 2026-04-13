#include <iostream>
#include <string>
#include <algorithm> // fo reverse

#ifndef BIGINT_HPP
# define BIGINT_HPP

class bigint {

private:
    std::string num;

public:

    bigint() : num("0") {}

    bigint(long long bignum) : num(std::to_string(bignum)) {};

	// Constructor with String
	/**
	 * std::string::find_first_not_of is a member function of 
	 * the std::string class. It searches the string for the 
	 * first character that does not match any of the characters 
	 * specified in its argument. It returns the index of this 
	 * first non-matching character.
	 * 
	 * std::string::npos is a special constant value that is returned 
	 * by string search functions (like find_first_not_of) when the 
	 * character or substring being searched for is not found.
	 */
	bigint(const std::string &bignum) {
		if (bignum.empty()) {
            num = "0";
            return;
        }
        size_t first_digit = bignum.find_first_not_of('0');
        if (std::string::npos == first_digit) {
            num = "0"; // all zeros
        } else {
            num = bignum.substr(first_digit);
        }
	}; 
    // Copy and assignment. ??? Do I need them ???

    // returns arithmetic addition of str1+str2
    // Extract digits as chars → convert to int → add → convert back to char
    std::string addstr(const std::string &a, const std::string &b) const {

        int sum = 0;
        std::string result = "";
        int carry = 0;
        int i = a.size() - 1;
        int j = b.size() - 1;
		while (i >= 0 || j >= 0 || carry > 0) {
            if (i >= 0)
				sum += a[i--] - '0';
            if (j >= 0)
				sum += b[j--] - '0';
            sum += carry;
            result.push_back((sum % 10) + '0');
            carry = sum / 10;
			sum = 0;
        }
		std::reverse(result.begin(), result.end());
        return result;
    }

    // Addition. Creates a new object and returns a value
    // The function is const, so it can't modify *this
    bigint operator+(const bigint &b) const {
        return(bigint(addstr(num, b.num)));
    }

	bigint& operator+=(const bigint &b) {
		this->num = addstr(this->num, b.num);
		return *this;
	}

	// ++b pre-increament
    bigint& operator++() {
        num = addstr(num, "1");
        return *this;
    }
    
	// b++ post-increament
    bigint operator++(int) {
		bigint tmp = *this;
        num = addstr(num, "1");
        return tmp;
    }
    // <, >, ==, !=, <=, >=
    bool	operator<(const bigint &b) const {
		if (num.size() < b.num.size())
			return true;
		else if (num.size() > b.num.size())
			return false;
		else if (num < b.num)
			return true;
		return false;
	}

	// this operator can use operator above!
	bool	operator>(const bigint &b) const {
		if (b < *this)
			return true;
		else
			return false;
	}

	bool	operator<=(const bigint &b) const {
		if (*this > b)
			return false;
		else
			return true;
	}

	bool	operator>=(const bigint &b) const {
		if (b > *this)
			return false;
		return true;
	}

	bool	operator==(const bigint &b) const {	return this->num == b.num; }

	bool	operator!=(const bigint &b) const {	return this->num != b.num; }

	bigint	operator-(const bigint &b) {
		(void)b;
		return bigint(0);
	}

	// Append/remove zeros for left/right shifts (<<, >>, <<=, >>=) 
	bigint	operator<<(size_t shift) const {
		return bigint(num + std::string(shift, '0'));
	}

	bigint	operator<<(const bigint &b) const {
		bigint result;
		result.num = num;
		if(num != "0") {
			bigint	tmp(0);
			while (tmp < b) {
				result.num = result.num + std::string(1, '0');
				tmp++;
			}
		}
		return result;
	}

	bigint& operator<<=(const bigint &b) {
		if(num != "0") {
			bigint	tmp(0);
			while (tmp < b) {
				num = num + std::string(1, '0');
				tmp++;
			}
		}
		return *this;
	}

	bigint operator>>(const bigint &b) const {
		bigint result;
		if (b >= bigint(num.size()))
			result.num = "0";
		else {
			result.num = num;
			bigint tmp(0);
			while (tmp < b) {
				result.num = result.num.substr(0, result.num.size() - 1);
				tmp++;
			}
		}
		return result;
	}

	// value of b
	bigint& operator>>=(const bigint &b) {
		if (b >= bigint(num.size()))
			num = "0";
		else {
			bigint i(0);
			while (i < b) 
			{
				num = num.substr(0, num.size() - 1);
				i++;
			}
		}
		return *this;
	}
    
	friend std::ostream &operator<<(std::ostream &os, const bigint &bi);

};

#endif


