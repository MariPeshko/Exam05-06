#include "bigint.hpp"
#include <iostream>

static void negative_addition() {
	
	std::string divider(60, '-');
	std::cout << divider << std::endl;

	const bigint a(-1);
	bigint b(-2);
	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;

	bigint c = a + b;
	std::cout << "c = -3\nc = " << c << std::endl;

	std::cout << divider << std::endl;

}

int main()
{

	const bigint a(42);
	bigint b(21), c, d(1337), e(d);

	//std::cout << "(c <<= 10) = " << (c <<= 10) << std::endl;
	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "c = " << c << std::endl;
	std::cout << "d = " << d << std::endl;
	std::cout << "e = " << e << std::endl;

	std::cout << "a + b = " << a + b << std::endl;

	bigint f = a + b;
	std::cout << "f = " << f << std::endl;

	negative_addition();

	/* std::cout << "(c += a) = " << (c += a) << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "++b = " << ++b << std::endl;
	std::cout << "b++ = " << b++ << std::endl;
	std::cout << "(b << 10) + 42 = " << (b << 10) + 42 << std::endl;
	std::cout << "(d <<= 4) = " << (d <<= 4) << std::endl;
	std::cout << "(d >>= 2) = " << (d >>= (const bigint)2) << std::endl;
	std::cout << "a = " << a << std::endl;
	std::cout << "d = " << d << std::endl;
	std::cout << "(d < a) = " << (d < a) << std::endl;
	std::cout << "(d > a) = " << (d > a) << std::endl;
	std::cout << "(d == a) = " << (d == a) << std::endl;
	std::cout << "(d != a) = " << (d != a) << std::endl;
	std::cout << "(d <= a) = " << (d <= a) << std::endl;
	std::cout << "(d >= a) = " << (d >= a) << std::endl; */
}

/* // Створення великого числа через digit shifting
    // 99999999999999999999999999999999999999999... (200 цифр)
    bigint huge(9);
    huge <<= 199;  // зсув на 199 позицій: 9 + 199 нулів
    huge += (bigint(99999999) << 191);  // додаємо ще 9-ок
    huge += (bigint(99999999) << 183);  // і так далі...
    // ... або інший спосіб створення через операції
    
    std::cout << huge << std::endl;
    
    bigint one(1);
    std::cout << huge + one << std::endl;  // huge + 1
    std::cout << one - one << std::endl;  // 0 */
