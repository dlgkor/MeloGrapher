#include"WindowWrapper.h"

MeloWindow::MeloWindow(HINSTANCE _hInstance) {
	hInstance = _hInstance;
}

//initialize root data
int MeloWindow::set_root_data() {
	if (root_hwnd == NULL)
		return -1;

	root_data.block_wrapper = &block_wrapper;
	SetWindowLongPtr(root_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&root_data));
	return 0;
}


//create root wnd
int MeloWindow::create_root() {
	LPCTSTR lpszClass = TEXT("melo_grapher_root");

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.lpfnWndProc = WndProc_Root;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClass;
	wcex.cbWndExtra = sizeof(MeloRootWndData); //extra memory for setptr and getptr
	RegisterClassEx(&wcex);

	root_hwnd = CreateWindow(lpszClass, L"MeloGrapher", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, NULL, NULL, hInstance, NULL);
	
	set_root_data();
	ShowWindow(root_hwnd, SW_HIDE);

	nid = { sizeof(NOTIFYICONDATA) };
	nid.hWnd = root_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	wcscpy_s(nid.szTip, L"MeloGrapher Tray Icon");

	Shell_NotifyIcon(NIM_ADD, &nid);

	return 0;
}

//destroy tray icon of root wnd
int MeloWindow::destroy_root() {
	Shell_NotifyIcon(NIM_DELETE, &nid);

	return 0;
}

int MeloWindow::set_main_data() {
	main_data.block_wrapper = &block_wrapper;
	main_data.root_hwnd = root_hwnd;
	main_data.this_window = &main_window;

	main_data.spectrum_option.s_gap = 32768 / 16;
	main_data.spectrum_option.s_window = 32768 / 4;
	main_data.spectrum_option.s_window_half = main_data.spectrum_option.s_window / 2;	
	main_data.spectrum_option.base_frequency = 44100.0 / (double)main_data.spectrum_option.s_window;

	main_data.spectrum_option.max_out_frequency = 4000;
	main_data.spectrum_option.min_out_frequency = 10;
	main_data.spectrum_option.max_height = 150;
	main_data.spectrum_option.n_graph = 200;
	main_data.spectrum_option.r_center = { 0, 0 };
	main_data.spectrum_option.radius = 200;

	main_data.custom_menu.ResetMenu(main_data.this_window->w_hWnd);

	SetWindowLongPtr(main_window.w_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&main_data));
	return 0;
}

//create main window
int MeloWindow::create_main() {
	LPCTSTR lpszClass = TEXT("melo_grapher_main");

	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = sizeof(MeloWndData);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc_Main;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int sceen_height = GetSystemMetrics(SM_CYSCREEN);

	main_window.setScreenSize(700, 700);
	main_window.SetScreenLocation((screen_width - main_window.screenWidth) / 2, (sceen_height - main_window.screenHeight) / 2);

	main_window.w_hWnd = CreateWindow(lpszClass, L"MeloGrapher", WS_OVERLAPPEDWINDOW,
		main_window.locationX, main_window.locationY, main_window.screenWidth, main_window.screenHeight,
		root_hwnd, (HMENU)NULL, hInstance, NULL); //root 윈도우와 소유-피소유 관계를 설정해준다

	set_main_data();
	return 0;
}

int MeloWindow::show_main() {
	//SetWindowLongPtr(main_window.w_hWnd, GWL_STYLE, 0); //테두리를 없애준다
	
	SetWindowPos(main_window.w_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	ShowWindow(main_window.w_hWnd, SW_SHOW);
	return 0;
}

//final wrapper
int MeloWindow::wnd_main() {
	ULONG_PTR gpToken;
	Gdiplus::GdiplusStartupInput gpsi;
	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Gdiplus::Ok) return 0;

	MSG Message;

	create_root();
	create_main();
	show_main();

	//translate and dispatch message
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	destroy_root();

	Gdiplus::GdiplusShutdown(gpToken);
	return (int)Message.wParam;
}

MeloWindow::~MeloWindow() {

}