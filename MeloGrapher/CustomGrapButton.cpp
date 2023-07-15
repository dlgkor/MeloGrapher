#include"CustomUI.h"

CustomUI::GrapButton::GrapButton() : Button() {

}

void CustomUI::GrapButton::Set(vector2d _p1, vector2d _p2,
	COLORREF _default, COLORREF _highlight, HWND _parent, int _id) {
	p1 = _p1;
	p2 = _p2;
	Active = false;
	Highlight = false;

	defaultColor = _default;
	highlightColor = _highlight;
	parent = _parent;

	id = _id; //number to discriminate button

	grapped = false;
}

void CustomUI::GrapButton::Move(vector2d mousepos) {
	if (!Active)
		return;

	if (DetectMouse(mousepos))
		Highlight = true;
	else
		Highlight = false;

	if (grapped) {
		//Send Command and Update location to current mousepos
		SendMessage(parent, WM_COMMAND, MAKEWORD(id, 0), MAKEWORD(0, 0));
	}

	return;
}

void CustomUI::GrapButton::Grap(vector2d mousepos) {
	if (!Active)
		return;

	if (!DetectMouse(mousepos))
		return;

	//Send Custom WindowMessage
	grapped = true;
	SendMessage(parent, WM_COMMAND, MAKEWORD(id, 0), MAKEWORD(0, 0));
}

void CustomUI::GrapButton::Release(vector2d mousepos) {
	if (!Active)
		return;

	if (grapped) {
		grapped = false;
		SendMessage(parent, WM_COMMAND, MAKEWORD(id, 0), MAKEWORD(0, 0));
	}
}


void CustomUI::GrapButton::Render_GrapButton(Gdiplus::Graphics* p_graphic) {
	if (!Active)
		return;

	Gdiplus::SolidBrush solidbrush(Gdiplus::Color(0, 0, 0));
	Gdiplus::Pen solidpen(Gdiplus::Color(0, 0, 0), 1);

	//Fill Rectangle With Color
	//Color is selected by presence of highlight
	if (Highlight) {
		solidbrush.SetColor(Gdiplus::Color(ConvertColorRefToGdiPlusColor(highlightColor)));
	}
	else {
		solidbrush.SetColor(Gdiplus::Color(ConvertColorRefToGdiPlusColor(defaultColor)));
	}

	Gdiplus::Rect crt(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
	p_graphic->FillRectangle(&solidbrush, crt);
	p_graphic->DrawRectangle(&solidpen, crt);
}