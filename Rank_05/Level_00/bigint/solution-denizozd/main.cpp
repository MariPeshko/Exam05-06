#include "bigint.hpp"
#include <iostream>

int main()
{

	const bigint a(42);
	bigint b(21), c, d(1337), e(d);

	// copy assignment operator
	//c = a;

	std::cout << "(c << 10) = " << (c << 10) << std::endl;
	std::cout << "(c <<= 10) = " << (c <<= 10) << std::endl;
	//std::cout << "(b <<= 10) = " << (b <<= 10) << std::endl;

	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "c = " << c << std::endl;
	std::cout << "d = " << d << std::endl;
	std::cout << "e = " << e << std::endl;

	std::cout << "a + b = " << a + b << std::endl;

	bigint f = a + b;
	std::cout << "f = " << f << std::endl;

	std::cout << "(c += a) = " << (c += a) << std::endl;

	std::cout << "b = " << b << std::endl;
	std::cout << "++b = " << ++b << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "b++ = " << b++ << std::endl;
	std::cout << "b = " << b << std::endl;

	std::cout << d << " < " << a << " = " << std::boolalpha << (d < a) << std::endl;
	// same number
	std::cout << d << " < " << e << " = " << (d < e) << std::endl;
	std::cout << d << " > " << e << " = " << (d > e) << std::endl;
	std::cout << a << " < " << d << " = " << (a < d) << std::endl;

	std::cout << d << " > " << a << " = " << (d > a) << std::endl;
	std::cout << a << " > " << d << " = " << (a > d) << std::endl;

	std::cout << d << " == " << a << " = " << (d == a) << std::endl;
	std::cout << d << " == " << e << " = " << (d == e) << std::endl;

	std::cout << d << " != " << a << " = " << (d != a) << std::endl;
	std::cout << d << " != " << d << " = " << (d != d) << std::endl;
	
	std::cout << d << " <= " << a << " = " << (d <= a) << std::endl;
	std::cout << d << " <= " << d << " = " << (d <= d) << std::endl;
	
	std::cout << d << " >= " << a << " = " << (d >= a) << std::endl;
	std::cout << a << " >= " << d << " = " << (a >= d) << std::endl;
	std::cout << d << " >= " << e << " = " << (d >= e) << std::endl;

	std::cout << "b = " << b << std::endl;
	std::cout << "(b << 10) + 42 = " << (b << 10) + 42 << std::endl;
	std::cout << "(b << (const bigint)1) = " << (b << (const bigint)1) << std::endl;
	std::cout << "b = " << b << std::endl;

	std::cout << "d = " << d << std::endl;
	std::cout << "(d <<= 4) = " << (d <<= 4) << std::endl;
	std::cout << "(d <<= 4) = " << (d <<= (const bigint)4) << std::endl;

	std::cout << "(d >>= 2) = " << (d >>= (const bigint)2) << std::endl;
	std::cout << "a = " << a << std::endl;
	std::cout << "d = " << d << std::endl;
	std::cout << "(d >> 6) = " << (d >> (const bigint)6) << std::endl;
	std::cout << "d = " << d << std::endl;

	bigint g(43);
	std::cout << g - g << std::endl;

	// Створення великого числа через digit shifting
    // 99999999999999999999999999999999999999999... (200 цифр)
    bigint huge(9);
    huge <<= 199;  // зсув на 199 позицій: 9 + 199 нулів
    huge += (bigint(99999999) << 191);  // додаємо ще 9-ок
    huge += (bigint(99999999) << 183);  // і так далі...
    // ... або інший спосіб створення через операції
    
    std::cout << huge << std::endl;
    
    bigint one(1);
    std::cout << huge + one << std::endl;  // huge + 1

}


