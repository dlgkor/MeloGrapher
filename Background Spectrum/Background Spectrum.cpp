#pragma comment(lib, "winmm.lib")
#include<windows.h>
#include<mmsystem.h>
#include<tchar.h>
#include<thread>

#include<gdiplus.h>
#pragma comment(lib, "gdiplus")

#include"FFT.h"
#include"Background Spectrum.h"
#include"NoiseMaker.h"
#include"CustomUI.h"


using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("Background Spectrum Window");
OPENFILENAME OFN;
const UINT nFileNameMaxLen = 512;// 다음 줄에 정의하는 szFileName 문자열의 최대 길이
WCHAR szFileName[nFileNameMaxLen];

HBITMAP hbit;

//Edit
enum
{
	ID_E1 = 201
};
//Button
enum {
	ID_B1 = 401, ID_B2, ID_B3, ID_B4
};

int windowsizeX, windowsizeY;
int countH = 1;

char* tchar2char(TCHAR* unicode)
{
	char* szRet = NULL;
	int len = ::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, 0, NULL, NULL);

	if (0 < len)
	{
		szRet = (char*)malloc(sizeof(char) * len);
		::ZeroMemory(szRet, len);

		::WideCharToMultiByte(CP_ACP, 0, unicode, -1, szRet, len, NULL, NULL);
	}

	return szRet;
}

void DrawBitmap(HDC hdc, int x, int y, HBITMAP hBit) {
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx, by;
	BITMAP bit;

	MemDC = CreateCompatibleDC(hdc);
	OldBitmap = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	BitBlt(hdc, x, y, bx, by, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap);
	DeleteDC(MemDC);
}

