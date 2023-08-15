#include"WndProc.h"

LRESULT NchittestProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, CustomUI::Menu* menu, vector2d changed_pos) {
	//function not working
	LRESULT hittest = DefWindowProc(hWnd, iMessage, wParam, lParam);
	if (menu->DetectMouse(changed_pos) == CustomUI::DEFUALT)
		return HTNOWHERE;
	else
		return hittest;
}

void MenuMouseMove(CustomUI::Menu* menu, vector2d changed_pos) {
	//menu->Update(vector2d(changed_pos.x, changed_pos.y), false);
	menu->update_mousemove(changed_pos);
}

void MenuMouseClick(CustomUI::Menu* menu, vector2d changed_pos) {
	//menu->Update(vector2d(changed_pos.x, changed_pos.y), true);
	menu->update_mousedown(changed_pos);
}

void MenuMouseRelease(CustomUI::Menu* menu, vector2d changed_pos) {
	menu->update_mouseup(changed_pos);
}