#pragma once

#include"CommonHeader.h"
#include"spectrumUtils.h"
#include"DataStruct.h"

namespace melo {
	const cpx img(0.0, 1.0);

	void HEllipse(Gdiplus::Graphics* p_graphic, vector2d center, double rad);

	int PrintCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, SpectrumBlock* spectrum_block, SpectrumOption option);
}