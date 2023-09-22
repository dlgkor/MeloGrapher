#pragma once
#include"CommonHeader.h"
#include"BlockWrapper.h"
#include"displaySpectrum.h"
#include"CustomWindow.h"
#include"CustomUI.h"
#include"YoutubeParser.h"

struct MeloWndData {
	HINSTANCE hInstance;
	BlockWrapper* block_wrapper;
	melo::SpectrumOption spectrum_option;
};

struct MeloRootData {
	MeloWndData* common_data;
	CustomWindow* this_window;
};

struct MeloMainData {
	MeloWndData* common_data;
	CustomWindow* this_window;
	CustomUI::Menu custom_menu;
	youtube_parser_ui ytparser_ui;
	//HWND root_hwnd;
};

struct MeloGraphData {
	MeloWndData* common_data;
	CustomWindow* this_window;
	HWND root_hwnd;
};