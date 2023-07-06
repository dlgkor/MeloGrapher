#ifndef __FFT_H__
#define __FFT_H__
#define M_PI 3.14159265

#include"AudioFile.h"
#include<vector>
#include<math.h>
#include<complex>
#include<gdiplus.h>

using namespace std;

typedef complex<double> cpx;

const cpx img(0.0, 1.0);

class vector2d {
public:
	float x, y;
public:
	vector2d() :x(0.0f), y(0.0f) {}
	vector2d(float _x, float _y) :x(_x), y(_y) {}

	float Size() {
		float size = sqrt((x * x) + (y * y));
		return size;
	}

	vector2d UnitVector() {
		//단위벡터를 반환한다
		return vector2d(x / Size(), y / Size());
	}

	vector2d operator+(const vector2d& a) {
		vector2d vec(x + a.x, y + a.y);
		return vec;
	}
	vector2d operator-(const vector2d& a) {
		vector2d vec(x - a.x, y - a.y);
		return vec;
	}
	vector2d operator*(const float& a) {
		vector2d vec(x * a, y * a);
		return vec;
	}
};

class SpectrumReadOption {
public:
	int Window;
	double baseFq; //관측한 파장의 주파수
};

class LineSpectrumSetting {
public:
	double minFq; //출력할 최소 주파수
	double maxFq; //출력할 최대 주파수
	int N; //막대 개수
	double Length; //막대 길이
	vector2d startp; //시작좌표

	double MaxHeight; //최대높이
};

class CircularSpectrumSetting {
public:
	double minFq; //출력할 최소 주파수
	double maxFq; //출력할 최대 주파수
	int N; //막대 개수

	vector2d center; //원 중심좌표
	double rad; //원 반지름

	double MaxHeight; //최대높이
};

class SpectrumBuffer {
public:
	vector<vector<cpx>> content;
	unsigned int startPoint;
	bool activated;
};

class PlayInform {
public:
	bool Play;
	bool readSpec;
	int SpectrumSlice;
	unsigned int SpectrumRead;
	vector<vector<cpx>> SpBuff;
	//SpectrumBuffer SpecBuff;
	TCHAR WavName[128];
};

void FFT(vector<cpx>& v, bool inv) {
	int S = v.size();

	for (int i = 1, j = 0; i < S; i++) {
		int bit = S / 2;

		while (j >= bit) {
			j -= bit;
			bit /= 2;
		}
		j += bit;

		if (i < j) swap(v[i], v[j]);
	}

	for (int k = 1; k < S; k *= 2) {
		double angle = (inv ? M_PI / k : -M_PI / k);
		cpx w(cos(angle), sin(angle));

		for (int i = 0; i < S; i += k * 2) {
			cpx z(1, 0);

			for (int j = 0; j < k; j++) {
				cpx even = v[i + j];
				cpx odd = v[i + j + k];

				v[i + j] = even + z * odd;
				v[i + j + k] = even - z * odd;

				z *= w;
			}
		}
	}

	if (inv)
		for (int i = 0; i < S; i++) v[i] /= S;
}

void HEllipse(HDC hdc, vector2d center, double rad) {
	Ellipse(hdc, center.x - rad, center.y - rad, center.x + rad, center.y + rad);
}


int PrintCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, vector<cpx>& output, SpectrumReadOption option, CircularSpectrumSetting setting) {
	Gdiplus::Pen* p_pen;
	Gdiplus::SolidBrush solidbrush1(Gdiplus::Color(255, 120, 193, 243));

	if (output.size() == 0)
		return 0;

	int maxFqpoint = (int)(setting.maxFq / option.baseFq);
	int minFqpoint = (int)(setting.minFq / option.baseFq);
	if (maxFqpoint == 0)
		return 0;

	int Cut = (maxFqpoint - minFqpoint) / setting.N;
	if (Cut == 0)
		return 0;

	double amp = 15.0;

	double* f = new double[setting.N];
	for (int i = 0; i < setting.N; i++) {
		f[i] = 0.0f;
	}

	int ic = 0;

	for (int i = 0; i < setting.N * Cut; i++) {
		ic = i / Cut;
		amp = 0.2;
		f[ic] = max(f[ic], sqrt(pow(output.at(i + minFqpoint).real(), 2) + pow(output.at(i + minFqpoint).imag(), 2)) * amp);

		if (ic > setting.MaxHeight) {
			f[ic] = setting.MaxHeight;
		}
		else if (f[ic] < 0) {
			f[ic] = 0;
		}
	}

	cpx W = exp((-2.0 * M_PI * img) / (double)setting.N);
	cpx Z(0, 1);
	vector2d p1, p2;
	vector2d tanline;

	p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 1);

	Gdiplus::Point p_point[500];

	for (int i = 0; i < setting.N; i++) {
		p1.x = setting.rad * Z.real(); p1.y = setting.rad * Z.imag();
		Z *= W;
		p2.x = setting.rad * Z.real(); p2.y = setting.rad * Z.imag();
		tanline = ((p1 + p2) * 0.5).UnitVector();

		Gdiplus::Point p_point[] = {
			Gdiplus::Point((INT)(setting.center.x + p1.x), -(INT)(setting.center.y + p1.y)),
			Gdiplus::Point((INT)(setting.center.x + p1.x + (f[i] * tanline.x)), -(INT)(setting.center.y + p1.y + (f[i] * tanline.y))),
			Gdiplus::Point((INT)(setting.center.x + p2.x + (f[i] * tanline.x)), -(INT)(setting.center.y + p2.y + (f[i] * tanline.y))),
			Gdiplus::Point((INT)(setting.center.x + p2.x), -(INT)(setting.center.y + p2.y))
		};
		p_graphic->FillPolygon(&solidbrush1, p_point, 4);
		p_graphic->DrawPolygon(p_pen, p_point, 4);
	}

	delete p_pen;

	delete[] f;
	return 1;
}

