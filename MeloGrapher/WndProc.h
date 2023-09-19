#pragma once
#include"CommonHeader.h"
#include"meloData.h"

//wndproc function of root window
LRESULT CALLBACK WndProc_Root(HWND, UINT, WPARAM, LPARAM);

void PaintProc_Root(MeloGraphData* root_data);


//wndproc function of main window
LRESULT CALLBACK WndProc_Main(HWND, UINT, WPARAM, LPARAM);

//WM_PAINT
void PaintProc_Main(MeloMainData* main_data);



void PaintSpectrum(MeloWndData* common_data, Gdiplus::Graphics* p_graphic);

void PaintMenu(CustomUI::Menu* menu, Gdiplus::Graphics* p_graphic);


//WM_MOUSEMOVE
void MenuMouseMove(CustomUI::Menu* menu, vector2d changed_pos); //Update Menu

//WM_LBUTTONDOWN
void MenuMouseClick(CustomUI::Menu* menu, vector2d changed_pos); //Update Menu

//WM_LBUTTONUP
void MenuMouseRelease(CustomUI::Menu* menu, vector2d changed_pos);

//WM_COMMAND
void CommandProc(MeloMainData* main_data, WPARAM wParam);

void MainCommandProc(MeloMainData* main_data, WPARAM wParam);

int CommandOpenFile(MeloMainData* main_data);

char* tchar2char(TCHAR* unicode);