#pragma once
#include"CommonHeader.h"
#include"vector2d.h"

//change custom ui to gdiplus base

namespace CustomUI {
	Gdiplus::Color ConvertColorRefToGdiPlusColor(COLORREF colorRef);
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

		virtual void Set(vector2d _p1, vector2d _p2, const TCHAR* _Btext,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id);

		void SetActive(bool active);
		bool DetectMouse(vector2d mousepos);
		virtual void isHighlight(vector2d mousepos);
		virtual void Click(vector2d mousepos);
		virtual void Release(vector2d mousepos);
		void Render(HDC hdc);
		virtual void Render(Gdiplus::Graphics* p_graphic);
	};

	class GrapButton : public Button {
	public:
		bool grapped;
	public:
		GrapButton();
		virtual void Set(vector2d _p1, vector2d _p2,
			COLORREF _default, COLORREF _highlight, HWND _parent, int _id);
		void Move(vector2d mousepos);
		void Grap(vector2d mousepos);
		void Release(vector2d mousepos);
		void Render_GrapButton(Gdiplus::Graphics* p_graphic);
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
		void Render(Gdiplus::Graphics* p_graphic);
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
		float default_plansize;
		float PlaneSize;
		//Cube defaultCube;
		Cube stateCube[4];
		Cube currentCube;
		Cube TransformVelocity;

		PlaneState CurrentState;
		PlaneState NextState;

		std::vector<Button> btnlist;
		GrapButton grapbtn;


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
		void update_mousedown(vector2d mousepos);
		void update_mouseup(vector2d mousepos);
		void update_mousemove(vector2d mousepos);
		void Render(HDC hdc, float deltaT);
		void Render(Gdiplus::Graphics* p_graphic, float deltaT);
	};
}