int PrintContinueCircularFrequencyWithGDI(Gdiplus::Graphics* p_graphic, vector<cpx>& output, SpectrumReadOption option, CircularSpectrumSetting setting) {
	Gdiplus::Pen* p_pen;

	if (output.size() == 0)
		return 0;

	int maxFqpoint = (int)(setting.maxFq / option.baseFq);
	int minFqpoint = (int)(setting.minFq / option.baseFq);
	if (maxFqpoint == 0)
		return 0;

	int Cut = (maxFqpoint - minFqpoint) / setting.N;
	if (Cut == 0)
		return 0;

	double amp = 10.0;

	double* f = new double[setting.N];
	for (int i = 0; i < setting.N; i++) {
		f[i] = 0.0f;
	}

	int ic = 0;

	for (int i = 0; i < setting.N * Cut; i++) {
		ic = i / Cut;

		amp = 0.2;
		f[ic] = max(f[ic], sqrt(pow(output.at(i + minFqpoint).real(), 2) + pow(output.at(i + minFqpoint).imag(), 2)) * amp);

		if (ic > setting.MaxHeight) {
			f[ic] = setting.MaxHeight;
		}
		else if (f[ic] < 0) {
			f[ic] = 0;
		}
	}

	cpx W = exp((-2.0 * M_PI * img) / (double)setting.N);
	cpx Z(0, -1);
	vector2d p1, p2;
	vector2d tanline;	

	Gdiplus::Point p_point[1000];

	for (int i = 0; i < setting.N; i++) {
		p1.x = setting.rad * Z.real(); p1.y = setting.rad * Z.imag();
		Z *= W;
		tanline = (p1.UnitVector());
		p_point[i] = Gdiplus::Point((INT)(setting.center.x + p1.x + (f[i] * tanline.x)), -(INT)(setting.center.y + p1.y + (f[i] * tanline.y)));
	}
	p_point[setting.N] = p_point[0];

	p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 255, 255), 2);
	p_graphic->DrawCurve(p_pen, p_point, setting.N + 1);
	delete p_pen;

	Gdiplus::SolidBrush solidbrush(Gdiplus::Color(255, 80, 188, 223));
	p_graphic->FillClosedCurve(&solidbrush, p_point, setting.N + 1);

	delete[] f;
	return 1;
}

/*
int PrintTriCircleWithGDI(Gdiplus::Graphics* p_graphic, CircularSpectrumSetting setting) {
	//원 그리는거 따로 뺐음.
	//다른 gdi 이용 함수들 수정필요. 1개빼고 전부.

	Gdiplus::Pen* p_pen;

	Gdiplus::SolidBrush solidbrush2(Gdiplus::Color(255, 255, 255, 255));
	HGDIFillEllipse(p_graphic, &solidbrush2, setting.center, setting.rad);
	HGDIFillEllipse(p_graphic, &solidbrush2, setting.center, setting.rad * 0.5);
	HGDIFillEllipse(p_graphic, &solidbrush2, setting.center, setting.rad * 0.25);

	p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 0.1);
	HGDIEllipse(p_graphic, p_pen, setting.center, setting.rad);
	HGDIEllipse(p_graphic, p_pen, setting.center, setting.rad * 0.5);
	HGDIEllipse(p_graphic, p_pen, setting.center, setting.rad * 0.25);
	delete p_pen;

	return 1;
}
*/

/*
int DrawSpectrumRing(Gdiplus::Graphics* p_graphic, CircularSpectrumSetting setting) {
	Gdiplus::Pen* p_pen;

	Gdiplus::SolidBrush solidbrush2(Gdiplus::Color(0, 0, 255, 0));
	HGDIFillEllipse(p_graphic, &solidbrush2, setting.center, setting.rad);

	p_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0), 2);
	HGDIEllipse(p_graphic, p_pen, setting.center, setting.rad);
	delete p_pen;

	return 1;
}
*/

int DrawSpectrumRing(HDC hdc, CircularSpectrumSetting setting) {
	HBRUSH hbrush, oldbrush;
	hbrush = CreateSolidBrush(RGB(255, 255, 255));

	HPEN HPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN oldPen;

	oldbrush = (HBRUSH)SelectObject(hdc, hbrush);
	oldPen = (HPEN)SelectObject(hdc, HPen);

	HEllipse(hdc, setting.center, setting.rad);

	DeleteObject(SelectObject(hdc, oldbrush));

	hbrush = CreateSolidBrush(RGB(0, 0, 255));
	oldbrush = (HBRUSH)SelectObject(hdc, hbrush);

	HEllipse(hdc, setting.center, setting.rad - 5);

	DeleteObject(SelectObject(hdc, oldbrush));
	DeleteObject(SelectObject(hdc, oldPen));
	//여기 수정했음
	return 1;
}



void ReadWavFile(AudioFile<double>& audioFile, const char* FILENAME) {
	audioFile.load(FILENAME);

	int sampleRate = audioFile.getSampleRate();
	int bitDepth = audioFile.getBitDepth();

	int numSamples = audioFile.getNumSamplesPerChannel();
	double lengthInSeconds = audioFile.getLengthInSeconds();

	int numChannels = audioFile.getNumChannels();
	bool isMono = audioFile.isMono();
	bool isStereo = audioFile.isStereo();

	return;
}
#endif // !__FFT_H__
