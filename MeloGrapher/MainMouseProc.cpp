#include"WndProc.h"

void MenuMouseMove(CustomUI::Menu* menu, vector2d changed_pos) {
	menu->Update(vector2d(changed_pos.x, changed_pos.y), false);
}

void MenuMouseClick(CustomUI::Menu* menu, vector2d changed_pos) {
	
	menu->Update(vector2d(changed_pos.x, changed_pos.y), true);
}