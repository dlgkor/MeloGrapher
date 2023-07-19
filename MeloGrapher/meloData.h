#pragma once
#include"CommonHeader.h"
#include"BlockWrapper.h"
#include"displaySpectrum.h"
#include"CustomWindow.h"
#include"CustomUI.h"

struct MeloRootWndData {
	BlockWrapper* block_wrapper;
	//no calculation
	//just check child window state and display
};

struct MeloWndData {
	HWND root_hwnd; //use to post destroy message to root when child is distroyed
	BlockWrapper* block_wrapper;
	CustomWindow* this_window;
	melo::SpectrumOption spectrum_option;
	CustomUI::Menu custom_menu;
};