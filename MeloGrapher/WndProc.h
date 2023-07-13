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