#include"WndProc.h"

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