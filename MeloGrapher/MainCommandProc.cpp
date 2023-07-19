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
    case 12:
        MoveWindow(main_data);
        return;
	case 13:
		DestroyWindow(main_data->this_window->w_hWnd);
		return;
	default:
		return;
	}
}

void MoveWindow(MeloWndData* main_data) {

    POINT cursorPos; // 마우스 좌표를 저장할 POINT 구조체
    GetCursorPos(&cursorPos); // 마우스의 화면 좌표 가져오기

    main_data->this_window->SetScreenLocation(cursorPos.x - (main_data->this_window->screenWidth/2), cursorPos.y - (main_data->this_window->screenHeight / 2));
    main_data->this_window->Apply();
}


int CommandOpenFile(MeloWndData* main_data) {

    SetWindowPos(main_data->this_window->w_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        main_data->block_wrapper->close_file(); //close if file is opened
                        main_data->block_wrapper->open_file(tchar2char(pszFilePath));
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    SetWindowPos(main_data->this_window->w_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    return 0;
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