#include"CustomUI.h"

Gdiplus::Color CustomUI::ConvertColorRefToGdiPlusColor(COLORREF colorRef)
{
	BYTE r = GetRValue(colorRef);
	BYTE g = GetGValue(colorRef);
	BYTE b = GetBValue(colorRef);
	return Gdiplus::Color(r, g, b);
}