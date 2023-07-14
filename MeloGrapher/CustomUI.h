#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include<math.h>
#include<vector>
#include"vector2d.h"

//change custom ui to gdiplus base

namespace CustomUI {
	class Button {
	public:
		bool Active;
		bool Highlight;
		vector2d p1, p2;
		TCHAR Btext[128];

		COLORREF defaultColor;
		COLORREF highlightColor;
		HWND parent;

		int id;

	public:
		Button();
		Button(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id);

		void Set(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id);

		void SetActive(bool active);
		bool DetectMouse(vector2d mousepos);
		void isHighlight(vector2d mousepos);
		void Click(vector2d mousepos);
		void Render(HDC hdc);
	};


	class Plane {
	public:
		vector2d p[4];
		vector2d p_normal[4];
		COLORREF color;
	public:
		void UpdateNormal();
		bool IsInsideLine(vector2d normalv, vector2d point);
		bool Detect(vector2d mousepos);
		void Render(HDC hdc);
	};

	class Cube {
	public:
		Plane plane[3];
	};

	enum PlaneState {
		DEFUALT = 0, PLANE1 = 1, PLANE2 = 2, PLANE3 = 3, NONE
	};

	class Menu {
	private:
		float PlaneSize;
		//Cube defaultCube;
		Cube stateCube[4];
		Cube currentCube;
		Cube TransformVelocity;

		PlaneState CurrentState;
		PlaneState NextState;

		int btnnum;
		//Button btnList[10];
		std::vector<Button> btnlist;

		float transitionTime;
		float accumulatedTime;

		bool targetChange;
		bool animationOn;
	public:
		Menu();
		void ResetMenu(HWND hwnd);
		PlaneState DetectMouse(vector2d mousepos);
		void TransitionAnimation(float deltaT);
		void Update(vector2d mousepos, bool clicked);
		void Render(HDC hdc, float deltaT);
	};
}