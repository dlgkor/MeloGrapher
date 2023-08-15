#pragma once
#include"CommonHeader.h"
#include"meloData.h"

//wndproc function of root window
LRESULT CALLBACK WndProc_Root(HWND, UINT, WPARAM, LPARAM);




//wndproc function of main window
LRESULT CALLBACK WndProc_Main(HWND, UINT, WPARAM, LPARAM);

//WM_PAINT
void PaintProc(MeloWndData* main_data);

void PaintSpectrum(MeloWndData* main_data, Gdiplus::Graphics* p_graphic);

void PaintMenu(CustomUI::Menu* menu, Gdiplus::Graphics* p_graphic);


//WM_MOUSEMOVE
void MenuMouseMove(CustomUI::Menu* menu, vector2d changed_pos); //Update Menu

//WM_LBUTTONDOWN
void MenuMouseClick(CustomUI::Menu* menu, vector2d changed_pos); //Update Menu

//WM_LBUTTONUP
void MenuMouseRelease(CustomUI::Menu* menu, vector2d changed_pos);

//WM_COMMAND
void CommandProc(MeloWndData* main_data, WPARAM wParam);

int CommandOpenFile(MeloWndData* main_data);

char* tchar2char(TCHAR* unicode);

void MoveWindow(MeloWndData* main_data);