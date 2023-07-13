#pragma once
#include"CommonHeader.h"
#include"meloData.h"

LRESULT CALLBACK WndProc_Root(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc_Main(HWND, UINT, WPARAM, LPARAM);

void paintFunction(MeloWndData* main_data);