void SetAttr(HWND hWnd, int iOpa) {
	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 255), iOpa, LWA_ALPHA | LWA_COLORKEY);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {

	ULONG_PTR gpToken;
	GdiplusStartupInput gpsi;

	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Ok) return 0;

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWndMain = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWndMain, SW_HIDE);

	//taskbar에서 아이콘 안보이게 하는 작업
	//숨겨진 윈도우 생성 -> 실제 프로그램 윈도우를 숨겨진 윈도우의 자식 윈도우로 설정
	
	//최소화를 금지시키는 작업이 필요하다
	int scx = GetSystemMetrics(SM_CXSCREEN);
	int scy = GetSystemMetrics(SM_CYSCREEN);

	windowsizeX = 700; windowsizeY = 700;

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		(scx-windowsizeX)/2, (scy-windowsizeY)/2, windowsizeX, windowsizeY,
		hWndMain, (HMENU)NULL, hInstance, NULL);
	
	//SetWindowLong(hWnd, GWL_STYLE, 0); //테두리를 없애준다
	SetWindowLongPtr(hWnd, GWL_STYLE, 0); //테두리를 없애준다

	ShowWindow(hWnd, nCmdShow);

	/*--------------------*/

	//SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); //항상 레이어 최하단에 배치

	/*--------------------*/

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	GdiplusShutdown(gpToken);
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	static AudioFile<double> audioFile;

	static SpectrumReadOption option;
	static LineSpectrumSetting line_setting[5];
	static CircularSpectrumSetting circle_setting[5];

	static PlayInform PInfo;

	HDC hdc, hMemDC;
	PAINTSTRUCT ps;
	static RECT crt;
	HBITMAP OldBit;
	HBRUSH dbrush;

	Graphics* p_graphic; //Using GDI+

	static TCHAR stre1[128];

	static W_Sound MyWavSound;
	
	static double* buffer;

	int spread, threadread;

	std::thread* FFTthread;

	int middlex;
	int middley;

	static CustomUI::Menu menu;
	static vector2d ChangedMousepos;

	switch (iMessage) {
	case WM_CREATE:
		if (countH == 1) {
			countH--;
			return 0;
		}

		//GetClientRect(hWnd, &crt); //윈도우 크기 가지고오기
		crt.right = windowsizeX;
		crt.bottom = windowsizeY;
		middlex = crt.right / 2;
		middley = crt.bottom / 2;


		SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetAttr(hWnd, 255);

		buffer = new double[MyWavSound.GetBlockSamples()];

		PInfo.Play = false;
		PInfo.readSpec = false;
		PInfo.SpectrumSlice = 32768 / 16; //16

		option.Window = 32768 / 4;
		option.baseFq = (double)audioFile.getSampleRate() / (double)option.Window;

		circle_setting[0].maxFq = 8000; //1600
		circle_setting[0].minFq = 1;
		circle_setting[0].MaxHeight = 1000;
		circle_setting[0].N = 100; //150
		circle_setting[0].center = { -80, 0 };
		circle_setting[0].rad = 50;

		circle_setting[1].maxFq = 1000; //1600
		circle_setting[1].minFq = 10;
		circle_setting[1].MaxHeight = 400;
		circle_setting[1].N = 100; //100
		circle_setting[1].center = { 0, 0 };
		circle_setting[1].rad = 200;

		line_setting[0].Length = 1;
		line_setting[0].maxFq = 16000; //1600
		line_setting[0].minFq = 5000;
		line_setting[0].MaxHeight = 5000;
		line_setting[0].N = 150; //150
		line_setting[0].startp = { 0, 30 };

		line_setting[1].Length = 1;
		line_setting[1].maxFq = 24000; //1600
		line_setting[1].minFq = 16000;
		line_setting[1].MaxHeight = 5000;
		line_setting[1].N = 150; //150
		line_setting[1].startp = { 0, -30 };
		

		MyWavSound.OpenDevice(hWnd);

		menu.ResetMenu(hWnd);

		SetTimer(hWnd, 1, 10, NULL);
		return 0;
	case MM_WOM_DONE:
		if (!PInfo.Play)
			return 0;
		memcpy(buffer, &audioFile.samples[0][MyWavSound.GetCurrentSampleNum()], MyWavSound.GetBlockSamples()*sizeof(double));
		MyWavSound.PrintWave(buffer);
		if (MyWavSound.GetCurrentSampleNum() + MyWavSound.GetBlockSamples() >= audioFile.getNumSamplesPerChannel()) {
			PInfo.Play = false;
			MyWavSound.StopDevice();
			MyWavSound.ResetCurrentSampleNum();
			break;
		}
		return 0;
	case WM_MOUSEMOVE:
		ChangedMousepos.x = (float)(LOWORD(lParam) - (crt.right / 2));
		ChangedMousepos.y = (float)(HIWORD(lParam) - (crt.bottom / 2));
		menu.Update(CustomUI::vector2d(ChangedMousepos.x, ChangedMousepos.y), false);
		return 0;
	case WM_LBUTTONDOWN:
		ChangedMousepos.x = (float)(LOWORD(lParam) - (crt.right / 2));
		ChangedMousepos.y = (float)(HIWORD(lParam) - (crt.bottom / 2));
		menu.Update(CustomUI::vector2d(ChangedMousepos.x, ChangedMousepos.y), true);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 10:
			PInfo.Play = false; //원래 플레이 하던거 멈춤. 플레이 버튼 눌러야 플레이되게 바꿈.
			PInfo.readSpec = false;
			MyWavSound.StopDevice();
			MyWavSound.ResetCurrentSampleNum();

			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = L"웨이브 파일\0*.wav";
			OFN.lpstrFile = szFileName;
			OFN.nMaxFile = nFileNameMaxLen;

			if (0 == GetOpenFileName(&OFN)) {
				return 0;
			}

			audioFile.load(tchar2char(szFileName));
			if (audioFile.getNumSamplesPerChannel() == 0)
			{
				MessageBox(hWnd, TEXT("읽어들이는데 실패했습니다!"), TEXT("알림"), MB_OK);
				break;
			}
			MessageBox(hWnd, TEXT("성공적으로 읽었습니다!"), TEXT("알림"), MB_OK);

			_tcscpy_s(PInfo.WavName, _tcslen(stre1) + 1, stre1);
			for (int i = 0; i < PInfo.SpBuff.size(); i++) {
				PInfo.SpBuff[i].clear();
			}
			PInfo.SpBuff.clear();
			for (int i = 0; i <= audioFile.getNumSamplesPerChannel() / PInfo.SpectrumSlice; i++) {
				vector<cpx> cpbuf;
				for (int j = 0; j < option.Window; j++) {
					cpbuf.push_back(cpx(0, 0));
				}
				PInfo.SpBuff.push_back(cpbuf);
			}
			PInfo.SpectrumRead = 0;
			PInfo.readSpec = true;
			SetTimer(hWnd, 0, 20, NULL);
			break;
		case 11:
			if (audioFile.getNumSamplesPerChannel() == 0)
				break;

			if (!PInfo.Play) {
				PInfo.Play = true;
				//MyWavSound.ResetCurrentSampleNum(); //현재 출력 중인 위치를 0으로 초기화
				//위에걸, 읽어들였을 때랑 MM_WOM_DONE으로 옮기면 되지 않을까? 

				memset(buffer, 0, sizeof(buffer));
				for (int i = 0; i < MyWavSound.GetBlockN(); i++) {
					PostMessage(hWnd, MM_WOM_DONE, NULL, NULL);
				}
				MyWavSound.StartDevice();
			}
			else{
				PInfo.Play = false;
				MyWavSound.StopDevice();
			}
			break;
		case 13:
			DestroyWindow(hWnd);
			break;
		default:
			break;
		}
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 0:
			if (!PInfo.readSpec)
				break;

			FFTthread = new thread[std::thread::hardware_concurrency()];

			//더블버퍼링을 통해 메모리 최적화
			//스펙트럼 위치계산도 조정해줘야

			threadread = 0;

			for (int j = 0; j < std::thread::hardware_concurrency(); j++) {
				spread = PInfo.SpectrumRead;

				if (spread * PInfo.SpectrumSlice + option.Window > audioFile.getNumSamplesPerChannel()) {
					PInfo.readSpec = false;
					break;
				}
				PInfo.SpectrumRead++;

				for (int i = 0; i < option.Window; i++) {
					PInfo.SpBuff[spread][i] = cpx(audioFile.samples[0][(spread * PInfo.SpectrumSlice) + i], 0.0);
				}

				FFTthread[j] = std::thread(FFT, std::ref(PInfo.SpBuff[spread]), false);

				threadread = j + 1;
			}

			for (int j = 0; j < threadread; j++) {
				FFTthread[j].join();
			}

			delete[] FFTthread;
			break;
		case 1:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps); //DC생성

		if (hbit == NULL) {
			hbit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
		}
		hMemDC = CreateCompatibleDC(hdc); //hdc와 호환되는 dc 생성
		OldBit = (HBITMAP)SelectObject(hMemDC, hbit); //hbit을 hmemdc에 선택
		
		dbrush = CreateSolidBrush(RGB(0, 0, 255));
		FillRect(hMemDC, &crt, dbrush);
		DeleteObject(dbrush);

		SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //원점을 중간으로 설정
		//SetMapMode(hMemDC, MM_LOENGLISH); //평범한 xy 좌표계로 맵핑모드 설정
		
		p_graphic = new Graphics(hMemDC);
		p_graphic->SetSmoothingMode(SmoothingModeAntiAlias);
		
		if (audioFile.getNumSamplesPerChannel() != 0) {
			if (PInfo.Play) {
				//PrintContinueCircularFrequencyWithGDI(p_graphic, PInfo.SpBuff[(int)(MyWavSound.GetCurrentSampleNum() / PInfo.SpectrumSlice)], option, circle_setting[1]);
				PrintCircularFrequencyWithGDI(p_graphic, PInfo.SpBuff[(int)(MyWavSound.GetCurrentSampleNum() / PInfo.SpectrumSlice)], option, circle_setting[1]);
			}
		}

		//애초에 불연속적이다!
		//전환과정에서 애니메이션을 (df/dt)의 속도로 넣어서 자연스럽게, 연속적으로 변하는 것처럼 보이게 하자

		delete p_graphic;

		if (PInfo.Play) {
			DrawSpectrumRing(hMemDC, circle_setting[1]);
		}

		menu.Render(hMemDC, 0.02);
		SelectObject(hMemDC, OldBit);
		//DeleteObject(hbit);
		DeleteDC(hMemDC);
		if (hbit) DrawBitmap(hdc, 0, 0, hbit); //hbit을 hdc를 이용해 출력
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		MyWavSound.StopDevice();
		MyWavSound.CloseDeivce();
		delete[] buffer;
		if (hbit) DeleteObject(hbit);

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
