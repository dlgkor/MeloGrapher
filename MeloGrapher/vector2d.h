#pragma once
#include<math.h>

class matrix2x2 {
public:
	float m[2][2];
};

class vector2d {
public:
	double x, y;
public:
	vector2d();
	vector2d(double _x, double _y);

	double Size();

	vector2d UnitVector();

	vector2d operator+(const vector2d& a);
	vector2d operator-(const vector2d& a);
	vector2d operator*(const float& a);

	vector2d operator*(const matrix2x2& matrix) const;
	float dot(const vector2d& a);
};