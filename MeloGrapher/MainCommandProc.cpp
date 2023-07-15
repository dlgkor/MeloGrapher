#include"WndProc.h"

void CommandProc(MeloWndData* main_data, WPARAM wParam) {
	switch (LOWORD(wParam))
	{
	case 10:
		CommandOpenFile(main_data);
		return;
	case 11:
		main_data->block_wrapper->seek(0);
		main_data->block_wrapper->play_file();
		return;
	case 13:
		DestroyWindow(main_data->this_window->w_hWnd);
		return;
	default:
		return;
	}
}


int CommandOpenFile(MeloWndData* main_data) {
	OPENFILENAME OFN;
	TCHAR szFileName[512];

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = main_data->this_window->w_hWnd;
	OFN.lpstrFilter = L"모든 파일\0*.*\0mp3_file\0*.mp3";
	OFN.lpstrFile = szFileName;
	OFN.nMaxFile = 512;
	OFN.lpstrInitialDir = L".";

	if (0 == GetOpenFileName(&OFN)) {
		MessageBeep(0);
		return -1;
	}

	main_data->block_wrapper->close_file(); //close if file is opened
	main_data->block_wrapper->open_file(tchar2char(szFileName));
}

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