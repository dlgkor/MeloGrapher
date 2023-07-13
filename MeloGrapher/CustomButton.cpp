#include"CustomUI.h"

CustomUI::Button::Button() {	}

CustomUI::Button::Button(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
	COLORREF _default, COLORREF _highlight, HWND _parent, int _id) {
	p1 = _p1;
	p2 = _p2;
	lstrcpy(Btext, _Btext);
	Active = false;
	Highlight = false;

	defaultColor = _default;
	highlightColor = _highlight;
	parent = _parent;

	id = _id; //number to discriminate button
}

void CustomUI::Button::Set(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
	COLORREF _default, COLORREF _highlight, HWND _parent, int _id) {
	p1 = _p1;
	p2 = _p2;
	lstrcpy(Btext, _Btext);
	Active = false;
	Highlight = false;

	defaultColor = _default;
	highlightColor = _highlight;
	parent = _parent;

	id = _id; //number to discriminate button
}

void CustomUI::Button::SetActive(bool active) {
	Active = active;
}

bool CustomUI::Button::DetectMouse(vector2d mousepos) {
	if (mousepos.x<p1.x || mousepos.x>p2.x)
		return false;
	if (mousepos.y<p1.y || mousepos.y>p2.y)
		return false;
	return true;
}

void CustomUI::Button::isHighlight(vector2d mousepos) {
	if (!Active)
		return;

	if (DetectMouse(mousepos))
		Highlight = true;
	else
		Highlight = false;

	return;
}

void CustomUI::Button::Click(vector2d mousepos) {
	if (!Active)
		return;

	if (!DetectMouse(mousepos))
		return;

	//Send Custom WindowMessage
	SendMessage(parent, WM_COMMAND, MAKEWORD(id, 0), MAKEWORD(0, 0));
}


void CustomUI::Button::Render(HDC hdc) {
	if (!Active)
		return;

	HBRUSH HBrush, OldBrush;
	//Fill Rectangle With Color
	//Color is selected by presence of highlight
	if (Highlight) {
		HBrush = CreateSolidBrush(highlightColor);
	}
	else {
		HBrush = CreateSolidBrush(defaultColor);
	}

	OldBrush = (HBRUSH)SelectObject(hdc, HBrush);

	Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);

	SetTextAlign(hdc, TA_CENTER);
	SetBkMode(hdc, TRANSPARENT);

	//Todo : Set Text standard to middle of text
	TextOut(hdc, (p1.x + p2.x) / 2, (p1.y + p2.y) / 2, Btext, lstrlen(Btext));

	DeleteObject(SelectObject(hdc, OldBrush));
}