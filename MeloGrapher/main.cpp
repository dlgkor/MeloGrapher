#include"common.h"
#include"meloWrapper.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain;
LPCTSTR lpszClass = TEXT("MeloGrapher");

HBITMAP hbit;

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


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	ULONG_PTR gpToken;
	Gdiplus::GdiplusStartupInput gpsi;

	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Gdiplus::Ok) return 0;

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

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 700, 700,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	Gdiplus::GdiplusShutdown(gpToken);
	return (int)Message.wParam;
}

melo::Player* melo_player;

void paintFunction(HWND hWnd, HDC hdc) {
	HDC hMemDC;
	HBITMAP OldBit;
	HBRUSH dbrush;
	RECT crt;

	Gdiplus::Graphics* p_graphic;
	Gdiplus::Pen* p_pen;

	//GetWindowRect(hWnd, &crt);
	crt.left = 0;
	crt.top = 0;
	crt.right = 700;
	crt.bottom = 700;

	if (hbit == NULL) {
		hbit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom); //hdc에 호환된는 윈도우 크기의 비트맵 생성
	}

	hMemDC = CreateCompatibleDC(hdc); //hdc와 호환되는 dc 생성
	OldBit = (HBITMAP)SelectObject(hMemDC, hbit); //hbit을 hmemdc에 선택

	dbrush = CreateSolidBrush(RGB(200, 200, 200));
	FillRect(hMemDC, &crt, dbrush);
	DeleteObject(dbrush);

	SetViewportOrgEx(hMemDC, crt.right / 2, crt.bottom / 2, NULL); //원점을 중간으로 설정

	p_graphic = new Gdiplus::Graphics(hMemDC);
	p_graphic->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	melo_player->checkSpectrum();
	melo_player->displaySpectrum(p_graphic);

	delete p_graphic;

	SelectObject(hMemDC, OldBit);
	DeleteDC(hMemDC);
	if (hbit) DrawBitmap(hdc, 0, 0, hbit); //hbit을 hdc를 이용해 출력
}

void waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg != WOM_DONE) return;
	//s_buffer->fill_buffer(s_decoder);
	
	if (!melo_player->is_muisc_start())
		return;

	while (!melo_player->next_buffer_filled());

	melo_player->WriteWaveBuffer();
	return;
}

static void CALLBACK waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	waveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	//static RECT crt;
	PAINTSTRUCT ps;


	switch (iMessage) {
	case WM_CREATE:
		hWndMain = hWnd;

		melo_player = new melo::Player();
		melo_player->readAudio("D:/mp3/Sneaky Driver.mp3");
		melo_player->setSpecturmOption();
		
		SetTimer(hWnd, 1, 10, NULL);

		hbit = NULL;
		return 0;
	case WM_LBUTTONDOWN:
		melo_player->setWaveOut(waveOutProcWrap);
		melo_player->Start();
		return 0;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		paintFunction(hWnd, hdc);
		
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		if (hbit) DeleteObject(hbit);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}