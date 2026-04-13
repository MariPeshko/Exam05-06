#pragma once
#include <iostream>

class vect2 {
private:
	int x, y;

public:
	vect2() : x(0), y(0) {}
	vect2(const int x_input, const int y_input) : x(x_input), y(y_input) {}
	vect2(const vect2& oth) : x(oth.x), y(oth.y) {}
	~vect2(void) {}
	vect2& operator=(const vect2& oth) {
		if (this != &oth) {
			x = oth.x;
			y = oth.y;
		}
		return *this;
	}
	int& operator[](const size_t i) {
		if (i == 0)
			return x;
		return y;
	}
	int operator[](const size_t i) const {
		if (i == 0)
			return x;
		return y;
	}
	vect2 operator+(const vect2& rh) const {
		return vect2(x + rh.x, y + rh.y);
	}
	vect2& operator+=(const vect2& rh) {
		x += rh.x;
		y += rh.y;
		return *this;
	}
	vect2 operator-(const vect2& rh) const {
		return vect2(x - rh.x, y - rh.y);
	}
	vect2& operator-=(const vect2& rh) {
		x -= rh.x;
		y -= rh.y;
		return *this;
	}
	vect2 operator*(const int n) const {
		return vect2(x * n, y * n);
	}
	vect2& operator*=(const int n) {
		x *= n;
		y *= n;
		return *this;
	}
	vect2& operator++() { x++; y++; return *this; }
	vect2 operator++(int) {
		vect2 tmp(*this);
		x += 1;
		y += 1;
		return tmp;
	}
	vect2& operator--() {
		x--;
		y--;
		return *this;
	}
	vect2 operator--(int) {
		vect2 tmp(*this);
		x--;
		y--;
		return tmp;
	}
	vect2 operator-() { return vect2(-x, -y); }
	bool operator==(const vect2& rh) {
		return (x == rh.x && y == rh.y);
	}
	bool operator!=(const vect2& rh) {
		return !(x == rh.x && y == rh.y);
	}
	friend std::ostream& operator<<(std::ostream& os, const vect2& v);
	friend vect2 operator*(const int n, const vect2& v);
};

