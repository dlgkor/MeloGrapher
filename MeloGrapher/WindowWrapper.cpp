#include"WindowWrapper.h"

MeloWindow::MeloWindow(HINSTANCE _hInstance) {
	hInstance = _hInstance;
}

int MeloWindow::set_common_data() {
	wnd_data.spectrum_option.s_gap = 32768 / 16; //will change to 32768/32
	wnd_data.spectrum_option.s_window = 32768 / 4;
	wnd_data.spectrum_option.s_window_half = wnd_data.spectrum_option.s_window / 2;
	wnd_data.spectrum_option.base_frequency = 44100.0 / (double)wnd_data.spectrum_option.s_window;

	wnd_data.spectrum_option.max_out_frequency = 1000;
	wnd_data.spectrum_option.min_out_frequency = 10;
	wnd_data.spectrum_option.max_height = 200;
	wnd_data.spectrum_option.n_graph = 150;
	wnd_data.spectrum_option.r_center = { 0, 0 };
	wnd_data.spectrum_option.radius = 220;
	//develop spectrum option load_save method
	wnd_data.spectrum_option.set_circle_points();

	wnd_data.block_wrapper = &block_wrapper;
	wnd_data.block_wrapper->set_buffer_manager(wnd_data.spectrum_option);

	//set block_wrapper.buffer_manger spectrum window and gap

	return 0;
}


//create root wnd
int MeloWindow::create_root() {
	LPCTSTR lpszClass = TEXT("melo_grapher_root");

	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.lpfnWndProc = WndProc_Root;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = lpszClass;
	wcex.cbWndExtra = sizeof(MeloRootData*); //extra memory for setptr and getptr
	RegisterClassEx(&wcex);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int sceen_height = GetSystemMetrics(SM_CYSCREEN);
	root_window.setScreenSize(800, 800);
	root_window.SetScreenLocation((screen_width - root_window.screenWidth) / 2, (sceen_height - root_window.screenHeight) / 2);

	root_hwnd = CreateWindow(lpszClass, L"MeloGrapher", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, NULL, NULL, hInstance, NULL);
	root_window.w_hWnd = root_hwnd;
	root_window.Apply();
	
	SetWindowLong(root_hwnd, GWL_EXSTYLE, GetWindowLong(root_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetLayeredWindowAttributes(root_hwnd, RGB(0, 0, 255), 0, LWA_COLORKEY);

	nid = { sizeof(NOTIFYICONDATA) };
	nid.hWnd = root_hwnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MAIN_ICON));
	wcscpy_s(nid.szTip, L"MeloGrapher Tray Icon");

	Shell_NotifyIcon(NIM_ADD, &nid);

	set_root_data();

	ShowWindow(root_hwnd, SW_SHOW);
	return 0;
}

int MeloWindow::set_root_data() {
	root_data.common_data = &wnd_data;
	root_data.this_window = &root_window;
	SetWindowLongPtr(root_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&root_data));

	SetWindowPos(root_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return 0;
}

int MeloWindow::show_root() {
	//SetWindowLongPtr(root_hwnd, GWL_STYLE, 0); //테투리 제거

	//SetWindowPos(root_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//ShowWindow(root_hwnd, SW_SHOW);
	return 0;
}

//destroy tray icon of root wnd
int MeloWindow::destroy_root() {
	Shell_NotifyIcon(NIM_DELETE, &nid);

	return 0;
}

//create main window
int MeloWindow::create_main() {
	LPCTSTR lpszClass = TEXT("melo_grapher_main");

	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = sizeof(MeloMainData*);
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
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	main_window.setScreenSize(250, 250);
	main_window.SetScreenLocation((screen_width - main_window.screenWidth) / 2, (screen_height - main_window.screenHeight) / 2);
	main_hWnd = CreateWindow(lpszClass, L"MeloGrapher", WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		root_hwnd, (HMENU)NULL, hInstance, NULL); //root 윈도우와 소유-피소유 관계를 설정해준다
	main_window.w_hWnd = main_hWnd;
	main_window.Apply();
	
	main_data.custom_menu.ResetMenu(main_hWnd);

	set_main_data();

	ShowWindow(main_hWnd, SW_SHOW);
	return 0;
}

int MeloWindow::set_main_data() {
	main_data.common_data = &wnd_data;
	main_data.this_window = &main_window;
	main_data.root_hwnd = root_hwnd;
	SetWindowLongPtr(main_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&main_data));

	//SetWindowPos(main_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return 0;
}

int MeloWindow::show_main() {
	//SetWindowLongPtr(main_hWnd, GWL_STYLE, 0); //테투리 제거
	//SetWindowPos(main_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//ShowWindow(main_hWnd, SW_SHOW);
	return 0;
}

//final wrapper
int MeloWindow::wnd_main() {
	ULONG_PTR gpToken;
	Gdiplus::GdiplusStartupInput gpsi;
	if (GdiplusStartup(&gpToken, &gpsi, NULL) != Gdiplus::Ok) return 0;

	MSG Message;

	set_common_data();
	create_root();
	create_main();

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