#pragma once
#include"CommonHeader.h"
/*
parse mp3 stream link from youtube link by yt-dlp.exe
*/

class youtube_parser {
private:
public:
	youtube_parser();
	std::string get_mp3_stream_url(std::string youtube_url);
	~youtube_parser();
};

LRESULT CALLBACK Parser_WndProc(HWND, UINT, WPARAM, LPARAM);

class youtube_parser_ui {
private:
	HWND hWnd;
	std::string audio_url;
public:
	youtube_parser_ui();
	void Create(HINSTANCE, HWND);
	const char* get_audio_url();
	~youtube_parser_ui();
};