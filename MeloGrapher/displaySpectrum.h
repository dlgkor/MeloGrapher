#pragma once
#include"CommonHeader.h"
#include"spectrumUtils.h"
#include"DataStruct.h"

namespace melo {
	void HEllipse(HDC hdc, vector2d center, double rad);

	int PrintCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, SpectrumBlock* spectrum_block, SpectrumOption option);
}