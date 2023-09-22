#include "YoutubeParser.h"

youtube_parser::youtube_parser() {

}

std::string youtube_parser::get_mp3_stream_url(std::string youtube_url) {
	std::string command = "yt-dlp.exe -g ";

	command += youtube_url;

	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe) return "";
	char mybuffer[128];
	std::string result = "";

	while (!feof(pipe)) {
		if (fgets(mybuffer, 128, pipe) != NULL)
			result += mybuffer;
	}
	_pclose(pipe);

	size_t pos = result.find('\n');
	std::string audio_link = result.substr(pos + 1);

	return audio_link;
}

youtube_parser::~youtube_parser() {

}

LRESULT CALLBACK Parser_WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static HWND hEdit;
	WCHAR buffer[500];
	static youtube_parser yt_parser;
	std::string* audio_url = reinterpret_cast<std::string*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	std::wstring* wideString;
	std::string* narrowstring;

	switch (iMessage) {
	case WM_CREATE:
		hEdit = CreateWindow(TEXT("edit"), TEXT("edit"), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, 10, 10, 200, 20, hWnd, (HMENU)1, GetModuleHandle(nullptr), NULL);
		CreateWindow(TEXT("button"), TEXT("Open"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 10, 50, 20, hWnd, (HMENU)2, GetModuleHandle(nullptr), NULL);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 1:
			return 0;
		case 2:
			
			GetWindowText(hEdit, buffer, 500);
			wideString = new std::wstring(buffer);
			narrowstring = new std::string((*wideString).begin(), (*wideString).end());
			*audio_url = yt_parser.get_mp3_stream_url(*narrowstring);
			delete wideString;
			delete narrowstring;
			SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(15, 0), NULL);
			
			DestroyWindow(hWnd);
			return 0;
		default:
			return 0;
		}
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

youtube_parser_ui::youtube_parser_ui() {

}

void youtube_parser_ui::Create(HINSTANCE hInstance, HWND hParent) {
	LPCTSTR lpszClass = TEXT("melographer_youtubeparser");
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.lpfnWndProc = Parser_WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClass;
	wcex.cbWndExtra = sizeof(std::string*);
	RegisterClassEx(&wcex);

	RECT parentRect;
	GetWindowRect(hParent, &parentRect);

	// 자식 윈도우의 위치 설정 (예: 부모 윈도우의 오른쪽에 생성)
	int childX = (parentRect.right + parentRect.left) / 2 - 200; // 예: 오른쪽으로 10 픽셀 떨어진 곳에 생성
	int childY = (parentRect.top + parentRect.bottom) / 2 - 50; // 부모 윈도우와 동일한 높이에서 생성

	hWnd = CreateWindow(lpszClass, L"MeloGrapher", WS_CHILD | WS_POPUPWINDOW | WS_CAPTION, childX, childY,
		400, 100, hParent, (HMENU)NULL, hInstance, NULL);

	//change to dialog

	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&audio_url));

	ShowWindow(hWnd, SW_SHOW);
}

const char* youtube_parser_ui::get_audio_url() {
	return audio_url.c_str();
}

youtube_parser_ui::~youtube_parser_ui() {

}