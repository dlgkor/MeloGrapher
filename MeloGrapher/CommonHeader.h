#pragma once

#include<Windows.h>
#pragma comment(lib, "winmm.lib")
#include"resource.h"

#include<gdiplus.h>
#pragma comment(lib, "gdiplus")

#include<iostream>
#include<memory>

#include<vector>
#include<complex>
typedef std::complex<double> cpx;

#include<future>
#include<thread>
#include<chrono>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include<libavformat/avformat.h>
}

#include<math.h>
//#define M_PI 3.141592