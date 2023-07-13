#include"CustomUI.h"

void CustomUI::Plane::UpdateNormal() {
	matrix2x2 rotation;
	rotation.m[0][0] = 0;
	rotation.m[0][1] = 1;
	rotation.m[1][0] = -1;
	rotation.m[1][1] = 0;

	for (int i = 0; i < 4; i++) {
		p_normal[i] = ((p[(i + 1) % 4] - p[i]) * rotation).UnitVector();
	}

}
bool CustomUI::Plane::IsInsideLine(vector2d normalv, vector2d point) {
	if (normalv.dot(point) < 0)
		return true;

	return false;
}
bool CustomUI::Plane::Detect(vector2d mousepos) {
	UpdateNormal();

	for (int i = 0; i < 4; i++) {
		vector2d mousev = mousepos - p[i];
		if (!IsInsideLine(p_normal[i], mousev)) {
			return false;
		}
	}
	return true;
}
void CustomUI::Plane::Render(HDC hdc) {
	HBRUSH HBrush = CreateSolidBrush(color);
	HBRUSH OldBrush;

	HPEN HPen = CreatePen(PS_NULL, 1, RGB(0, 0, 0));
	HPEN OldPen;

	OldBrush = (HBRUSH)SelectObject(hdc, HBrush);
	OldPen = (HPEN)SelectObject(hdc, HPen);

	POINT points[4];
	for (int i = 0; i < 4; i++) {
		points[i].x = p[i].x;
		points[i].y = p[i].y;
	}
	Polygon(hdc, points, 4);

	DeleteObject(SelectObject(hdc, OldBrush));
	DeleteObject(SelectObject(hdc, OldPen));
}