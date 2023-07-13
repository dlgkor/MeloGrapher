#include"vector2d.h"

vector2d::vector2d() :x(0.0f), y(0.0f) {}
vector2d::vector2d(double _x, double _y) :x(_x), y(_y) {}

double vector2d::Size() {
	double size = sqrt((x * x) + (y * y));
	return size;
}

vector2d vector2d::UnitVector() {
	//단위벡터를 반환한다
	return vector2d(x / Size(), y / Size());
}

vector2d vector2d::operator+(const vector2d& a) {
	vector2d vec(x + a.x, y + a.y);
	return vec;
}
vector2d vector2d::operator-(const vector2d& a) {
	vector2d vec(x - a.x, y - a.y);
	return vec;
}
vector2d vector2d::operator*(const float& a) {
	vector2d vec(x * a, y * a);
	return vec;
}